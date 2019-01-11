#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
int main(int argc, char** argv)
{
    try {
        // Declare the supported options.
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help, h", "Instructions");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "You need to provide 3 arguments: ip, port and version" << std::endl
                      << "-----------------------------------------------------------------" << std::endl
                      << "Usage example: IPChanger 127.0.0.1 7171 860" << std::endl
                      << "Arg 1: IP-Address of the server (e.g 127.0.0.1)" << std::endl
                      << "Arg 2: Port (e.g 7171)" << std::endl
                      << "Arg 3: Version in the range 790-971 (eg. 860 = 8.60)" << std::endl;
            return 1;
        }

        if(argc < 4) {
            std::cout << desc << std::endl;
            return 0;
        }

    } catch (const po::error &ex) {
        std::cerr << ex.what() << '\n';
    }



    return 1;
}
