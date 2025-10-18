#include "errors/type_error.h"

namespace CantC {
    TypeError::TypeError(const std::string& message, const Token& token, std::string source) : Error(message, token, source) {}

    std::string TypeError::getName() const {
        return "Type Error";
    }
}