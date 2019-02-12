#include <IPChanger.h>
#include <array>

namespace ip = ipchanger::system;

int main(int argc, char *argv[])
{
	auto file = ip::ReadFile<std::string>("Tibia/Tibia.exe");
	printf("Size of file is : %d\n", file.size());

	char c = 0xB8;

	std::string seq{ c, 0x0F, 0x10, 0x01, 0x00 };

	auto search = std::search(file.begin(), file.end(), std::boyer_moore_searcher(seq.begin(), seq.end()));
	while (search != file.end()) {

		printf("MATCH!\n");

		for (int i = 0; i < seq.size(); i++) {
			search[i] = 0x90; // NOP
		}

		search = std::search(search, file.end(), std::boyer_moore_searcher(seq.begin(), seq.end()));
	}

	ip::WriteBinary("Temp.exe", file.data(), file.size());

	printf("Done\n");
	std::cin.get();
	return 0;
}