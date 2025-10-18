#include "errors/ws_error.h"

namespace CantC {
    Error::Error(const std::string& message, const Token& token, std::string source) : message(message), token(token), source(source) {}

    std::string Error::getName() const {
        return "Error";
    }

    const char* Error::what() const noexcept {
        std::string error = "";

        if (source.size() > 0) {
            std::istringstream stream(source);
            std::string line;
            std::string prevLine = "";
            std::string nextLine = "";
            size_t currentLine = 1;
            while (std::getline(stream, line)) {
                if (currentLine == token.location.line) {
                    error += prevLine + "\n";
                    error += line + "\n";
                    
                    for (size_t i = 1; i < token.location.column; i++) {
                        if (i - 1 < line.size() && line[i - 1] == '\t') {
                            error += "\t";
                        } else {
                            error += " ";
                        }
                    }

                    error += "^\n";

                    if (std::getline(stream, nextLine)) {
                        error += nextLine + "\n";
                    }
                    break;
                }
                currentLine++;
                prevLine = line;
            }
        }

        error += getName() + ": " + message + "\n";
        error += "  at " + token.location.filepath.string() + ":" + std::to_string(token.location.line) + ":" + std::to_string(token.location.column);

        std::cerr << error << std::endl;

        return "SyntaxError";
    }
}