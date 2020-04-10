
#pragma once

#include <cstdint>
#include <initializer_list>

namespace tcp {

constexpr std::uint64_t shift(std::uint8_t offset) {
	return (1 << offset);
}

enum class Event : std::uint64_t {
	WriteRdy = shift(0),
	ReadRdy = shift(1), 
	Closed = shift(2)
};

class EventSet {
public:
	EventSet():
		event_set_(0) {
	}
	EventSet(std::initializer_list<Event> list):
		event_set_(0) {

		for (Event e : list) {
			add(e);
		}
	}
	~EventSet() = default;

	void add(Event event) {
		event_set_ = event_set_ | static_cast<std::uint64_t>(event);
	}

	bool isIn(Event event) const {
		return event_set_ & static_cast<std::uint64_t>(event);
	}

private:
	std::uint64_t event_set_;
};

}
