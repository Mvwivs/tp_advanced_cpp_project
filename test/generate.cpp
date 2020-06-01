
#include <random>
#include <fstream>

#include "file/DbServer.hpp"

int main() {

	constexpr std::size_t elements = 17'000'000;

	std::mt19937 rng(1);
	std::uniform_int_distribution<std::size_t> skip_gen(0, 100);
	std::uniform_int_distribution<std::size_t> skip_len_gen(0, 3);
	std::uniform_int_distribution<char> symbol_gen('a', 'z');

	auto skip = [&skip_gen, &rng] {
		return skip_gen(rng) < 50;
	};
	auto string_gen = [&symbol_gen, &rng] (std::size_t len) {
		std::string res(len, '-');
		for (char& c : res) {
			c = symbol_gen(rng);
		}
		return res;
	};

	std::ofstream out("test.db");

	std::uint64_t key = 0;
	std::size_t element_count = 0;
	while (element_count < elements) {
		if (skip()) {
			key += skip_len_gen(rng);
		}
		else {
			std::pair<std::uint64_t, file::Data> data;
			data.first = key;
			std::string payload = std::to_string(key);
			payload += string_gen(sizeof(data.second.payload) - payload.size());
			for (std::size_t i = 0; i < payload.size(); ++i) {
				data.second.payload[i] = static_cast<std::uint8_t>(payload[i]);
			}

			out.write(reinterpret_cast<char*>(&data), sizeof(data));

			++element_count;
			++key;
		}
	}

	std::ofstream urls("url.txt");
	for (std::uint64_t i = 0; i < key; i += 10) {
		urls << "http://127.0.0.1:8888/?key=" << i << std::endl;
	}

	return 0;
}
