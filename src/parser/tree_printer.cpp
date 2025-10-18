#include "parser/tree_printer.h"

namespace CantC {
    void TreePrinter::print(Node* node, int indent) {
        if (!node) {
            printIndent(indent);
            std::cout << "null" << std::endl;
            return;
        }

        switch (node->type) {
            case NodeType::EXPR_STMT:
                printStatement(static_cast<Statement*>(node), indent);
                break;
            case NodeType::TYPE:
                printType(static_cast<Type*>(node), indent);
                break;
            case NodeType::VAR_DECL:
            case NodeType::ASSIGN:
            case NodeType::FUNC_DECL:
            case NodeType::FUNC_PARAM:
            case NodeType::CLASS_DECL:
            case NodeType::IF_STMT:
            case NodeType::WHILE_STMT:
            case NodeType::FOR_STMT:
            case NodeType::RETURN_STMT:
            case NodeType::BREAK_STMT:
            case NodeType::CONTINUE_STMT:
            case NodeType::BLOCK_STMT:
                printStatement(static_cast<Statement*>(node), indent);
                break;
            case NodeType::BINARY_EXPR:
            case NodeType::UNARY_EXPR:
            case NodeType::LITERAL_EXPR:
            case NodeType::IDENTIFIER_EXPR:
            case NodeType::MEMBER_EXPR:
            case NodeType::SUBSCRIPT_EXPR:
            case NodeType::FUNC_CALL:
                printExpression(static_cast<Expression*>(node), indent);
                break;
            default:
                printIndent(indent);
                std::cout << "Unknown node type" << std::endl;
                break;
        }
    }

    void TreePrinter::printIndent(int indent) {
        for (int i = 0; i < indent; i++) {
            std::cout << "  ";
        }
    }

