#include "errors/compile_error.h"

namespace CantC {
    CompileError::CompileError(const std::string& message, const Token& token, std::string source) : Error(message, token, source) {}

    std::string CompileError::getName() const {
        return "Compile-Time Error";
    }
}