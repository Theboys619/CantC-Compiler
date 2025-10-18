#pragma once

#include "parser/ast.h"
#include "compiler/instructions.h"
#include "common.h"
#include "opts.h"

namespace CantC {
    class Constant {
    public:
        enum class ConstType {
            INT,
            DOUBLE,
            STRING,
            BOOL
        } type;
        union {
            int intValue;
            double doubleValue;
            bool boolValue;
        };
        std::string stringValue;

        Constant(int value);
        Constant(double value);
        Constant(std::string value);
        Constant(bool value);
        Constant(const Constant& o);

        ConstType getType() const;

        int asInt() const;
        double asFloat() const;
        std::string asString() const;
        bool asBool() const;

        ~Constant();
    };
    class ConstantList {
    public:
        std::vector<Constant*> constants;

        ConstantList() = default;
        int addConstant(int constant);
        int addConstant(double constant);
        int addConstant(bool constant);
        int addConstant(std::string constant);
        int addConstant(Constant* constant);

        int size() const;
        Constant* getConstant(int index);
        int findConstant(int constant);
        int findConstant(double constant);
        int findConstant(bool constant);
        int findConstant(std::string constant);
        int findConstant(Constant* constant);

        ~ConstantList();
    };
    class Compiler {
    public:
        Compiler(Options options);
        void compile(BlockStmt* root);

        ~Compiler();
    private:
        Options options;
        ConstantList constantList;

        std::vector<uint8_t> bytecode;

        bool funcReturned;

        void emit(Instruction byte);
        void emit(uint8_t byte);
        void emit(std::vector<uint8_t> bytes);
        void insert(size_t position, uint8_t byte);
        void insert(size_t position, std::vector<uint8_t> bytes);

        void compileConstants();
        
        void compileBlock(BlockStmt* block);
        void compileStatement(Statement* stmt);
        void compileExpression(Expression* expr);
        void compileFunction(FuncDecl* func, bool isMethod = false);
        void compileConstructor(ClassDecl* classDecl, FuncDecl* func);
        void compileVarDecl(VarDecl* varDecl);
        void compileIfStmt(IfStmt* ifStmt);
        void compileWhileStmt(WhileStmt* whileStmt);
        void compileForStmt(ForStmt* forStmt);
        void compileReturnStmt(ReturnStmt* returnStmt);
        void compileBreakStmt(BreakStmt* breakStmt);
        void compileContinueStmt(ContinueStmt* continueStmt);
        void compileAssign(Assign* assign);
        void compileBinaryExpr(BinaryExpr* binExpr);
        void compileUnaryExpr(UnaryExpr* unaryExpr);
        void compileLiteralExpr(LiteralExpr* literalExpr);
        void compileIdentifier(Identifier* identifier);
        void compileMemberExpr(MemberExpr* memberExpr);
        void compileSubscriptExpr(SubscriptExpr* subscriptExpr);
        void compileFuncCall(FuncCall* funcCall);
        void compileClassDecl(ClassDecl* classDecl);

        void writeBytecode();
    };
}