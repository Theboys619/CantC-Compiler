#include "cantc.h"

int main(int argc, char** argv) {

    if (argc < 2) {
        CantC::printUsage(argv[0]);
        return 1;
    }

    // std::filesystem::path file = argv[1];
    // if (!std::filesystem::exists(file)) {
    //     std::cerr << "File not found: " << file << std::endl;
    //     return 1;
    // }
    
    CantC::Options options = CantC::parseCommandLine(argc, argv);

    try {
        CantC::run(options);

        // std::cout << "Parsed " << ast->statements.size() << " top-level statements." << std::endl;
    } catch (const CantC::Error& e) {
        // std::cerr << e.what() << std::endl;
        e.what();
    }

    return 0;
}