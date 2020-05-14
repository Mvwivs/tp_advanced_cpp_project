
#include "http/ClientState.hpp"

namespace http {

ClientState::ClientState(Coroutine::routine_t new_id) :
	id(new_id),
	keep_alive(false),
	process_state(ReadingRequest),
	start(std::chrono::system_clock::now()) {
}

bool ClientState::timed_out() const {
	auto now = std::chrono::system_clock::now();
	auto limit = std::chrono::seconds(10);
	return ((now - start) > limit);
}

}
