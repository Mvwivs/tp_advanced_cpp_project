
#pragma once

#include <optional>
#include <chrono>

#include "coroutine/coroutine.h"
#include "http/HTTP.hpp"

namespace http {

// Stores client state and provides means to send and recieve http data for client
struct ClientState {
	ClientState(Coroutine::routine_t new_id);
	ClientState(const ClientState& other) = default;
	
	Coroutine::routine_t id;									// execution routine
	bool keep_alive;											// is connection kept alive
	enum { ReadingRequest, WritingResponse } process_state;		// currently reading or writint
	std::chrono::time_point<std::chrono::system_clock> start;	// connected on
	bool timed_out_;											// was marked as timed out

	// check if connection timed out
	bool timed_out() const;
	// read request from `fd` based on client state
	std::optional<http::HTTP::Request> readRequest(int fd);
	// send response to `fd` based on client state
	bool sendResponse(int fd, const http::HTTP::Response& response);
private:
	// read head for http request
	std::optional<std::pair<std::vector<char>, std::size_t>> readHead(int fd);
	// read body for http request
	std::optional<std::string> readBody(int fd, std::size_t len, std::vector<char> body);

};

}
