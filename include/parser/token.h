#pragma once
#include "common.h"

namespace CantC {
namespace fs = std::filesystem;

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,

    // Delimiters.
    COMMA, DOT, SEMICOLON, COLON, BACKTICK, QUOTE, DOUBLE_QUOTE,

    // Operators.
    PLUS, MINUS, STAR, SLASH, MODULO, CARET, AMPERSAND,
    SPREAD, RANGE,

    // Comparison operators.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    AND, OR,

    // Assignment operators.
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, MODULO_EQUAL, CARET_EQUAL,
    INCREMENT, DECREMENT,

    // Literals.
    IDENTIFIER, STRING, NUMBER, BOOL,

    KEYWORD,

    END_OF_FILE
};

class TokenLoc {
public:
    fs::path filepath;

    size_t line;
    size_t column;

    TokenLoc(fs::path filepath = "UNKNOWN", size_t line = 1, size_t column = 1);
};

class Token {
private:


public:
    TokenType type;
    std::string value;

    TokenLoc location;

    Token(TokenType type, std::string value, TokenLoc location);
    Token();
};

};