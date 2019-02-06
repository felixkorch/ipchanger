#include "pch.h"
#include "System.h"
#include <random>
#include <limits>

namespace fs = std::filesystem;

namespace ipchanger::system {

	fs::path TempName(const std::string_view& temp, const fs::path& path)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

		auto generate = [](int r) {
			std::stringstream stream;
			stream << std::hex << r;
			std::string result(stream.str());
			return result;
		};

		fs::path new_path;
		do {
			std::string out{ temp };
			std::string hex = generate(dis(gen));
			auto it = hex.begin();
			for (char& c : out) {
				if (it == hex.end()) {
					hex = generate(dis(gen));
					it = hex.begin();
				}
				if (c == '%')
					c = *it++;
			}
			new_path = path / out;
		} while (fs::exists(new_path));


		return new_path;
	}
}