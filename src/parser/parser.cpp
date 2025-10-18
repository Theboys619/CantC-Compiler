#include "parser/parser.h"

namespace CantC {
    Parser::Parser() : lexer(), currentToken(), source() {
        className = "";
    }

    void Parser::loadFromFile(fs::path filepath) {
        lexer.loadFromFile(filepath);
        source = std::string(lexer.source);
        std::cout << "Loaded source from file: " << filepath << std::endl;
        advance();
    }

    void Parser::loadFromString(std::string source) {
        lexer.loadFromString(source);
        this->source = std::string(lexer.source);
        advance();
    }

    BlockStmt* Parser::parse() {
        std::vector<Statement*> statements;
        while (currentToken.type != TokenType::END_OF_FILE) {
            statements.push_back(parseStatement());
        }

        BlockStmt* root = new BlockStmt(statements);
        root->type = NodeType::BLOCK_STMT;
        return root;
    }

    void Parser::advance() {
        currentToken = lexer.nextToken();
    }

    bool Parser::check(TokenType type) const {
        return currentToken.type == type;
    }

    bool Parser::checkAny(std::initializer_list<TokenType> types) const {
        for (TokenType type : types) {
            if (check(type)) {
                return true;
            }
        }
        return false;
    }

    bool Parser::match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }
    bool Parser::matchAny(std::initializer_list<TokenType> types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    void Parser::consume(TokenType type, const std::string& errorMessage) {
        if (check(type)) {
            advance();
        } else {
            throw SyntaxError(errorMessage, currentToken, std::string(source));
        }
    }

    Statement* Parser::parseVarDeclaration() {
        bool isConst = (currentToken.value == "const");
        advance(); // consume 'let' or 'const'
        // Type* varType = parseType();
        Token name = currentToken;
        consume(TokenType::IDENTIFIER, "Expected variable name.");

        Expression* initializer = nullptr;
        if (match(TokenType::EQUAL)) {
            initializer = parseExpression();
        }

        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");

        VarDecl* varDecl = new VarDecl(isConst, nullptr, name);
        varDecl->initializer = initializer;
        varDecl->type = NodeType::VAR_DECL;
        return varDecl;
    }

    Statement* Parser::parseFunctionParameters() {
        if (check(TokenType::SPREAD)) {
            Token spreadToken = currentToken;
            advance(); // consume '...'
            return new FunctionParam(new Type("..."), spreadToken);
        }
        // Type* paramType = parseType();
        Token name = currentToken;
        consume(TokenType::IDENTIFIER, "Expected parameter name.");
        
        FunctionParam* param = new FunctionParam(nullptr, name);
        return param;
    }

    Statement* Parser::parseFunctionDeclaration() {

        bool isExported = false;
        bool isExtern = false;
        if (check(TokenType::KEYWORD) && currentToken.value == "export") {
            advance(); // consume 'export'
            isExported = true;
        } else if (check(TokenType::KEYWORD) && currentToken.value == "extern") {
            advance(); // consume 'extern'
            isExtern = true;
        }
        advance(); // consume 'func'

        Type* returnType = nullptr;

        Token name = currentToken;
        consume(TokenType::IDENTIFIER, "Expected function name.");

        consume(TokenType::LEFT_PAREN, "Expected '(' after function name.");
        std::vector<FunctionParam*> parameters;
        while (!check(TokenType::RIGHT_PAREN)) {
            parameters.push_back((FunctionParam*)parseFunctionParameters());
            if (!match(TokenType::COMMA)) break;
        }
        consume(TokenType::RIGHT_PAREN, "Expected ')' after function parameters.");

        if (check(TokenType::SEMICOLON)) {
            advance(); // consume ';'
            FuncDecl* funcDecl = new FuncDecl(returnType, name, parameters, nullptr);
            funcDecl->type = NodeType::FUNC_DECL;
            funcDecl->isExported = isExported;
            funcDecl->isExtern = isExtern;
            return funcDecl;
        }

        Statement* body = parseStatement();

        FuncDecl* funcDecl = new FuncDecl(returnType, name, parameters, body);
        funcDecl->type = NodeType::FUNC_DECL;
        funcDecl->isExported = isExported;
        funcDecl->isExtern = isExtern;
        return funcDecl;
    }

    Statement* Parser::parseClassDeclaration() {
        Token classToken = currentToken;
        advance(); // consume 'class'
        Token name = currentToken;
        consume(TokenType::IDENTIFIER, "Expected class name.");

        consume(TokenType::LEFT_BRACE, "Expected '{' after class name.");
        std::vector<VarDecl*> members;
        std::vector<FuncDecl*> methods;
        className = name.value;
        while (!check(TokenType::RIGHT_BRACE)) {
            Statement* member = parseStatement();
            if (member->type == NodeType::VAR_DECL) {
                members.push_back(static_cast<VarDecl*>(member));
            } else if (member->type == NodeType::FUNC_DECL) {
                methods.push_back(static_cast<FuncDecl*>(member));
            } else {
                throw SyntaxError("Expected variable declaration in class body.", currentToken, std::string(source));
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after class members.");
        className.clear();

        ClassDecl* classDecl = new ClassDecl(name, members, methods);
        classDecl->token = classToken;
        classDecl->type = NodeType::CLASS_DECL;
        return classDecl;
    }

    Statement* Parser::parseStatement() {
        if (check(TokenType::KEYWORD)) {
            if (currentToken.value == "let" || currentToken.value == "const") {
                return parseVarDeclaration();
            } else if (currentToken.value == "func" || currentToken.value == "export" || currentToken.value == "extern") {
                return parseFunctionDeclaration();
            } else if (currentToken.value == "class") {
                return parseClassDeclaration();
            } else if (currentToken.value == "if") {
                return parseIfStatement();
            } else if (currentToken.value == "while") {
                return parseWhileStatement();
            } else if (currentToken.value == "for") {
                return parseForStatement();
            } else if (currentToken.value == "return") {
                return parseReturnStatement();
            } else if (currentToken.value == "break") {
                return parseBreakStatement();
            } else if (currentToken.value == "continue") {
                return parseContinueStatement();
            }
        }
        if (check(TokenType::LEFT_BRACE)) {
            BlockStmt* blockStmt = new BlockStmt(parseBlock());
            blockStmt->type = NodeType::BLOCK_STMT;
            return blockStmt;
        }
        return parseExpressionStatement();
    }

    Statement* Parser::parseIfStatement() {
        advance(); // consume 'if'
        consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'.");
        Expression* condition = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after condition.");

        Statement* thenBranch = parseStatement();
        Statement* elseBranch = nullptr;
        if (check(TokenType::KEYWORD) && currentToken.value == "else") {
            advance();
            elseBranch = parseStatement();
        }

        IfStmt* ifStmt = new IfStmt(condition, thenBranch, elseBranch);
        ifStmt->type = NodeType::IF_STMT;
        return ifStmt;
    }

    Statement* Parser::parseWhileStatement() {
        advance(); // consume 'while'
        consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'.");
        Expression* condition = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after condition.");

        Statement* body = parseStatement();

        WhileStmt* whileStmt = new WhileStmt(condition, body);
        whileStmt->type = NodeType::WHILE_STMT;
        return whileStmt;
    }

    Statement* Parser::parseForStatement() {
        advance(); // consume 'for'
        consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'.");
        Statement* initializer = nullptr;
        if (!check(TokenType::SEMICOLON)) {
            if (check(TokenType::KEYWORD) && (currentToken.value == "let" || currentToken.value == "const")) {
                initializer = parseVarDeclaration();
            } else {
                initializer = parseExpressionStatement();
                consume(TokenType::SEMICOLON, "Expected ';' after initializer.");
            }
        }

        Expression* condition = nullptr;
        if (!check(TokenType::SEMICOLON)) {
            condition = parseExpression();
        }
        consume(TokenType::SEMICOLON, "Expected ';' after condition.");

        Expression* increment = nullptr;
        if (!check(TokenType::RIGHT_PAREN)) {
            increment = parseExpression();
        }
        consume(TokenType::RIGHT_PAREN, "Expected ')' after for loop.");

        Statement* body = parseStatement();

        ForStmt* forStmt = new ForStmt(initializer, condition, increment, body);
        forStmt->type = NodeType::FOR_STMT;
        return forStmt;
    }

    Statement* Parser::parseReturnStatement() {
        Token returnToken = currentToken;
        advance(); // consume 'return'
        
        Expression* value = parseExpression();

        consume(TokenType::SEMICOLON, "Expected ';' after return value.");

        ReturnStmt* returnStmt = new ReturnStmt(value);
        returnStmt->type = NodeType::RETURN_STMT;
        returnStmt->token = returnToken;
        return returnStmt;
    }

    Statement* Parser::parseBreakStatement() {
        advance(); // consume 'break'

        BreakStmt* breakStmt = new BreakStmt();
        breakStmt->type = NodeType::BREAK_STMT;
        return breakStmt;
    }
    Statement* Parser::parseContinueStatement() {
        advance(); // consume 'continue'

        ContinueStmt* continueStmt = new ContinueStmt();
        continueStmt->type = NodeType::CONTINUE_STMT;
        return continueStmt;
    }
    Statement* Parser::parseBlockStatement() {
        consume(TokenType::LEFT_BRACE, "Expected '{' after 'block'.");
        std::vector<Statement*> statements;
        while (!check(TokenType::RIGHT_BRACE) && !lexer.isAtEnd()) {
            statements.push_back(parseStatement());
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");

        BlockStmt* blockStmt = new BlockStmt(statements);
        blockStmt->type = NodeType::BLOCK_STMT;
        return blockStmt;
    }
    Statement* Parser::parseExpressionStatement() {
        Expression* expr = parseExpression();
        consume(TokenType::SEMICOLON, "Expected ';' after expression.");

        ExprStatement* exprStmt = new ExprStatement(expr);
        exprStmt->type = NodeType::EXPR_STMT;
        return exprStmt;
    }
    Expression* Parser::parseExpression() {
        Token exprToken = currentToken;
        Expression* expr = parseAssignment();
        if (expr->token.type == TokenType::END_OF_FILE) {
            expr->token = exprToken;
        }
        return expr;
    }
    Expression* Parser::parseAssignment() {
        Expression* left = parseLogicalOr();
        if (checkAny({TokenType::EQUAL})) {
            Token operatorToken = currentToken;
            advance(); // consume operator
            Expression* right = parseAssignment();

            Assign* assignExpr = new Assign(left, right);
            assignExpr->type = NodeType::ASSIGN;
            return assignExpr;
        }
        return left;
    }
    // POSTFIX operators (e.g., func calls, member access, array subscripts) can be added here later
    Expression* Parser::parseLogicalOr() {
        Expression* left = parseLogicalAnd();
        while (checkAny({TokenType::OR})) {
            Token operatorToken = currentToken;
            advance(); // consume operator
            Expression* right = parseLogicalAnd();
            left = new BinaryExpr(left, operatorToken, right);
            left->type = NodeType::BINARY_EXPR;
        }
        return left;
    }
    Expression* Parser::parseLogicalAnd() {
        Expression* left = parseEquality();
        while (checkAny({TokenType::AND})) {
            Token operatorToken = currentToken;
            advance(); // consume operator
            Expression* right = parseEquality();
            left = new BinaryExpr(left, operatorToken, right);
            left->type = NodeType::BINARY_EXPR;
        }
        return left;
    }
    Expression* Parser::parseEquality() {
        Expression* left = parseComparison();
        while (checkAny({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL})) {
            Token operatorToken = currentToken;
            advance(); // consume operator
            Expression* right = parseComparison();
            left = new BinaryExpr(left, operatorToken, right);
            left->type = NodeType::BINARY_EXPR;
        }
        return left;
    }
    Expression* Parser::parseComparison() {
        Expression* left = parseTerm();
        while (checkAny({TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL})) {
            Token operatorToken = currentToken;
            advance(); // consume operator
            Expression* right = parseTerm();
            left = new BinaryExpr(left, operatorToken, right);
            left->type = NodeType::BINARY_EXPR;
        }
        return left;
    }
    Expression* Parser::parseTerm() {
        Expression* left = parseFactor();
        while (checkAny({TokenType::PLUS, TokenType::MINUS})) {
            Token operatorToken = currentToken;
            advance(); // consume operator
            Expression* right = parseFactor();
            left = new BinaryExpr(left, operatorToken, right);
            left->type = NodeType::BINARY_EXPR;
        }
        return left;
    }
    Expression* Parser::parseFactor() {
        Expression* left = parseUnary();
        while (checkAny({TokenType::STAR, TokenType::SLASH, TokenType::MODULO})) {
            Token operatorToken = currentToken;
            advance(); // consume operator
            Expression* right = parseUnary();
            left = new BinaryExpr(left, operatorToken, right);
            left->type = NodeType::BINARY_EXPR;
        }
        return left;
    }
    Expression* Parser::parseUnary() {
        if (checkAny({TokenType::BANG, TokenType::MINUS, TokenType::STAR, TokenType::AMPERSAND, TokenType::INCREMENT, TokenType::DECREMENT})) {
            Token operatorToken = currentToken;
            advance(); // consume operator
            Expression* operand = parseUnary();

            UnaryExpr* unaryExpr = new UnaryExpr(operatorToken, operand);
            unaryExpr->type = NodeType::UNARY_EXPR;
            return unaryExpr;
        }
        return parsePrefix();
    }
    Expression* Parser::parsePrefix() {
        // if (checkAny({TokenType::LEFT_PAREN})) {
        //     Token operatorToken = currentToken;
        //     advance(); // consume operator
        //     Type* castType = parseType();
        //     consume(TokenType::RIGHT_PAREN, "Expected ')' after type in cast expression.");

        //     Expression* operand = parsePrefix();

        //     UnaryExpr* castExpr = new UnaryExpr(operatorToken, operand);
        //     castExpr->unaryType = castType;
        //     castExpr->type = NodeType::UNARY_EXPR;
        //     return castExpr;
        // }
        return parsePostFix();
    }
    Expression* Parser::parsePrimary() {
        if (check(TokenType::NUMBER) || check(TokenType::STRING)) {
            Token valueToken = currentToken;
            Type* valueType = new Type();
            if (valueToken.type == TokenType::NUMBER) {
                valueType->name = "int";
                valueType->isPrimitive = true;
                valueType->pointerLevel = 0;
                valueType->isPointer = false;
            } else if (valueToken.type == TokenType::STRING) {
                valueType->name = "char";
                valueType->isPrimitive = true;
                valueType->isPointer = true;
                valueType->pointerLevel = 1;
            }

            advance(); // consume literal

            LiteralExpr* literalExpr = new LiteralExpr(valueToken, valueType);
            literalExpr->type = NodeType::LITERAL_EXPR;
            return literalExpr;
        }
        if (check(TokenType::IDENTIFIER)) {
            Identifier* identifier = new Identifier(currentToken);
            identifier->type = NodeType::IDENTIFIER_EXPR;
            identifier->token = currentToken;

            advance(); // consume identifier
            return identifier;
        }
        if (match(TokenType::LEFT_PAREN)) {
            Expression* expr = parseExpression();
            consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
            return expr;
        }
        std::cout << currentToken.value << " " << (int)currentToken.type << std::endl;
        throw SyntaxError("Expected expression.", currentToken, std::string(source));
    }
    Expression* Parser::parsePostFix() {
        Expression* expr = parsePrimary();
        while (true) {
            Token prevToken = currentToken;
            if (match(TokenType::LEFT_PAREN)) {
                std::vector<Expression*> arguments;
                if (!check(TokenType::RIGHT_PAREN)) {
                    do {
                        arguments.push_back(parseExpression());
                    } while (match(TokenType::COMMA));
                }
                consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments.");

                FuncCall* funcCall = new FuncCall(expr, arguments);
                funcCall->type = NodeType::FUNC_CALL;
                funcCall->token = prevToken;

                if (expr->type == NodeType::IDENTIFIER_EXPR) {
                    funcCall->isAnonymous = false;
                } else {
                    funcCall->isAnonymous = true;
                }

                expr = funcCall;
            } else if (match(TokenType::DOT)) {
                Token memberName = currentToken;
                consume(TokenType::IDENTIFIER, "Expected member name after '.'.");

                MemberExpr* memberExpr = new MemberExpr(expr, memberName);
                memberExpr->type = NodeType::MEMBER_EXPR;
                memberExpr->token = memberName;
                expr = memberExpr;

                // obj.member1.member2
                // MEMBER[MEMBER[FUNC(), MEMBER1], MEMBER2]
            } else if (match(TokenType::LEFT_BRACKET)) {
                Token exprToken = currentToken;
                Expression* index = parseExpression();
                consume(TokenType::RIGHT_BRACKET, "Expected ']' after index.");

                SubscriptExpr* subscriptExpr = new SubscriptExpr(expr, index);
                subscriptExpr->type = NodeType::SUBSCRIPT_EXPR;
                subscriptExpr->token = exprToken;
                expr = subscriptExpr;
            } else if (checkAny({TokenType::INCREMENT, TokenType::DECREMENT})) {
                Token operatorToken = currentToken;
                advance(); // consume operator

                BinaryExpr* postFixExpr = new BinaryExpr(expr, operatorToken, nullptr);
                postFixExpr->type = NodeType::BINARY_EXPR;
                expr = postFixExpr;
            } else {
                break;
            }
        }
        return expr;
    }
    Type* Parser::parseType(bool allowVoid) {
        if (check(TokenType::IDENTIFIER)) {
            std::string typeName = currentToken.value;
            Type* type = new Type();
            type->type = NodeType::TYPE;
            type->name = typeName;
            type->isPrimitive = false;
            type->isPointer = false;
            type->pointerLevel = 0;
            type->isArray = false;
            type->arraySize = 0;
            type->token = currentToken;

            advance(); // consume type name

            if (!allowVoid && typeName == "void" && !match(TokenType::STAR)) {
                throw SyntaxError("Void type not allowed here.", currentToken, std::string(source));
            }
            if (std::find(primitiveTypes.begin(), primitiveTypes.end(), typeName) != primitiveTypes.end()) {
                type->isPrimitive = true;
            }

            while (match(TokenType::STAR)) {
                type->isPointer = true;
                type->pointerLevel++;
            }

            if (match(TokenType::LEFT_BRACKET)) {
                if (check(TokenType::NUMBER)) {
                    int arraySize = std::stoi(currentToken.value);
                    advance();
                    consume(TokenType::RIGHT_BRACKET, "Expected ']' after array size.");
                    type->isArray = true;
                    type->arraySize = arraySize;
                } else {
                    throw SyntaxError("Expected array size.", currentToken, std::string(source));
                }
            }

            return type;
        }
        throw SyntaxError("Expected type.", currentToken, std::string(source));
    }
    std::vector<Statement*> Parser::parseBlock() {
        consume(TokenType::LEFT_BRACE, "Expected '{' to start block.");
        std::vector<Statement*> statements;
        while (!check(TokenType::RIGHT_BRACE) && currentToken.type != TokenType::END_OF_FILE) {
            statements.push_back(parseStatement());
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to end block.");
        return statements;
    }
    int Parser::getOperatorPrecedence(const std::string& op) const {
        auto it = operatorPrecedence.find(op);
        if (it != operatorPrecedence.end()) {
            return it->second;
        }
        return -1; // Not an operator
    }
}