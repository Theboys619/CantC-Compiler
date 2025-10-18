#pragma once
#include "common.h"

#include "parser/token.h"
#include "errors/syntax_error.h"

namespace CantC {
    namespace fs = std::filesystem;

    class Lexer {
    public:
        fs::path filepath;
        size_t line;
        size_t column;
        
        Lexer(std::string source);
        Lexer();

        void loadFromFile(fs::path filepath);
        void loadFromString(std::string source);

        void skipWhitespace();

        char advance(int amt = 1);

        Token nextToken(int amt = 1);
        Token peekToken(int amt = 1);
        bool isAtEnd() const;

        Token getToken();
        Token currentToken() const;
    private:
        Token current;

        std::string source;
        size_t currentIndex;

        friend class Parser;
        friend class TypeChecker;
    };
}