
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

#include <sys/epoll.h>
#include <netinet/in.h>

#include "fd.hpp"
#include "coroutine/coroutine.h"
#include "tcp/connection_utils.hpp"
#include "http/HTTP.hpp"

using namespace std::string_literals;

namespace http {

struct ClientState {
	ClientState(Coroutine::routine_t new_id) :
		id(new_id),
		keep_alive(),
		process_state(ReadingRequest) {
	}
	Coroutine::routine_t id;
	struct KeepAlive {
		int time;
	};
	std::optional<KeepAlive> keep_alive;
	enum { ReadingRequest, WritingResponse } process_state;
};

class Worker {
public:

	Worker() {
		epoll = std::move(fd_t{epoll_create1(0)});
		if (!epoll) {
			throw std::runtime_error("Unable to create epoll: "s + std::strerror(errno));
		}
	}

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
		while (true) {
			constexpr std::size_t epoll_size = 101;
			std::array<epoll_event, epoll_size> events;
			int recieved = epoll_wait(epoll.fd, events.data(), epoll_size, -1);
			if (recieved < 0) {
				if (errno == EINTR) {
					continue;
				}
				throw std::runtime_error("Epoll wait error: "s + std::strerror(errno));
			}

			for (int i = 0; i < recieved; ++i) {
				int fd = events[i].data.fd;
				uint32_t event = events[i].events;

				if (clients.find(fd) == clients.end()) {
					Coroutine::routine_t id = Coroutine::create(
						[this, fd] { this->serveClient(fd); });
					std::cout << "Created coroutine " << id  << '-' << fd << std::endl;
					clients.emplace(fd, ClientState{id});
				}
				handleClient(fd, event);
			}

		}
	}

	// http::HTTP::Response onRequest(const http::HTTP::Request& request) {

	// }

private:
	void handleClient(int fd, uint32_t event) {
		if (event & EPOLLIN) {
			std::cout << "EPOLLIN ";
		}
		if (event & EPOLLOUT) {
			std::cout << "EPOLLOUT ";
		}
		if (event & EPOLLRDHUP) {
			std::cout << "EPOLLRDHUP ";
		}

		ClientState& client = clients.at(fd);
		std::cout << "Resuming coroutine " << client.id  << '-' << fd << std::endl;
		bool resume_res = Coroutine::resume(client.id);
		std::cout << "Exit, resume res: " << resume_res << std::endl;
		bool finished = Coroutine::finished(client.id);
		if (finished) {
			if (!client.keep_alive) { // close
				::close(fd);
				clients.erase(fd);
				std::cout << "client close " << fd << std::endl;
				return;
			}
		}
		bool reading = true;
		if (client.process_state == ClientState::WritingResponse) {
			reading = false;
		}
		// check timeout

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
	}

	void serveClient(int fd) {
		while (clients.at(fd).process_state == ClientState::ReadingRequest) {
			std::vector<char> full_data;
			http::HTTP::Request request;

			while (true) {
				std::array<char, 400> buffer;
				ssize_t recieved = ::read(fd, buffer.data(), buffer.size());
				std::cout << "data recieved " << fd << " " << recieved << std::endl;
				if (recieved == 0) { // connection closed
					clients.at(fd).keep_alive = {};
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
							clients.at(fd).keep_alive = ClientState::KeepAlive{2};
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
							std::array<char, 400> buffer;
							ssize_t recieved = ::read(fd, buffer.data(), buffer.size());
							std::cout << "data recieved " << fd << " " << recieved << std::endl;
							if (recieved == 0) { // connection closed
								clients.at(fd).keep_alive = {};
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
						std::cout << "What" << std::endl;
					}
					break;
				}
			}

			http::HTTP::Response resp {
				{http::HTTP::Version::HTTP_1_1, http::HTTP::StatusCode{200}},
				{{"Connection", "Keep-Alive"}},
				{"<some> HTML </some>"}
			};

			std::string resp_str = resp.to_string();
			clients.at(fd).process_state = ClientState::WritingResponse;

			size_t written = 0;
			while (resp_str.size() - written != 0) {
				ssize_t res = ::write(fd, resp_str.data() + written, resp_str.size() - written);
				std::cout << "data sent " << fd << " " << res << std::endl;
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
			if (clients.at(fd).keep_alive) {
				clients.at(fd).process_state = ClientState::ReadingRequest;
			}
		}
		std::cout << "Exiting " << fd << std::endl;
		return;
	}

private:
	std::size_t worker_count_;
	std::atomic<bool> running;

	std::unordered_map<int, ClientState> clients;
	fd_t epoll;
};

}
