
#pragma once

#include <optional>
#include <chrono>

#include "coroutine/coroutine.h"
#include "http/HTTP.hpp"

namespace http {

struct ClientState {
	ClientState(Coroutine::routine_t new_id);
	ClientState(const ClientState& other) = default;
	
	Coroutine::routine_t id;
	bool keep_alive;
	enum { ReadingRequest, WritingResponse } process_state;
	std::chrono::time_point<std::chrono::system_clock> start;

	bool timed_out() const;

	std::optional<std::pair<std::vector<char>, std::size_t>> readHead(int fd);
	std::optional<std::string> readBody(int fd, std::size_t len, std::vector<char> body);
	std::optional<http::HTTP::Request> readRequest(int fd);
	bool sendResponse(int fd, const http::HTTP::Response& response);
};

}
