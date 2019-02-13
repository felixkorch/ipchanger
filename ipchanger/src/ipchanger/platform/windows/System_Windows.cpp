#include "ippch.h"
#include "ipchanger/System.h"

namespace fs = std::filesystem;

namespace ipchanger::system {

	void WriteBinary(const fs::path& path, const char* buff, std::size_t length, unsigned long options)
	{
		const char* p = path.string().c_str();

		HANDLE handl = CreateFile(p, GENERIC_WRITE, 0, nullptr,
			CREATE_ALWAYS, options, nullptr);
		WriteFile(handl, buff, length, nullptr, nullptr);
		CloseHandle(handl);
	}

	void ExecuteBinary(const fs::path& path)
	{
		if (!fs::exists(path))
			return;

		fs::current_path(path.parent_path());
		WinExec(path.string().c_str(), SW_HIDE);
	}

}