
#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <thread>
#include <cstdint>
#include <atomic>
#include <unordered_map>
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
		keep_alive(false) {
	}
	Coroutine::routine_t id;
	bool keep_alive;
};

class Server {
public:

	Server(const tcp::Address& address) {
		epoll = std::move(fd_t{epoll_create1(0)});
		if (!epoll) {
			throw std::runtime_error("Unable to create epoll: "s + std::strerror(errno));
		}

		openServer(address);

		epoll_event e{};
		e.data.fd = server.fd;
		e.events = EPOLLIN | EPOLLEXCLUSIVE;
		int res = epoll_ctl(epoll.fd, EPOLL_CTL_ADD, server.fd, &e);
		if (res == -1) {
			throw std::runtime_error("Unable to add to server epoll: "s + std::strerror(errno));
		}
	}

	void run() {
		worker_job();
	}

	// http::HTTP::Response onRequest(const http::HTTP::Request& request) {

	// }

private:
	void openServer(const tcp::Address& address) {
		server = std::move(fd_t{socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)});
		if (!server) {
			throw std::runtime_error("Unable to create server socket: "s + std::strerror(errno));
		}

		int enable = 1;
		int res = setsockopt(server.fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
		if (res == -1) {
			throw std::runtime_error("Unable to set SO_REUSEADDR for server socket: "s + std::strerror(errno));
		}

		sockaddr_in local_address{};
		local_address.sin_family = AF_INET;
		local_address.sin_addr.s_addr = htonl(address.ip);
		local_address.sin_port = htons(address.port);

		res = bind(server.fd, reinterpret_cast<sockaddr*>(&local_address), sizeof(local_address));
		if (res == -1) {
			throw std::runtime_error("Unable to bind server socket: "s + std::strerror(errno));
		}

		res = listen(server.fd, 100);
		if (res == -1) {
			throw std::runtime_error("Unable to mark server socket as listening: "s + std::strerror(errno));
		}
	}

	void worker_job() {
		while (true) {
			constexpr std::size_t epoll_size = 128;
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

				if (fd == server.fd) {
					acceptClient();
				}
				else {
					handleClient(fd, event);
				}
			}

		}
	}

	void acceptClient() {
		sockaddr_in client_addr{};
		socklen_t client_len = sizeof(client_addr);
		while (true) {
			int client_fd = accept4(server.fd, reinterpret_cast<sockaddr*>(&client_addr), 
				&client_len, SOCK_NONBLOCK);
			if (client_fd == -1) {
				if (errno == EINTR) {
					continue;
				}
				else if (errno == EAGAIN || errno == EWOULDBLOCK) {
					return;
				}
				else {
					throw std::runtime_error("Unable to accept: "s + std::strerror(errno));
				}
			}
			epoll_event e{};
			e.data.fd = client_fd;
			e.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLONESHOT;
			int res = epoll_ctl(epoll.fd, EPOLL_CTL_ADD, client_fd, &e);
			if (res == -1) {
				throw std::runtime_error("Unable to add to epoll: "s + std::strerror(errno));
			}
			Coroutine::routine_t id = Coroutine::create(
				[this, client_fd] { this->serveClient(client_fd); });
			clients.emplace(client_fd, ClientState{id});
			return;
		}
	}

	void handleClient(int fd, uint32_t event) {
		ClientState& client = clients.at(fd);
		bool finished = Coroutine::resume(client.id);
		if (finished) {
			if (!client.keep_alive) { // close
				::close(fd);
				clients.erase(fd);
				return;
			}
		}
		// check timeout
		
		epoll_event e{};
		e.data.fd = fd;
		e.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLONESHOT;
		int res = epoll_ctl(epoll.fd, EPOLL_CTL_MOD, fd, &e);
		if (res == -1) {
			std::cerr << std::strerror(errno) << std::endl;
			throw std::runtime_error("Unable to re-add to epoll: "s + std::strerror(errno));
		}
	}

	void serveClient(int fd) {
		std::vector<char> full_data;
		http::HTTP::Request request;

		while (true) {
			std::array<char, 400> buffer;
			ssize_t recieved = ::read(fd, buffer.data(), buffer.size());
			if (recieved == 0) { // connection closed
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
					const std::string& len = request.getHeader("Content-Length");
					std::size_t body_len = std::stoull(len);

					std::vector<char> body(head_end + 4, full_data.end());
					std::size_t to_read = body_len - body.size();
					while (to_read != 0) {
						std::array<char, 400> buffer;
						ssize_t recieved = ::read(fd, buffer.data(), buffer.size());
						if (recieved == 0) { // connection closed
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

		http::HTTP::Response resp {
			{http::HTTP::Version::HTTP_1_1, http::HTTP::StatusCode{404}}
		};

		std::string resp_str = resp.to_string();

		size_t written = 0;
		while (resp_str.size() - written != 0) {
			ssize_t res = ::write(fd, resp_str.data() + written, resp_str.size() - written);;
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
	}

private:
	std::size_t worker_count_;
	std::atomic<bool> running;

	std::unordered_map<int, ClientState> clients;
	fd_t epoll;
	fd_t server;
};

}
