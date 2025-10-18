#include "cantc.h"

namespace CantC {
    void printAST(BlockStmt* root) {
        TreePrinter::print(root);
    }

    void run(Options options) {
        if (options.inputFiles.empty()) {
            throw std::runtime_error("No input files provided.");
        }

        Parser parser;
        for (const auto& file : options.inputFiles) {
            parser.loadFromFile(file);
        }
        BlockStmt* ast = parser.parse();
        if (options.debug) {
            printAST(ast);
        }

        Compiler compiler(options);
        compiler.compile(ast);

        // Compiler compiler(options);
        // compiler.compile();
    }
}