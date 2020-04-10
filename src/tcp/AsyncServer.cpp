
#include "tcp/AsyncServer.hpp"

#include <array>
#include <cstring>
#include <utility>

#include <sys/epoll.h>
#include <netinet/in.h>

namespace tcp {

using namespace std::string_literals;

AsyncServer::AsyncServer(const Address& address, Callback callback, const EventSet& events) : 
	epoll_fd_(-1),
	server_fd_(-1),
	callback_(callback),
	events_(events) {

	epoll_fd_ = epoll_create1(0);
	if (epoll_fd_ == -1) {
		close();
		throw ConnectionException("Unable create epoll: "s + std::strerror(errno));
	}

	openServer(address);

	epoll_event e{};
	e.data.fd = server_fd_;
	e.events = EPOLLIN;
	int res = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, server_fd_, &e);
	if (res == -1) {
		close();
		throw ConnectionException("Unable to add to server epoll: "s + std::strerror(errno));
	}
}

AsyncServer::AsyncServer(AsyncServer&& other):
	epoll_fd_(std::exchange(other.epoll_fd_, -1)),
	server_fd_(std::exchange(other.server_fd_, -1)),
	callback_(std::move(other.callback_)),
	connections_(std::move(other.connections_)),
	events_(std::move(other.events_)) {
}

AsyncServer& AsyncServer::operator=(AsyncServer&& other) {
	close();
	epoll_fd_ = std::exchange(other.epoll_fd_, -1);
	server_fd_ = std::exchange(other.server_fd_, -1);
	callback_ = std::move(other.callback_);
	connections_ = std::move(other.connections_);
	events_ = std::move(other.events_);
	return *this;
}

AsyncServer::~AsyncServer() {
	close();
}

void AsyncServer::close() {
	close_fd(server_fd_);
	close_fd(epoll_fd_);
}

void AsyncServer::run() {
	constexpr std::size_t epoll_size = 128;
	std::array<epoll_event, epoll_size> events;

	while (true) {
		int recieved = epoll_wait(epoll_fd_, events.data(), epoll_size, -1);
		if (recieved < 0) {
			if (errno == EINTR) {
				continue;
			}
			throw ConnectionException("Epoll wait error: "s + std::strerror(errno));
		}

		for (std::size_t i = 0; i < recieved; ++i) {
			int fd = events[i].data.fd;
			uint32_t event = events[i].events;

			if (fd == server_fd_) {
				acceptClient();
			}
			else {
				handleClient(fd, event);
			}
		}
	}
}

void AsyncServer::handleClient(int socket, uint32_t event) {
	EventSet recieved_events;

	if ((event & EPOLLHUP) || (event & EPOLLERR)) {
		recieved_events.add(Event::Closed);	// closed unexpectedly
	}

	if (event & EPOLLIN) {
		recieved_events.add(Event::ReadRdy);
	}
	if (event & EPOLLOUT) {
		recieved_events.add(Event::WriteRdy);
	}
	if (event & EPOLLRDHUP) {
		recieved_events.add(Event::Closed);	// normal close
	}

	AsyncConnection& connection = connections_.find(socket)->second;
	connection.setEvents(recieved_events);
	callback_(connection);

	if (connection.hasEvent(Event::Closed)) {
		connections_.erase(socket);
	}
}

void AsyncServer::acceptClient() {
	sockaddr_in client_addr{};
	socklen_t client_len = sizeof(client_addr);
	int client_fd = -1;
	while (true) {
		client_fd = accept4(server_fd_, reinterpret_cast<sockaddr*>(&client_addr), 
			&client_len, SOCK_NONBLOCK);
		if (client_fd == -1) {
			if (errno == EINTR) {
				continue;
			}
			else if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return;
			}
			else {
				throw ConnectionException("Unable to accept: "s + std::strerror(errno));
			}
		}
		addEpoll(client_fd, events_);
		return;
	}
}

void AsyncServer::addEpoll(int socket, const EventSet& events) {
	epoll_event e{};
	e.data.fd = socket;
	std::uint32_t triggering_events = 0;
	if (events_.isIn(Event::ReadRdy)) {
		triggering_events = triggering_events | EPOLLIN;
	}
	if (events_.isIn(Event::WriteRdy)) {
		triggering_events = triggering_events | EPOLLOUT;
	}
	if (events_.isIn(Event::Closed)) {
		triggering_events = triggering_events | EPOLLRDHUP;
	}
	e.events = triggering_events;
	int res = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket, &e);
	if (res == -1) {
		throw ConnectionException("Unable to add to epoll: "s + std::strerror(errno));
	}
	connections_.emplace(std::make_pair(socket, AsyncConnection{socket}));
}

void AsyncServer::openServer(const Address& address) {
	server_fd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_fd_ == -1) {
		close();
		throw ConnectionException("Unable to create server socket: "s + std::strerror(errno));
	}

	int enable = 1;
	int res = setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	if (res == -1) {
		close();
		throw ConnectionException("Unable to set SO_REUSEADDR for server socket: "s + std::strerror(errno));
	}

	sockaddr_in local_address{};
	local_address.sin_family = AF_INET;
	local_address.sin_addr.s_addr = htonl(address.ip);
	local_address.sin_port = htons(address.port);

	res = bind(server_fd_, reinterpret_cast<sockaddr*>(&local_address), sizeof(local_address));
	if (res == -1) {
		close();
		throw ConnectionException("Unable to bind server socket: "s + std::strerror(errno));
	}

	res = listen(server_fd_, 100);
	if (res == -1) {
		close();
		throw ConnectionException("Unable to mark server socket as listening: "s + std::strerror(errno));
	}
}

}
