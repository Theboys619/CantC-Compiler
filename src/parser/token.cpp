#include "parser/token.h"

namespace CantC {
    TokenLoc::TokenLoc(fs::path filepath, size_t line, size_t column) : filepath(filepath), line(line), column(column) {}

    Token::Token(TokenType type, std::string value, TokenLoc location) : type(type), value(value), location(location) {}

    Token::Token() : type(TokenType::END_OF_FILE), value(""), location(TokenLoc()) {}
}