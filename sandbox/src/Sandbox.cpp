#include <IPChanger.h>
#include <array>

namespace ip = ipchanger::system;

int main(int argc, char *argv[])
{
	auto file = ip::ReadFile<std::string>("Tibia/Tibia.exe");
	printf("Size of file is : %d\n", file.size());

	std::string seq{ 0x41, 0x20, 0x54, 0x69, 0x62, 0x69, 0x61, 0x20, 0x63, 0x6C, 0x69 };

	auto search = std::search(file.begin(), file.end(), std::boyer_moore_horspool_searcher(seq.begin(), seq.end()));
	while (search != file.end()) {

		auto it = search - 9;
		for (int i = 0; i < 8; i++) {
			printf("%x, ", *it++);
		}

		std::string bajs{ "Bajs" };
		std::copy(bajs.begin(), bajs.end(), search);
		search = std::search(search, file.end(), std::boyer_moore_horspool_searcher(seq.begin(), seq.end()));
	}

	ip::WriteBinary("Temp.exe", file.data(), file.size());

	printf("Done\n");
	std::cin.get();
	return 0;
}