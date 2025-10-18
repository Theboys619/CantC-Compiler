#pragma once
#include "common.h"
#include <typeinfo>

#include "parser/token.h"

namespace CantC {
class Error : public std::exception {
private:
    std::string message;
    Token token;

    std::string source;

public:
    Error(const std::string& message, const Token& token, std::string source);

    virtual std::string getName() const;

    const char* what() const noexcept override;
};
}