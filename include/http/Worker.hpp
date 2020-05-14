
#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <thread>
#include <cstdint>
#include <atomic>
#include <unordered_map>
#include <optional>
#include <iostream>
#include <string_view>
#include <cstring>
#include <functional>
#include <chrono>

#include <sys/epoll.h>
#include <netinet/in.h>

#include "fd.hpp"
#include "coroutine/coroutine.h"
#include "tcp/connection_utils.hpp"
#include "http/HTTP.hpp"

using namespace std::string_literals;

namespace http {

using FormResponse_cb = std::function<http::HTTP::Response(const http::HTTP::Request&)>;

struct ClientState {
	ClientState(Coroutine::routine_t new_id) :
		id(new_id),
		keep_alive(false),
		process_state(ReadingRequest),
		start(std::chrono::system_clock::now()) {
	}
	ClientState(const ClientState& other) = default;
	Coroutine::routine_t id;
	bool keep_alive;
	enum { ReadingRequest, WritingResponse } process_state;

	std::chrono::time_point<std::chrono::system_clock> start;
	bool timed_out() const {
		auto now = std::chrono::system_clock::now();
		auto limit = std::chrono::seconds(10);
		return ((now - start) > limit);
	}
};

class Worker {
public:

	Worker(FormResponse_cb callback) :
		running(false),
		formResponse(callback) {
		epoll = std::move(fd_t{epoll_create1(0)});
		if (!epoll) {
			throw std::runtime_error("Unable to create epoll: "s + std::strerror(errno));
		}
	}
	Worker(Worker&& other) :
		running(other.running.load()),
		clients(std::move(other.clients)),
		epoll(std::move(other.epoll)),
		formResponse(std::move(other.formResponse)) {
	}

	Worker(const Worker& other) = delete;
	Worker& operator=(const Worker& other) = delete;

	void addClient(int client_fd) {
		epoll_event e{};
		e.data.fd = client_fd;
		e.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
		int res = epoll_ctl(epoll.fd, EPOLL_CTL_ADD, client_fd, &e);
		if (res == -1) {
			throw std::runtime_error("Unable to add to epoll: "s + std::strerror(errno));
		}
	}

	void run() {
		running = true;
		while (running) {
			constexpr std::size_t epoll_size = 100;
			std::array<epoll_event, epoll_size> events;
			int recieved = epoll_wait(epoll.fd, events.data(), epoll_size, 2000);
			if (recieved < 0) {
				if (errno == EINTR) { // check timeout on connections
					continue;
				}
				throw std::runtime_error("Epoll wait error: "s + std::strerror(errno));
			}
			if (recieved == 0) {
				for(auto it = clients.begin(); it != clients.end(); ) {
					if (handleClient(it->first, 0)) {
						it = clients.erase(it);
					} else {
						++it;
					}
				}
				continue;
			}

			for (int i = 0; i < recieved; ++i) {
				int fd = events[i].data.fd;
				uint32_t event = events[i].events;

				if (clients.find(fd) == clients.end()) {
					Coroutine::routine_t id = Coroutine::create(
						[this, fd] { this->serveClient(fd); });
					clients.emplace(fd, ClientState{id});
				}
				bool erase = handleClient(fd, event);
				if (erase) {
					clients.erase(fd);
				}
			}
		}
		// close all clients
		for(auto it = clients.begin(); it != clients.end(); ) {
			if (handleClient(it->first, 0)) {
				::close(it->first);
				Coroutine::force_finish(it->second.id);
				it = clients.erase(it);
			} else {
				++it;
			}
		}
	}

