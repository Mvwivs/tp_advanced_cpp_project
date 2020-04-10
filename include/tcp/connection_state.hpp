
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
	EventSet();
	EventSet(std::initializer_list<Event> list);
	~EventSet() = default;

	void add(Event event);

	bool isIn(Event event) const;

private:
	std::uint64_t event_set_;
};

}
