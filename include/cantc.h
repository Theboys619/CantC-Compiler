#pragma once
#include "common.h"
#include "parser/lexer.h"
#include "parser/parser.h"
#include "parser/ast.h"
#include "parser/tree_printer.h"
#include "opts.h"

// #include "typechecker/typechecker.h"

#include "compiler/compiler.h"

namespace CantC {
    void printAST(BlockStmt* root);
    void run(Options options);
}