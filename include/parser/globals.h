#pragma once

#include "common.h"

namespace CantC {
    //keywords for lexer
    const std::vector<std::string> keywords = {
        "if", "else", "while", "for", "return", "func", "extern",
        
        "let", "const", "true", "false", "null", "break", "continue",

        "class", "inherits", "import", "as", "export"
    };

    const std::vector<std::string> primitiveTypes = {
        "int", "float", "char", "bool", "void"
    };

    const std::unordered_map<std::string, int> primitiveSizes = {
        {"int", 4},
        {"float", 4},
        {"bool", 1},
        {"char", 1}, //pointer size
        {"void", 0},
    };

    const std::unordered_map<std::string, int> operatorPrecedence = {
        {"=", 40},
        {"+=", 40},
        {"-=", 40},
        {"*=", 40},
        {"/=", 40},
        {"||", 50},
        {"&&", 60},
        {"==", 70},
        {"!=", 70},
        {"<", 80},
        {">", 80},
        {"<=", 80},
        {">=", 80},
        {"+", 90},
        {"-", 90},
        {"*", 100},
        {"/", 100},
        {"%", 100},
    };
};