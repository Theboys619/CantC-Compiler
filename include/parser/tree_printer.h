#pragma once
#include "parser/ast.h"

namespace CantC {
    class TreePrinter {
    public:
        static void print(Node* node, int indent = 0);

    private:
        static void printIndent(int indent);
        static void printExpression(Expression* expr, int indent);
        static void printStatement(Statement* stmt, int indent);
        static void printType(Type* type, int indent);
    };
}