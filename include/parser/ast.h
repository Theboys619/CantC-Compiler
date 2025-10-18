#pragma once

#include "common.h"
#include "parser/token.h"

namespace CantC {

    enum class NodeType {
        EXPR_STMT, TYPE,

        VAR_DECL, ASSIGN,
        FUNC_DECL, FUNC_CALL, FUNC_PARAM,
        
        CLASS_DECL,

        IF_STMT, WHILE_STMT, FOR_STMT,
        RETURN_STMT, BREAK_STMT, CONTINUE_STMT,
        
        BLOCK_STMT,

        BINARY_EXPR, UNARY_EXPR,
        LITERAL_EXPR, IDENTIFIER_EXPR,

        MEMBER_EXPR, SUBSCRIPT_EXPR,
    };

    class Node {
    public:
        NodeType type;
        Token token; // The token that generated this node, for error reporting

        Node();
    };

    class Expression : public Node {
    public:
    
    };

    class Statement : public Node {
    public:
    
    };

    class ExprStatement : public Statement {
    public:
        Expression* expression;

        ExprStatement();
        ExprStatement(Expression* expression);
    };

    class Type : public Node {
    public:
        std::string name;
        bool isPrimitive;
        bool isPointer;
        bool isArray;
        bool isClass;
        int arraySize;
        int pointerLevel;

        Type();
        Type(std::string name, bool isPrimitive = false, bool isPointer = false, bool isArray = false, int arraySize = 0, int pointerLevel = 0);
    };

    class Identifier : public Expression {
    public:
        Type* idType;
        Token name;

        Identifier();
        Identifier(Token name);
    };

    class VarDecl : public Statement {
    public:
        bool isConst;
        Type* varType; // Can be empty for type inference
        Token name;

        Expression* initializer; // Can be empty

        VarDecl();
        VarDecl(bool isConst, Type* varType, Token name);
    };

    class Assign : public Expression {
    public:
        Type* assignType; // Can be empty for type inference

        Expression* target;
        Expression* value;

        Assign();
        Assign(Expression* target, Expression* value);
    };

    class BlockStmt : public Statement {
    public:
        Type* blockType; // Can be empty for type inference
        std::vector<Statement*> statements;

        BlockStmt();
        BlockStmt(std::vector<Statement*> statements);
    };

    class FunctionParam : public Statement {
    public:
        Type* paramType;
        Token name;

        FunctionParam();
        FunctionParam(Type* paramType, Token name);
    };

    class FuncDecl : public Statement {
    public:
        Type* returnType; // Can be empty for type inference
        Token name;
        std::vector<FunctionParam*> parameters;
        Statement* body;

        bool isExported;
        bool isExtern;

        FuncDecl();
        FuncDecl(Type* returnType, Token name, std::vector<FunctionParam*> parameters, Statement* body);
    };

    class FuncCall : public Expression {
    public:
        Type* callType; // Can be empty for type inference
        Expression* callee;
        std::vector<Expression*> arguments;

        bool isAnonymous;

        FuncCall();
        FuncCall(Expression* callee, std::vector<Expression*> arguments);
    };

    class ClassDecl : public Statement {
    public:
        Token name;
        std::vector<VarDecl*> members;
        std::vector<FuncDecl*> methods;

        bool isExported;

        ClassDecl();
        ClassDecl(Token name, std::vector<VarDecl*> members, std::vector<FuncDecl*> methods);
    };

    class IfStmt : public Statement {
    public:
        Type* ifType; // Can be empty for type inference
        Expression* condition;
        Statement* thenBranch;
        Statement* elseBranch; // Can be empty

        IfStmt();
        IfStmt(Expression* condition, Statement* thenBranch, Statement* elseBranch);
    };

    class WhileStmt : public Statement {
    public:
        Type* whileType; // Can be empty for type inference
        Expression* condition;
        Statement* body;
        bool isDoWhile;

        WhileStmt();
        WhileStmt(Expression* condition, Statement* body, bool isDoWhile = false);
    };

    class ForStmt : public Statement {
    public:
        Type* forType; // Can be empty for type inference
        Statement* initializer; // Can be empty
        Expression* condition; // Can be empty
        Expression* increment; // Can be empty
        Statement* body;

        ForStmt();
        ForStmt(Statement* initializer, Expression* condition, Expression* increment, Statement* body);
    };

    class ReturnStmt : public Statement {
    public:
        Type* returnType; // Can be empty for type inference
        Expression* value; // Can be empty

        ReturnStmt();
        ReturnStmt(Expression* value);
    };

    class BreakStmt : public Statement {
    public:
    };

    class ContinueStmt : public Statement {
    public:
    };

    class BinaryExpr : public Expression {
    public:
        Type* binType; // Can be empty for type inference
        Token op;
        Expression* left;
        Expression* right;

        BinaryExpr();
        BinaryExpr(Expression* left, Token op, Expression* right);
    };

    class UnaryExpr : public Expression {
    public:
        Type* unaryType; // Can be empty for type inference
        Token op;
        Expression* operand;

        UnaryExpr();
        UnaryExpr(Token op, Expression* operand);
    };

    class LiteralExpr : public Expression {
    public:
        Token value;
        Type* valueType;

        LiteralExpr();
        LiteralExpr(Token value, Type* valueType);
    };

    class MemberExpr : public Expression {
    public:
        Type* memberType; // Can be empty for type inference
        Expression* object;
        Token memberName;

        MemberExpr();
        MemberExpr(Expression* object, Token memberName);
    };

    class SubscriptExpr : public Expression {
    public:
        Type* subscriptType; // Can be empty for type inference
        Expression* array;
        Expression* index;

        SubscriptExpr();
        SubscriptExpr(Expression* array, Expression* index);
    };
};

/**
 * test(2).x.y = 4;
 * 
 * MEMBER[]
 * FUNCCALL[2]
 * 
 * ASSIGN[MEMBER[FUNCCALL[2], MEMBER[IDENTIFIER[x], IDENTIFIER[y]]], 4]
 * ASSIGN[MEMBER[MEMBER[FUNCCALL[2], IDENTIFIER[x]], IDENTIFIER[y]], 4]
 * 
 * test(2)["x"][returny()] = 4;
 * 
 * MEMBER[]
 * FUNCCALL[2]
 * FUNCCALL[]
 * 
 * ASSIGN[SUBSCRIPT[FUNCCALL[2], SUBSCRIPT[STR[x], FUNCCALL[]]], 4]
 */