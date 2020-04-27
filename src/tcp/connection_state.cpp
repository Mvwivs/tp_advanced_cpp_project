
#include "tcp/connection_state.hpp"

namespace tcp {

EventSet::EventSet():
	event_set_(0) {
}

EventSet::EventSet(std::initializer_list<Event> list):
	event_set_(0) {

	for (Event e : list) {
		add(e);
	}
}

void EventSet::add(Event event) {
	event_set_ = event_set_ | static_cast<std::uint64_t>(event);
}

bool EventSet::isIn(Event event) const {
	return event_set_ & static_cast<std::uint64_t>(event);
}

}
