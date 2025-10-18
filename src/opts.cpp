#include "opts.h"

namespace CantC {
    Options parseCommandLine(int argc, char** argv) {
        Options opts;
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-v" || arg == "--verbose") {
                opts.verbose = true;
            } else if (arg == "-O" || arg == "--optimize") {
                opts.optimize = true;
            } else if (arg == "-g" || arg == "--debug") {
                opts.debug = true;
            } else if (arg == "--emit-ir") {
                opts.emitIR = true;
            } else if (arg == "--emit-asm") {
                opts.emitASM = true;
            } else if (arg == "--emit-obj") {
                opts.emitOBJ = true;
            } else if (arg == "-o" || arg == "--output") {
                if (i + 1 < argc) {
                    opts.outputFile = argv[++i];
                } else {
                    throw std::runtime_error("Expected output file after " + arg);
                }
            } else {
                opts.inputFiles.push_back(arg);
            }
        }
        return opts;
    }

    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options] <input files>\n"
                  << "Options:\n"
                  << "  -v, --verbose         Enable verbose output\n"
                  << "  -O, --optimize        Enable optimizations\n"
                  << "  -g, --debug           Include debug information\n"
                  << "      --emit-ir         Emit intermediate representation\n"
                  << "      --emit-asm        Emit assembly code\n"
                  << "      --emit-obj        Emit object code\n"
                  << "  -o, --output <file>   Specify output file (default: a.out)\n";
    }
}