#pragma once

#include "common.h"

namespace CantC {
    struct Options {
        bool verbose = false;
        bool optimize = false;
        bool debug = false;
        bool emitIR = false;
        bool emitASM = false;
        bool emitOBJ = false;
        std::string outputFile = "a.out";
        std::vector<std::string> inputFiles;

        Options() = default;
    };

    Options parseCommandLine(int argc, char** argv);
    void printUsage(const std::string& programName);
}