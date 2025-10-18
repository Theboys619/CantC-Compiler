#pragma once

#include "common.h"
#include "parser/lexer.h"
#include "parser/ast.h"
#include "parser/globals.h"
#include "errors/syntax_error.h"

namespace CantC {
    class Parser {
    public:
        Parser();

        void loadFromFile(fs::path filepath);
        void loadFromString(std::string source);

        BlockStmt* parse();

    private:
        Lexer lexer;
        Token currentToken;

        std::string source;

        std::string className;

        void advance();
        bool check(TokenType type) const;
        bool checkAny(std::initializer_list<TokenType> types) const;
        bool match(TokenType type);
        bool matchAny(std::initializer_list<TokenType> types);
        void consume(TokenType type, const std::string& errorMessage);

        Statement* parseVarDeclaration();

        Statement* parseFunctionParameters();
        Statement* parseFunctionDeclaration();
        Statement* parseConstructor();

        Statement* parseClassDeclaration();
        Statement* parseStatement();
        Statement* parseIfStatement();
        Statement* parseWhileStatement();
        Statement* parseForStatement();
        Statement* parseReturnStatement();
        Statement* parseBreakStatement();
        Statement* parseContinueStatement();
        Statement* parseBlockStatement();

        Statement* parseExpressionStatement();
        
        Expression* parseExpression();
        Expression* parseAssignment();
        Expression* parseLogicalOr();
        Expression* parseLogicalAnd();
        Expression* parseEquality();
        Expression* parseComparison();
        Expression* parseTerm();
        Expression* parseFactor();
        Expression* parseUnary();
        Expression* parsePrefix();
        Expression* parsePrimary();
        Expression* parsePostFix();
        
        Type* parseType(bool allowVoid = false);
        
        std::vector<Statement*> parseBlock();

        int getOperatorPrecedence(const std::string& op) const;
        
        friend class TypeChecker;
        friend class Compiler;
    };
}