#define NOMINMAX // Else it won't compile on Windows
#include "pch.h"
#include "ipchanger/system/System.h"
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

	void WriteBinary(const fs::path& path, const char* buff, std::size_t length, unsigned long options)
	{
		#ifdef _WIN32
		HANDLE handl = CreateFile(path.c_str(), GENERIC_WRITE, 0, nullptr,
			CREATE_ALWAYS, options, nullptr);
		WriteFile(handl, buff, length, nullptr, nullptr);
		CloseHandle(handl);
		#else
		std::ofstream output{ path, std::ios::binary };
		output.write(buff, length);
		fs::permissions(path, fs::perms::owner_all); // Allow all permissions to be able to execute it
		#endif
	}

	void ExecuteBinary(const fs::path& path)
	{
		if (!fs::exists(path))
			return;

		fs::current_path(path.parent_path());
		#ifdef _WIN32
		WinExec(path.string().c_str(), SW_HIDE);
		#else
		std::system(path.string().c_str());
		#endif
	}
}