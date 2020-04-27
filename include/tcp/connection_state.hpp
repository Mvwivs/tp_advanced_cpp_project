
#pragma once

#include <cstdint>
#include <initializer_list>

namespace tcp {

// Bit shift helper
constexpr std::uint64_t shift(std::uint8_t offset) {
	return (1 << offset);
}

// Possible events for descriptor
enum class Event : std::uint64_t {
	WriteRdy = shift(0),
	ReadRdy = shift(1),
	Closed = shift(2)
};

// Set containing events
class EventSet {
public:
	EventSet();
	EventSet(std::initializer_list<Event> list);
	~EventSet() = default;

	// Add event to set
	void add(Event event);

	// Check if event in set
	bool isIn(Event event) const;

private:
	std::uint64_t event_set_;
};

}
