
#pragma once

#include <chrono>

#include "coroutine/coroutine.h"

namespace http {

struct ClientState {
	ClientState(Coroutine::routine_t new_id);
	ClientState(const ClientState& other) = default;
	
	Coroutine::routine_t id;
	bool keep_alive;
	enum { ReadingRequest, WritingResponse } process_state;
	std::chrono::time_point<std::chrono::system_clock> start;

	bool timed_out() const;
};

}
