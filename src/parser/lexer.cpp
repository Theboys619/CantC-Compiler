#include "parser/lexer.h"
#include "parser/globals.h"

namespace CantC {
    Lexer::Lexer(std::string source) : Lexer() {
        this->source = source;
        filepath = "STRING_INPUT";
    }

    Lexer::Lexer() {
        source = "";
        filepath = "UNKNOWN";

        currentIndex = 0;

        line = 1;
        column = 1;
    }

    void Lexer::loadFromFile(fs::path filepath) {
        this->filepath = filepath;
        std::ifstream fileStream(filepath);
        if (!fileStream.is_open()) {
            throw std::runtime_error("Could not open file: " + filepath.string());
        }
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        source = buffer.str();
        fileStream.close();
        currentIndex = 0;
        line = 1;
        column = 1;
    }

    void Lexer::loadFromString(std::string source) {
        this->source = source;
        filepath = "STRING_INPUT";
        currentIndex = 0;
        line = 1;
        column = 1;
    }

    void Lexer::skipWhitespace() {
        while (currentIndex < source.size() && std::isspace(source[currentIndex])) {
            if (source[currentIndex] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            currentIndex++;
        }
    }

    char Lexer::advance(int amt) {
        char currentChar = '\0';
        for (int i = 0; i < amt; i++) {
            if (isAtEnd()) {
                return '\0';
            }
            
            currentIndex++;
            column++;
            currentChar = source[currentIndex];
        }
        return currentChar;
    }

    Token Lexer::nextToken(int amt) {
        skipWhitespace();


        if (isAtEnd()) {
            return Token(TokenType::END_OF_FILE, "", TokenLoc(filepath, line, column));
        }

        Token token = currentToken();

        for (int i = 0; i < amt; i++) {

            skipWhitespace();

            if (isAtEnd()) {
                return Token(TokenType::END_OF_FILE, "", TokenLoc(filepath, line, column));
            }

            token = getToken();
        }

        return token;
    }

    Token Lexer::peekToken(int amt) {
        int tempIndex = currentIndex;
        size_t tempLine = line;
        size_t tempColumn = column;

        Token token = nextToken(amt);

        currentIndex = tempIndex;
        line = tempLine;
        column = tempColumn;
        return token;
    }

    bool Lexer::isAtEnd() const {
        return currentIndex >= source.size();
    }

    Token Lexer::currentToken() const {
        return current;
    }

    Token Lexer::getToken() {
        if (isAtEnd()) {
            return Token(TokenType::END_OF_FILE, "", TokenLoc(filepath, line, column));
        }

        skipWhitespace();
        
        char currentChar = source[currentIndex];
        TokenLoc location(filepath, line, column);

        // Single and multi-character tokens
        switch (currentChar) {
            case '(': {
                current = Token(TokenType::LEFT_PAREN, "(", location);
                advance();
                return current;
            }
            case ')': {
                current = Token(TokenType::RIGHT_PAREN, ")", location);
                advance();
                return current;
            }
            case '{': {
                current = Token(TokenType::LEFT_BRACE, "{", location);
                advance();
                return current;
            }
            case '}': {
                current = Token(TokenType::RIGHT_BRACE, "}", location);
                advance();
                return current;
            }
            case ',': {
                current = Token(TokenType::COMMA, ",", location);
                advance();
                return current;
            }
            case '.': {
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '.') {
                    advance(); // skip first '.'.
                    if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '.') {
                        advance(2); // skip next two .'..'
                        return current = Token(TokenType::SPREAD, "...", location);
                    }
                    advance(); // skip second .'.'
                    return current = Token(TokenType::RANGE, "..", location);
                }
                current = Token(TokenType::DOT, ".", location);
                advance();
                return current;
            }
            case ';': {
                current = Token(TokenType::SEMICOLON, ";", location);
                advance();
                return current;
            }
            case ':': {
                current = Token(TokenType::COLON, ":", location);
                advance();
                return current;
            }
            case '`': {
                current = Token(TokenType::BACKTICK, "`", location);
                advance();
                return current;
            }
            case '+': {
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '+') {
                    advance(2);
                    return current = Token(TokenType::INCREMENT, "++", location);
                } else if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '=') {
                    advance(2);
                    return current = Token(TokenType::PLUS_EQUAL, "+=", location);
                }
                current = Token(TokenType::PLUS, "+", location);
                advance();
                return current;
            }
            case '-': {
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '-') {
                    advance(2);
                    return current = Token(TokenType::DECREMENT, "--", location);
                } else if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '=') {
                    advance(2);
                    return current = Token(TokenType::MINUS_EQUAL, "-=", location);
                }
                current = Token(TokenType::MINUS, "-", location);
                advance();
                return current;
            }
            case '*': {
                current = Token(TokenType::STAR, "*", location);
                advance();
                return current;
            }
            case '/': {
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '/') {
                    // Single-line comment
                    while (currentIndex < source.size() && source[currentIndex] != '\n') {
                        advance();
                    }
                    return getToken();
                } else if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '*') {
                    // Multi-line comment
                    advance(2); // Skip '/*'
                    while (currentIndex + 1 < source.size() && !(source[currentIndex] == '*' && source[currentIndex + 1] == '/')) {
                        if (source[currentIndex] == '\n') {
                            line++;
                            column = 1;
                        } else {
                            column++;
                        }
                        currentIndex++;
                    }
                    if (currentIndex + 1 >= source.size()) {
                        throw SyntaxError("Unterminated multi-line comment", Token(TokenType::IDENTIFIER, "/*", location), std::string(source));
                    }
                    advance(2); // Skip '*/'
                    return getToken();
                }
                current = Token(TokenType::SLASH, "/", location);
                advance();
                return current;
            }
            case '%': {
                current = Token(TokenType::MODULO, "%", location);
                advance();
                return current;
            }
            case '^': {
                current = Token(TokenType::CARET, "^", location);
                advance();
                return current;
            }
            case '!':
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '=') {
                    advance(2);
                    return current = Token(TokenType::BANG_EQUAL, "!=", location);
                } else {
                    advance();
                    return current = Token(TokenType::BANG, "!", location);
                }
            case '&':
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '&') {
                    advance(2);
                    return current = Token(TokenType::AND, "&&", location);
                } else {
                    advance();
                    return current = Token(TokenType::AMPERSAND, "&", location);
                }
            case '|':
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '|') {
                    advance(2);
                    return current = Token(TokenType::OR, "||", location);
                } else {
                    throw SyntaxError("Unexpected character '|'", Token(TokenType::IDENTIFIER, "|", location), std::string(source));
                }
            case '=':
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '=') {
                    advance(2);
                    return current = Token(TokenType::EQUAL_EQUAL, "==", location);
                } else {
                    advance();
                    return current = Token(TokenType::EQUAL, "=", location);
                }
            case '<':
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '=') {
                    advance(2);
                    return current = Token(TokenType::LESS_EQUAL, "<=", location);
                } else {
                    advance();
                    return current = Token(TokenType::LESS, "<", location);
                }
            case '>':
                if (currentIndex + 1 < source.size() && source[currentIndex + 1] == '=') {
                    advance(2);
                    return current = Token(TokenType::GREATER_EQUAL, ">=", location);
                } else {
                    advance();
                    return current = Token(TokenType::GREATER, ">", location);
                }
            case '[': {
                current = Token(TokenType::LEFT_BRACKET, "[", location);
                advance();
                return current;
            }
            case ']': {
                current = Token(TokenType::RIGHT_BRACKET, "]", location);
                advance();
                return current;
            }
            default:
                break;
        }
        
        // Number literals
        if (std::isdigit(currentChar)) {
            size_t start = currentIndex;
            while (currentIndex < source.size() && (std::isdigit(source[currentIndex]) || source[currentIndex] == '.')) {
                advance();
            }
            std::string numberStr = source.substr(start, currentIndex - start);
            return current = Token(TokenType::NUMBER, numberStr, location);
        }
        // Identifiers and keywords
        if (std::isalpha(currentChar) || currentChar == '_') {
            size_t start = currentIndex;
            while (currentIndex < source.size() && (std::isalnum(source[currentIndex]) || source[currentIndex] == '_')) {
                advance();
            }
            std::string identifier = source.substr(start, currentIndex - start);
            if (std::find(CantC::keywords.begin(), CantC::keywords.end(), identifier) != CantC::keywords.end()) {
                if (identifier == "true" || identifier == "false") {
                    return current = Token(TokenType::BOOL, identifier, location);
                }
                return current = Token(TokenType::KEYWORD, identifier, location);
            } else {
                return current = Token(TokenType::IDENTIFIER, identifier, location);
            }
        }
        // String literals
        if (currentChar == '"' || currentChar == '\'') {
            char quoteType = currentChar;
            advance(); // Skip the opening quote
            size_t start = currentIndex;
            while (currentIndex < source.size() && source[currentIndex] != quoteType) {
                if (source[currentIndex] == '\\' && currentIndex + 1 < source.size()) {
                    advance(2); // Skip escaped character
                } else {
                    advance();
                }
            }
            if (isAtEnd()) {
                throw SyntaxError("Unterminated string literal", Token(TokenType::STRING, source.substr(start, currentIndex - start), location), std::string(source));
            }
            std::string stringValue = source.substr(start, currentIndex - start);
            advance(); // Skip the closing quote
            return current = Token(TokenType::STRING, stringValue, location);
        }
        // If we reach here, it's an unknown character
        throw SyntaxError(std::string("Unexpected character: ") + currentChar, Token(TokenType::IDENTIFIER, std::string(1, currentChar), location), std::string(source));
    }
}