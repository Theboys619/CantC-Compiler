#include "parser/ast.h"

namespace CantC {
    Node::Node() : type(NodeType::EXPR_STMT), token() {}

    Type::Type() : Node(), name("void"), isPrimitive(false), isPointer(false), isArray(false), arraySize(0), pointerLevel(0) {
        type = NodeType::TYPE;
        isClass = false;
    }
    Type::Type(std::string name, bool isPrimitive, bool isPointer, bool isArray, int arraySize, int pointerLevel) 
        : Node(), name(name), isPrimitive(isPrimitive), isPointer(isPointer), isArray(isArray), arraySize(arraySize), pointerLevel(pointerLevel) {
        type = NodeType::TYPE;
        isClass = false;
    }

    ExprStatement::ExprStatement() : Statement(), expression(nullptr) {
        type = NodeType::EXPR_STMT;
    }

    ExprStatement::ExprStatement(Expression* expression) : Statement(), expression(expression) {
        type = NodeType::EXPR_STMT;
    }

    BlockStmt::BlockStmt() : Statement(), statements() {
        type = NodeType::BLOCK_STMT;
    }
    BlockStmt::BlockStmt(std::vector<Statement*> statements) : Statement(), statements(statements) {
        type = NodeType::BLOCK_STMT;
    }

    FunctionParam::FunctionParam() : Statement(), paramType(nullptr), name() {
        type = NodeType::FUNC_PARAM;
    }
    FunctionParam::FunctionParam(Type* paramType, Token name) : Statement(), paramType(paramType), name(name) {
        type = NodeType::FUNC_PARAM;
    }

    FuncDecl::FuncDecl() : Statement(), returnType(nullptr), name(), parameters(), body(nullptr) {
        type = NodeType::FUNC_DECL;
    }
    FuncDecl::FuncDecl(Type* returnType, Token name, std::vector<FunctionParam*> parameters, Statement* body) 
        : Statement(), returnType(returnType), name(name), parameters(parameters), body(body) {
        type = NodeType::FUNC_DECL;
    }

    FuncCall::FuncCall() : Expression(), callee(nullptr), arguments() {
        type = NodeType::FUNC_CALL;
    }
    FuncCall::FuncCall(Expression* callee, std::vector<Expression*> arguments) 
        : Expression(), callee(callee), arguments(arguments) {
        type = NodeType::FUNC_CALL;
    }

    ClassDecl::ClassDecl() : Statement(), name(), members(), methods() {
        type = NodeType::CLASS_DECL;
    }
    ClassDecl::ClassDecl(Token name, std::vector<VarDecl*> members, std::vector<FuncDecl*> methods) 
        : Statement(), name(name), members(members), methods(methods) {
        type = NodeType::CLASS_DECL;
    }

    IfStmt::IfStmt() : Statement(), condition(nullptr), thenBranch(nullptr), elseBranch(nullptr) {
        type = NodeType::IF_STMT;
    }
    IfStmt::IfStmt(Expression* condition, Statement* thenBranch, Statement* elseBranch) 
        : Statement(), condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
        type = NodeType::IF_STMT;
    }

    WhileStmt::WhileStmt() : Statement(), condition(nullptr), body(nullptr), isDoWhile(false) {
        type = NodeType::WHILE_STMT;
    }
    WhileStmt::WhileStmt(Expression* condition, Statement* body, bool isDoWhile) 
        : Statement(), condition(condition), body(body), isDoWhile(isDoWhile) {
        type = NodeType::WHILE_STMT;
    }

    ForStmt::ForStmt() : Statement(), initializer(nullptr), condition(nullptr), increment(nullptr), body(nullptr) {
        type = NodeType::FOR_STMT;
    }
    ForStmt::ForStmt(Statement* initializer, Expression* condition, Expression* increment, Statement* body) 
        : Statement(), initializer(initializer), condition(condition), increment(increment), body(body) {
        type = NodeType::FOR_STMT;
    }

    ReturnStmt::ReturnStmt() : Statement(), value(nullptr) {
        type = NodeType::RETURN_STMT;
    }
    ReturnStmt::ReturnStmt(Expression* value) : Statement(), value(value) {
        type = NodeType::RETURN_STMT;
    }

    BinaryExpr::BinaryExpr() : Expression(), op(), left(nullptr), right(nullptr) {
        type = NodeType::BINARY_EXPR;
    }
    BinaryExpr::BinaryExpr(Expression* left, Token op, Expression* right) 
        : Expression(), op(op), left(left), right(right) {
        type = NodeType::BINARY_EXPR;
    }

    UnaryExpr::UnaryExpr() : Expression(), op(), operand(nullptr) {
        type = NodeType::UNARY_EXPR;
    }
    UnaryExpr::UnaryExpr(Token op, Expression* operand) 
        : Expression(), op(op), operand(operand) {
        type = NodeType::UNARY_EXPR;
    }

    LiteralExpr::LiteralExpr() : Expression(), value(), valueType(nullptr) {
        type = NodeType::LITERAL_EXPR;
    }
    LiteralExpr::LiteralExpr(Token valueToken, Type* valueType) 
        : Expression(), value(valueToken), valueType(valueType) {
        type = NodeType::LITERAL_EXPR;
    }

    Identifier::Identifier() : Expression(), name() {
        type = NodeType::IDENTIFIER_EXPR;
    }
    Identifier::Identifier(Token name) : Expression(), name(name) {
        type = NodeType::IDENTIFIER_EXPR;
    }

    VarDecl::VarDecl() : Statement(), isConst(false), varType(nullptr), name() {
        type = NodeType::VAR_DECL;
    }
    VarDecl::VarDecl(bool isConst, Type* varType, Token name) 
        : Statement(), isConst(isConst), varType(varType), name(name) {
        type = NodeType::VAR_DECL;
    }

    Assign::Assign() : Expression(), target(nullptr), value(nullptr) {
        type = NodeType::ASSIGN;
    }
    Assign::Assign(Expression* target, Expression* value) 
        : Expression(), target(target), value(value) {
        type = NodeType::ASSIGN;
    }

    MemberExpr::MemberExpr() : Expression(), object(nullptr), memberName() {
        type = NodeType::MEMBER_EXPR;
    }
    MemberExpr::MemberExpr(Expression* object, Token memberName) 
        : Expression(), object(object), memberName(memberName) {
        type = NodeType::MEMBER_EXPR;
    }

    SubscriptExpr::SubscriptExpr() : Expression(), array(nullptr), index(nullptr) {
        type = NodeType::SUBSCRIPT_EXPR;
    }
    SubscriptExpr::SubscriptExpr(Expression* array, Expression* index) 
        : Expression(), array(array), index(index) {
        type = NodeType::SUBSCRIPT_EXPR;
    }
};