    void TreePrinter::printExpression(Expression* expr, int indent) {
        if (!expr) {
            printIndent(indent);
            std::cout << "null" << std::endl;
            return;
        }

        switch (expr->type) {
            case NodeType::BINARY_EXPR: {
                auto* binExpr = static_cast<BinaryExpr*>(expr);
                printIndent(indent);
                std::cout << "BinaryExpr (" << binExpr->op.value << ")" << std::endl;
                printExpression(binExpr->left, indent + 1);
                printExpression(binExpr->right, indent + 1);
                break;
            }
            case NodeType::UNARY_EXPR: {
                auto* unExpr = static_cast<UnaryExpr*>(expr);
                printIndent(indent);
                std::cout << "UnaryExpr (" << unExpr->op.value << ")" << std::endl;
                printExpression(unExpr->operand, indent + 1);
                break;
            }
            case NodeType::ASSIGN: {
                auto* assign = static_cast<Assign*>(expr);
                printIndent(indent);
                std::cout << "Assign" << std::endl;
                printExpression(assign->target, indent + 1);
                printExpression(assign->value, indent + 1);
                break;
            }
            case NodeType::LITERAL_EXPR: {
                auto* litExpr = static_cast<LiteralExpr*>(expr);
                printIndent(indent);
                std::cout << "LiteralExpr (" << litExpr->value.value << ")" << std::endl;
                printType(litExpr->valueType, indent + 1);
                break;
            }
            case NodeType::IDENTIFIER_EXPR: {
                auto* idExpr = static_cast<Identifier*>(expr);
                printIndent(indent);
                std::cout << "Identifier (" << idExpr->name.value << ")" << std::endl;
                break;
            }
            case NodeType::MEMBER_EXPR: {
                auto* memExpr = static_cast<MemberExpr*>(expr);
                printIndent(indent);
                std::cout << "MemberExpr (" << memExpr->memberName.value << ")" << std::endl;
                printExpression(memExpr->object, indent + 1);
                break;
            }
            case NodeType::SUBSCRIPT_EXPR: {
                auto* subExpr = static_cast<SubscriptExpr*>(expr);
                printIndent(indent);
                std::cout << "SubscriptExpr" << std::endl;
                printExpression(subExpr->array, indent + 1);
                printExpression(subExpr->index, indent + 1);
                break;
            }
            case NodeType::FUNC_CALL: {
                auto* funcCall = static_cast<FuncCall*>(expr);
                printIndent(indent);
                std::cout << "FuncCall:" << std::endl;
                printIndent(indent + 1);
                std::cout << "Callee:" << std::endl;
                printExpression(funcCall->callee, indent + 2);
                printIndent(indent + 1);
                std::cout << "Arguments:" << std::endl;
                for (auto* arg : funcCall->arguments) {
                    printExpression(arg, indent + 2);
                }
                break;
            }
            default:
                printIndent(indent);
                std::cout << "Unknown expression type " << static_cast<int>(expr->type) << std::endl;
                break;
        }
    }
    void TreePrinter::printStatement(Statement* stmt, int indent) {
        if (!stmt) {
            printIndent(indent);
            std::cout << "null" << std::endl;
            return;
        }

        switch (stmt->type) {
            case NodeType::EXPR_STMT: {
                auto* exprStmt = static_cast<ExprStatement*>(stmt);
                printIndent(indent);
                std::cout << "ExprStatement" << std::endl;
                printExpression(exprStmt->expression, indent + 1);
                break;
            }
            case NodeType::VAR_DECL: {
                auto* varDecl = static_cast<VarDecl*>(stmt);
                printIndent(indent);
                std::cout << "VarDecl (" << (varDecl->isConst ? "const" : "let") << " " << varDecl->name.value << ")" << std::endl;
                if (varDecl->varType) {
                    printType(varDecl->varType, indent + 1);
                }
                break;
            }
            case NodeType::FUNC_DECL: {
                auto* funcDecl = static_cast<FuncDecl*>(stmt);
                printIndent(indent);
                std::cout << "FuncDecl (" << (funcDecl->isExported ? "export " : "") << funcDecl->name.value << ")" << std::endl;
                if (funcDecl->returnType) {
                    printType(funcDecl->returnType, indent + 1);
                }
                printIndent(indent + 1);
                std::cout << "Parameters:" << std::endl;
                for (auto* param : funcDecl->parameters) {
                    printStatement(param, indent + 2);
                }
                printIndent(indent + 1);
                std::cout << "Body:" << std::endl;
                printStatement(funcDecl->body, indent + 2);
                break;
            }
            case NodeType::FUNC_PARAM: {
                auto* param = static_cast<FunctionParam*>(stmt);
                printIndent(indent);
                std::cout << "FunctionParam (" << param->name.value << ")" << std::endl;
                if (param->paramType) {
                    printType(param->paramType, indent + 1);
                }
                break;
            }
            case NodeType::CLASS_DECL: {
                auto* classDecl = static_cast<ClassDecl*>(stmt);
                printIndent(indent);
                std::cout << "ClassDecl (" << classDecl->name.value << ")" << std::endl;
                printIndent(indent + 1);
                std::cout << "Members:" << std::endl;
                for (auto* member : classDecl->members) {
                    printStatement(member, indent + 2);
                }
                printIndent(indent + 1);
                std::cout << "Methods:" << std::endl;
                for (auto* method : classDecl->methods) {
                    printStatement(method, indent + 2);
                }
                break;
            }
            case NodeType::IF_STMT: {
                auto* ifStmt = static_cast<IfStmt*>(stmt);
                printIndent(indent);
                std::cout << "IfStmt" << std::endl;
                printIndent(indent + 1);
                std::cout << "Condition:" << std::endl;
                printExpression(ifStmt->condition, indent + 2);
                printIndent(indent + 1);
                std::cout << "ThenBranch:" << std::endl;
                printStatement(ifStmt->thenBranch, indent + 2);
                if (ifStmt->elseBranch) {
                    printIndent(indent + 1);
                    std::cout << "ElseBranch:" << std::endl;
                    printStatement(ifStmt->elseBranch, indent + 2);
                }
                break;
            }
            case NodeType::WHILE_STMT: {
                auto* whileStmt = static_cast<WhileStmt*>(stmt);
                printIndent(indent);
                std::cout << "WhileStmt" << std::endl;
                printIndent(indent + 1);
                std::cout << "Condition:" << std::endl;
                printExpression(whileStmt->condition, indent + 2);
                printIndent(indent + 1);
                std::cout << "Body:" << std::endl;
                printStatement(whileStmt->body, indent + 2);
                break;
            }
            case NodeType::FOR_STMT: {
                auto* forStmt = static_cast<ForStmt*>(stmt);
                printIndent(indent);
                std::cout << "ForStmt" << std::endl;
                if (forStmt->initializer) {
                    printIndent(indent + 1);
                    std::cout << "Initializer:" << std::endl;
                    printStatement(forStmt->initializer, indent + 2);
                }
                if (forStmt->condition) {
                    printIndent(indent + 1);
                    std::cout << "Condition:" << std::endl;
                    printExpression(forStmt->condition, indent + 2);
                }
                if (forStmt->increment) {
                    printIndent(indent + 1);
                    std::cout << "Increment:" << std::endl;
                    printExpression(forStmt->increment, indent + 2);
                }
                printIndent(indent + 1);
                std::cout << "Body:" << std::endl;
                printStatement(forStmt->body, indent + 2);
                break;
            }
            case NodeType::RETURN_STMT: {
                auto* returnStmt = static_cast<ReturnStmt*>(stmt);
                printIndent(indent);
                std::cout << "ReturnStmt" << std::endl;
                if (returnStmt->value) {
                    printExpression(returnStmt->value, indent + 1);
                }
                break;
            }
            case NodeType::BREAK_STMT: {
                printIndent(indent);
                std::cout << "BreakStmt" << std::endl;
                break;
            }
            case NodeType::CONTINUE_STMT: {
                printIndent(indent);
                std::cout << "ContinueStmt" << std::endl;
                break;
            }
            case NodeType::BLOCK_STMT: {
                auto* blockStmt = static_cast<BlockStmt*>(stmt);
                printIndent(indent);
                std::cout << "BlockStmt" << std::endl;
                for (auto* statement : blockStmt->statements) {
                    printStatement(statement, indent + 1);
                }
                break;
            }
            default:
                printIndent(indent);
                std::cout << "Unknown statement type" << std::endl;
                break;
        }
    }
    void TreePrinter::printType(Type* type, int indent) {
        if (!type) {
            printIndent(indent);
            std::cout << "null" << std::endl;
            return;
        }

        printIndent(indent);
        std::cout << "Type (" << type->name 
                  << (type->isPrimitive ? ", primitive" : "") 
                  << (type->isPointer ? ", pointer*" + std::to_string(type->pointerLevel) : "") 
                  << (type->isArray ? ", array[" + std::to_string(type->arraySize) + "]" : "") 
                  << ")" << std::endl;
    }
}