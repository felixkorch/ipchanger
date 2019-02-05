#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H

#include <filesystem>

namespace ipchanger::system {

	constexpr unsigned long WINDOWS_HIDDEN_FILE = 0x2;
	constexpr unsigned long WINDOWS_NORMAL_FILE = 0x80;

	template <class T>
	inline T ReadFile(const std::filesystem::path& path)
	{
		std::ifstream in{ path, std::ios::binary };
		return T{ std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
	}

	std::filesystem::path TempName(const std::string_view& temp, const std::filesystem::path& path = std::filesystem::path());
	void WriteBinary(const std::filesystem::path& path, const char* buff, std::size_t length, unsigned long options = WINDOWS_NORMAL_FILE);
	void ExecuteBinary(const std::filesystem::path& path);

} // namespace

#endif // SYSTEM_H
