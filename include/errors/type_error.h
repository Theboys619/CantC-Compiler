#pragma once
#include "common.h"

#include "parser/token.h"
#include "ws_error.h"

namespace CantC {
class TypeError : public Error {
private:

public:
    TypeError(const std::string& message, const Token& token, std::string source);

    virtual std::string getName() const;
};
}