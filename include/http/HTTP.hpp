
#pragma once

#include <string>
#include <optional>
#include <vector>
#include <string_view>
#include <cstdint>

namespace http {

// Compare two strings case insensetive
bool iequals(const std::string& a, const std::string& b);

namespace HTTP {

// Exception used to indicate parsing errors
class ParsingException : public std::runtime_error {
public:
	ParsingException(const std::string& messsage);
};

// HTTP method
enum class Method {
	GET, POST
};

// convert string to `Method`
std::optional<Method> get_method(std::string method);

// HTTP Version
enum class Version {
	HTTP_1_0, HTTP_1_1
};

// convert string to `Version`
std::optional<Version> get_version(std::string version);

// convert `Version` to string
std::string map_version(Version version);

// HTTP start line (first line: method, target, version)
struct StartLine {
	Method method;
	std::string target;
	Version version;
};

// HTTP headers
struct Header {
	std::string name;
	std::string value;
};

// HTTP parameters
struct Parameter {
	std::string name;
	std::string value;
};

// HTTP Request
struct Request {
	StartLine startLine;								// request first line
	std::vector<Header> headers;						// header list
	std::optional<std::string> body;					// request body if present
	std::optional<std::vector<Parameter>> parameters;	// parameter vector if present

	Request() = default;
	Request(const std::string& data);
	~Request() = default;

	// try to get header from request struct
	const std::string& getHeader(const std::string& header_name) const;

private:
	// parse parameters from target (GET) or body (POST)
	void parseParameters(const std::string& data);

};

// Status code of HTTP request
using StatusCode = uint16_t;

// Get message for status code
std::string status_message(StatusCode code);

// status (first) line of response
struct StatusLine { // does not contain status text
	Version version;
	StatusCode code;
};

// HTTP response
struct Response {
	StatusLine statusLine;				// response first line
	std::vector<Header> headers;		// response header list
	std::optional<std::string> body;	// response body if present

	// build response message, content-length is set automatically
	std::string to_string() const;
};

// HTTP request "head" end sequence
constexpr std::string_view end_seq = "\r\n\r\n";

} // namespace HTTP

} // namespace http
