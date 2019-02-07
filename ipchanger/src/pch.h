#pragma once
#include <string>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

template <typename... T>
constexpr auto make_array(T&&... values) ->
std::array<
	typename std::decay<
	typename std::common_type<T...>::type>::type,
	sizeof...(T)> {
	return std::array<
		typename std::decay<
		typename std::common_type<T...>::type>::type,
		sizeof...(T)>{std::forward<T>(values)...};
}

enum class OS { Linux, Mac, Windows };

#ifdef __linux__
constexpr OS current_os = OS::Linux;
#elif __APPLE__
constexpr OS current_os = OS::Mac;
#elif _WIN32
constexpr OS current_os = OS::Windows;
#endif

#ifdef _WIN32
#define NOMINMAX // Required to avoid conflicts on Windows
#include <windows.h>
#include "winsock.h"
#include <fileapi.h>
#include <tlhelp32.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif
