﻿#pragma once
#ifndef CHANGER_H
#define CHANGER_H

#include "ippch.h"
#include "System.h"

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

}

#endif // CHANGER_H
