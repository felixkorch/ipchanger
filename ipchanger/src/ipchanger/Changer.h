#pragma once
#ifndef CHANGER_H
#define CHANGER_H
#include "pch.h"
#include "ipchanger/system/System.h"

namespace ipchanger {

	class Changer {
	private:
		std::string buffer;
	private:
		bool SearchAndReplace(std::string& in, const std::string_view& pattern, const std::string_view& replace);
		void SearchAndReplace(std::string& in, unsigned int pattern, unsigned int replace);

	public:
		Changer(const std::string& ip, unsigned int port, const std::filesystem::path& in);

		const std::string& Data() const
		{
			return buffer;
		}

	};

} // namespace

#endif // CHANGER_H
