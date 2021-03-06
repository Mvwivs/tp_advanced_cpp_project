
#include "http/HTTP.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <sstream>

namespace http {

bool iequals(const std::string& a, const std::string& b) {
    return std::equal(a.begin(), a.end(), b.begin(), b.end(),
						[] (char a, char b) { // ignore case
							return std::tolower(a) == std::tolower(b);
						});
}

namespace HTTP {

ParsingException::ParsingException(const std::string& messsage):
	std::runtime_error(messsage) {
}

std::optional<Method> get_method(std::string method) {
	if (method == "GET") {
		return Method::GET;
	}
	else if (method == "POST") {
		return Method::POST;
	}
	return {};
}

std::optional<Version> get_version(std::string version) {
	if (version == "HTTP/1.0") {
		return Version::HTTP_1_0;
	}
	else if (version == "HTTP/1.1") {
		return Version::HTTP_1_1;
	}
	return {};
}

std::string map_version(Version version) {
	switch (version) {
		case Version::HTTP_1_0:
			return "HTTP/1.0";
		case Version::HTTP_1_1:
			return "HTTP/1.1";
		default:
			return "Unsupported version";
	}
}

Request::Request(const std::string& data) {
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

	std::size_t target_pos = line.find(' ', method_pos + 1); // target page
	if (target_pos == std::string::npos) {
		throw ParsingException("Method not found");
	}
	startLine.target = line.substr(method_pos + 1, target_pos - (method_pos + 1));

	auto version = get_version(line.substr(target_pos + 1, line.size() - (target_pos + 1) - 1)); // version
	if (!version) {
		throw ParsingException("Wrong version");
	}
	startLine.version = *version;

	while (std::getline(ss, line) && line != "\r") { // parse headers
		std::size_t sep_pos = line.find(':');
		if (sep_pos == std::string::npos) {
			throw ParsingException("Header separator not found");
		}
		headers.push_back(
			{line.substr(0, sep_pos), line.substr(sep_pos + 2, line.size() - (sep_pos + 2) - 1)});
	}
	
	if (ss) { // everything left is body
		body = std::string(ss.str().substr(ss.tellg()));
	}

	if (method == Method::GET) {	// query params in GET
		std::size_t param_pos = startLine.target.find('?');
		if (param_pos != std::string::npos) {
			parseParameters(startLine.target.substr(param_pos + 1));
			startLine.target = startLine.target.substr(0, param_pos);
		}
	}
	else if (method == Method::POST) {	// query params for POST
		try {
			const std::string& type = getHeader("Content-Type");
			if (type == "application/x-www-form-urlencoded" && body) {
				parseParameters(*body);
			}
		}
		catch (const ParsingException& e) {
		}
	}
}

const std::string& Request::getHeader(const std::string& header_name) const {
	for (const auto& [name, value] : headers) {
		if (iequals(name, header_name)) {
			return value;
		}
	}
	throw ParsingException("Header not found");
}

void Request::parseParameters(const std::string& data) {
	std::vector<Parameter> params;

	std::size_t prev_param_end = 0;
	for (std::size_t equal_char = 0; 
		(equal_char = data.find('=', prev_param_end)) != std::string::npos;) {

		std::size_t and_char = data.find('&', equal_char + 1);
		params.push_back({data.substr(prev_param_end, equal_char - prev_param_end),
			data.substr(equal_char + 1, and_char - (equal_char + 1))});
		if (and_char == std::string::npos) {
			break;
		}
		prev_param_end = and_char + 1;
	}
	if (!params.empty()) {
		parameters = params;
	}
}

std::string status_message(StatusCode code) {
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

std::string Response::to_string() const {
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

}

}
