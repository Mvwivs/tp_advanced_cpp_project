
#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <optional>
#include <cstdint>
#include <cctype>
#include <stdexcept>

namespace http {

inline bool iequals(const std::string& a, const std::string& b) {
    return std::equal(a.begin(), a.end(), b.begin(), b.end(),
						[] (char a, char b) {
							return std::tolower(a) == std::tolower(b);
						});
}

namespace HTTP {

class ParsingException : public std::runtime_error {
public:
	ParsingException(const std::string& messsage):
		std::runtime_error(messsage) {
	}
	ParsingException() = delete;
};

enum class Method {
	GET, POST
};

inline std::optional<Method> get_method(std::string method) {
	if (method == "GET") {
		return Method::GET;
	}
	else if (method == "POST") {
		return Method::POST;
	}
	return {};
}

enum class Version {
	HTTP_1_0, HTTP_1_1
};

inline std::optional<Version> get_version(std::string version) {
	if (version == "HTTP/1.0") {
		return Version::HTTP_1_0;
	}
	else if (version == "HTTP/1.1") {
		return Version::HTTP_1_1;
	}
	return {};
}

inline std::string map_version(Version version) {
	switch (version) {
		case Version::HTTP_1_0:
			return "HTTP/1.0";
		case Version::HTTP_1_1:
			return "HTTP/1.1";
		default:
			return "Unsupported version";
	}
}

struct StartLine {
	Method method;
	std::string target;
	Version version;
};

struct Header {
	std::string name;
	std::string value;
};

struct Request {
	StartLine startLine;
	std::vector<Header> headers;
	std::optional<std::string> body;

	Request() = default;

	Request(const std::string& data) {
		std::istringstream ss(data);
		std::string line;

		std::getline(ss, line); // start line
		std::size_t method_pos = line.find(' ');
		if (method_pos == std::string::npos) {
			throw ParsingException("Method not found");
		}
		auto method = get_method(line.substr(0, method_pos));
		if (!method) {
			throw ParsingException("Wrong method");
		}
		startLine.method = *method;

		std::size_t target_pos = line.find(' ', method_pos + 1);
		if (target_pos == std::string::npos) {
			throw ParsingException("Method not found");
		}
		startLine.target = line.substr(method_pos + 1, target_pos - (method_pos + 1));

		auto version = get_version(line.substr(target_pos + 1, line.size() - (target_pos + 1) - 1));
		if (!version) {
			throw ParsingException("Wrong version");
		}
		startLine.version = *version;

		while (std::getline(ss, line) && line != "\r") {
			std::size_t sep_pos = line.find(':');
			if (sep_pos == std::string::npos) {
				throw ParsingException("Header separator not found");
			}
			headers.push_back(
				{line.substr(0, sep_pos), line.substr(sep_pos + 2, line.size() - (sep_pos + 2) - 1)});
		}
		
		if (ss) {
			body = std::string(ss.str().substr(ss.tellg()));
		}
	}

	const std::string& getHeader(const std::string& header_name) const {
		for (const auto& [name, value] : headers) {
			if (iequals(name, header_name)) {
				return value;
			}
		}
		throw ParsingException("Header not found");
	}
};

using StatusCode = uint16_t;

inline std::string status_message(StatusCode code) {
	switch (code) {
		case 200:
			return "OK";
		case 302:
			return "Found";
		case 400:
			return "Bad Request";
		case 404:
			return "Not Found";
		default:
			return "Unsupported error code";
	}
}

struct StatusLine { // does not contain status text
	Version version;
	StatusCode code;
};

struct Response {
	StatusLine statusLine;
	std::vector<Header> headers;
	std::optional<std::string> body;

	std::string to_string() const {
		std::ostringstream r;
		r << map_version(statusLine.version) << ' '
			<< std::to_string(statusLine.code) << ' '
			<< status_message(statusLine.code) << "\r\n";
		for (const auto& [name, value] : headers) {
			r << name << ": " << value << "\r\n";
		}

		if (body) {
			r << "Content-Length: " << body->size() << "\r\n";
		}
		r << "\r\n";
		if (body) {
			r << *body;
		}
		return r.str();
	}
};

}

}
