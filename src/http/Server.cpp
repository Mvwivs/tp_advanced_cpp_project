
#include "http/Server.hpp"

#include <cstring>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std::string_literals;

namespace http {

Server::Server(const tcp::Address& address, std::size_t worker_count):
	
	stdout_log(logger::create_stdout_logger()),
	stderr_log(logger::create_stderr_logger()),
	file_log(logger::create_file_logger("server.log")),

	running(false),
	epoll(epoll_create1(0)),
	load_balancing(0) {
	
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

	workers.reserve(worker_count);
	for (std::size_t i = 0; i < worker_count; ++i) {
		workers.emplace_back(
			[this](const http::HTTP::Request& req) { return this->onRequest(req); });
	}
}

void Server::run() {
	running = true;
	std::vector<std::thread> executors;
	executors.reserve(workers.size());
	for (auto& worker : workers) {
		executors.emplace_back(&Worker::run, std::ref(worker));
	}
	while (running) {
		constexpr std::size_t epoll_size = 100;
		std::array<epoll_event, epoll_size> events;
		int recieved = epoll_wait(epoll.fd, events.data(), epoll_size, 2);
		if (recieved < 0) {
			if (errno == EINTR) {
				continue;
			}
			throw std::runtime_error("Epoll wait error: "s + std::strerror(errno));
		}

		for (int i = 0; i < recieved; ++i) {
			acceptClient();
		}
	}
	for (auto& worker : workers) {
		worker.stop();
	}
	for (auto& exec : executors) {
		exec.join();
	}
	stdout_log->info("Server stopped");
}

void Server::stop() {
	running = false;
}

void Server::openServer(const tcp::Address& address) {
	server = std::move(Fd{socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)});
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

void Server::acceptClient() {
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

		workers[(load_balancing++) % workers.size()].addClient(client_fd); // round-robin
		return;
	}
}

}