	void stop() {
		running = false;
	}

private:
	bool handleClient(int fd, uint32_t event) {
		ClientState& client = clients.at(fd);
		bool resume_res = Coroutine::resume(client.id);
		bool finished = Coroutine::finished(client.id);
		if (finished) {
			if (!client.keep_alive || client.timed_out()) { // close
				::close(fd);
				return true;
			}
		}
		bool reading = true;
		if (client.process_state == ClientState::WritingResponse) {
			reading = false;
		}

		epoll_event e{};
		e.data.fd = fd;
		if (reading) {
			e.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
		}
		else {
			e.events = EPOLLOUT | EPOLLET | EPOLLONESHOT;
		}
		int res = epoll_ctl(epoll.fd, EPOLL_CTL_MOD, fd, &e);
		if (res == -1) {
			std::cerr << std::strerror(errno) << std::endl;
			throw std::runtime_error("Unable to re-add to epoll: "s + std::strerror(errno));
		}
		return false;
	}

	void serveClient(int fd) {
		ClientState& client = clients.at(fd);
		while (client.process_state == ClientState::ReadingRequest) {
			std::vector<char> full_data;
			http::HTTP::Request request;

			while (true) {
				if (client.timed_out()) {
					return;
				}
				std::array<char, 400> buffer;
				ssize_t recieved = ::read(fd, buffer.data(), buffer.size());
				if (recieved == 0) { // connection closed
					client.keep_alive = false;
					return;
				}
				else if (recieved == -1) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						Coroutine::yield();
						continue;
					}
					throw std::runtime_error("Unable to read: "s + std::strerror(errno));
				}

				full_data.insert(full_data.end(), buffer.begin(), buffer.begin() + recieved);

				constexpr std::string_view endSeq = "\r\n\r\n";
				auto head_end = std::search(full_data.end() - recieved - 3, full_data.end(),
					endSeq.begin(), endSeq.end());
				if (head_end != full_data.end()) { // found \r\n\r\n in header
					request = http::HTTP::Request(std::string(full_data.begin(), head_end + 2));
					try {
						const std::string& type = request.getHeader("Connection");
						if (type == "Keep-Alive") {
							client.keep_alive = true;
						}
					}
					catch (const http::HTTP::ParsingException&) { // no keep-alive
					}
					try {
						const std::string& len = request.getHeader("Content-Length");
						std::size_t body_len = std::stoull(len);

						std::vector<char> body(head_end + 4, full_data.end());
						std::size_t to_read = body_len - body.size();
						while (to_read != 0) {
							if (client.timed_out()) {
								return;
							}
							std::array<char, 400> buffer;
							ssize_t recieved = ::read(fd, buffer.data(), buffer.size());
							if (recieved == 0) { // connection closed
								client.keep_alive = false;
								return;
							}
							else if (recieved == -1) {
								if (errno == EAGAIN || errno == EWOULDBLOCK) {
									Coroutine::yield();
									continue;
								}
								throw std::runtime_error("Unable to read: "s + std::strerror(errno));
							}
							body.insert(body.end(), buffer.begin(), buffer.begin() + recieved);
							to_read -= recieved;
						}
						request.body = std::string(body.begin(), body.end());
					}
					catch (const http::HTTP::ParsingException&) { // no body, finish reading
					}
					break;
				}
			}

			http::HTTP::Response resp = formResponse(request);

			std::string resp_str = resp.to_string();
			client.process_state = ClientState::WritingResponse;

			size_t written = 0;
			while (resp_str.size() - written != 0) {
				if (client.timed_out()) {
					return;
				}
				ssize_t res = ::write(fd, resp_str.data() + written, resp_str.size() - written);
				if (res == 0) {
					throw std::runtime_error("Unable to write, EOF: "s + std::strerror(errno));
				}
				else if (res == -1) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						Coroutine::yield();
						continue;
					}
					throw std::runtime_error("Unable to write: "s + std::strerror(errno));
				}
				written += res;
			}
			if (client.keep_alive) {
				client.process_state = ClientState::ReadingRequest;
			}
		}
		return;
	}

private:
	std::atomic_bool running;
	std::unordered_map<int, ClientState> clients;
	fd_t epoll;

	FormResponse_cb formResponse;
};

}
