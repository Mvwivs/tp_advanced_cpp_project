
#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <optional>
#include <cstdint>
#include <cctype>
#include <stdexcept>

namespace http {

bool iequals(const std::string& a, const std::string& b);

namespace HTTP {

class ParsingException : public std::runtime_error {
public:
	ParsingException(const std::string& messsage);
};

enum class Method {
	GET, POST
};

std::optional<Method> get_method(std::string method);

enum class Version {
	HTTP_1_0, HTTP_1_1
};

std::optional<Version> get_version(std::string version);

std::string map_version(Version version);

struct StartLine {
	Method method;
	std::string target;
	Version version;
};

struct Header {
	std::string name;
	std::string value;
};

struct Parameter {
	std::string name;
	std::string value;
};

struct Request {
	StartLine startLine;
	std::vector<Header> headers;
	std::optional<std::string> body;
	std::optional<std::vector<Parameter>> parameters;

	Request() = default;
	Request(const std::string& data);
	~Request() = default;

	const std::string& getHeader(const std::string& header_name) const;

private:
	void parseParameters(const std::string& data);

};

using StatusCode = uint16_t;

std::string status_message(StatusCode code);

struct StatusLine { // does not contain status text
	Version version;
	StatusCode code;
};

struct Response {
	StatusLine statusLine;
	std::vector<Header> headers;
	std::optional<std::string> body;

	std::string to_string() const;
};

} // namespace HTTP

} // namespace http
