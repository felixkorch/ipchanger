#include "pch.h"
#include "ipchanger/system/System.h"

namespace fs = std::filesystem;

namespace ipchanger::system {

	void WriteBinary(const fs::path& path, const char* buff, std::size_t length, unsigned long options)
	{
		std::ofstream output{ path, std::ios::binary };
		output.write(buff, length);
		fs::permissions(path, fs::perms::owner_all); // Allow all permissions to be able to execute it
	}

	void ExecuteBinary(const fs::path& path)
	{
		if (!fs::exists(path))
			return;

		fs::current_path(path.parent_path());
		std::system(path.string().c_str());
	}

}