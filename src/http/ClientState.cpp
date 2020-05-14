
#include "http/ClientState.hpp"

#include <cstring>

#include <unistd.h>

using namespace std::string_literals;

namespace http {

ClientState::ClientState(Coroutine::routine_t new_id) :
	id(new_id),
	keep_alive(false),
	process_state(ReadingRequest),
	start(std::chrono::system_clock::now()),
	timed_out_{false} {
}

bool ClientState::timed_out() const {
	if (timed_out_) {
		return true;
	}
	auto now = std::chrono::system_clock::now();
	auto limit = std::chrono::seconds(10);
	return ((now - start) > limit);
}


std::optional<std::pair<std::vector<char>, std::size_t>>
ClientState::readHead(int fd) {
	std::vector<char> full_data;
	std::vector<char>::iterator head_end;
	while (true) {
		if (timed_out()) {
			return {};
		}
		std::array<char, 400> buffer;
		ssize_t recieved = ::read(fd, buffer.data(), buffer.size());
		if (recieved == 0 || (recieved == -1 && errno == ECONNRESET)) { // connection closed
			keep_alive = false;
			return {};
		}
		else if (recieved == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) { // wait for more data
				Coroutine::yield();
				continue;
			}
			throw std::runtime_error("Unable to read: "s + std::strerror(errno));
		}

		full_data.insert(full_data.end(), buffer.begin(), buffer.begin() + recieved);

		// check for http headers end
		head_end = std::search(full_data.end() - recieved - 3, full_data.end(),
			http::HTTP::end_seq.begin(), http::HTTP::end_seq.end());
		if (head_end != full_data.end()) { // found \r\n\r\n in header
			break;
		}
	}
	return { {full_data, head_end - full_data.begin() + 2} };
}

std::optional<std::string> 
ClientState::readBody(int fd, std::size_t len, std::vector<char> body) {

	std::size_t to_read = len - body.size();
	while (to_read != 0) {
		if (timed_out()) {
			return {};
		}
		std::array<char, 400> buffer;
		ssize_t recieved = ::read(fd, buffer.data(), buffer.size());
		if (recieved == 0 || (recieved == -1 && errno == ECONNRESET)) { // connection closed
			keep_alive = false;
			return {};
		}
		else if (recieved == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) { // wait for more data
				Coroutine::yield();
				continue;
			}
			throw std::runtime_error("Unable to read: "s + std::strerror(errno));
		}
		body.insert(body.end(), buffer.begin(), buffer.begin() + recieved);
		to_read -= recieved;
	}
	return std::string(body.begin(), body.end());

}

std::optional<http::HTTP::Request> ClientState::readRequest(int fd) {
	auto head = readHead(fd);
	if (!head) {
		return {};
	}
	auto& [full_data, head_end] = *head;

	http::HTTP::Request request = http::HTTP::Request(
		std::string(full_data.begin(), full_data.begin() + head_end));
	try { // check connection type
		const std::string& type = request.getHeader("Connection");
		if (type == "Keep-Alive") {
			keep_alive = true;
		}
	}
	catch (const http::HTTP::ParsingException&) { // no keep-alive
	}
	try { // try to read body
		const std::string& len = request.getHeader("Content-Length");
		std::size_t body_len = std::stoull(len);

		std::vector<char> body_data(full_data.begin() + head_end + 2, full_data.end());
		auto body = readBody(fd, body_len, body_data);
		if (body) {
			request.body = *body;
		}
	}
	catch (const http::HTTP::ParsingException&) { // no body, finish reading
	}

	return request;
}

bool ClientState::sendResponse(int fd, const http::HTTP::Response& response) {
	std::string resp_str = response.to_string();
	size_t written = 0;
	while (resp_str.size() - written != 0) {
		if (timed_out()) {
			return false;
		}
		ssize_t res = ::write(fd, resp_str.data() + written, resp_str.size() - written);
		if (res == 0 || (res == -1 && errno == ECONNRESET)) { // connection closed
			throw std::runtime_error("Unable to write, EOF: "s + std::strerror(errno));
		}
		else if (res == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				Coroutine::yield();
				continue;
			}
			throw std::runtime_error("Unable to write: "s + std::strerror(errno));
		}
		written += res;
	}
	return true;
}

}
