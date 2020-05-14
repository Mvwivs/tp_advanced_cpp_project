
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

#include "http/fd.hpp"
#include "coroutine/coroutine.h"
#include "tcp/connection_utils.hpp"
#include "http/HTTP.hpp"
#include "http/ClientState.hpp"

namespace http {

using FormResponse_cb = std::function<http::HTTP::Response(const http::HTTP::Request&)>;

class Worker {
public:

	Worker(FormResponse_cb callback);
	Worker(Worker&& other);

	Worker(const Worker& other) = delete;
	Worker& operator=(const Worker& other) = delete;

	void addClient(int client_fd);

	void run();

	void stop();

private:
	bool handleClient(int fd);

	void serveClient(int fd);

private:
	std::atomic_bool running;
	std::unordered_map<int, ClientState> clients;
	fd_t epoll;

	FormResponse_cb formResponse;
};

}
