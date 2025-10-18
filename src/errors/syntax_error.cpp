#include "errors/syntax_error.h"

namespace CantC {
    SyntaxError::SyntaxError(const std::string& message, const Token& token, std::string source) : Error(message, token, source) {}

    std::string SyntaxError::getName() const {
        return "Syntax Error";
    }
}