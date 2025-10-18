#include "compiler/compiler.h"

namespace CantC {
    /* Constant methods */
    Constant::Constant(const Constant& o) : type(o.type) {
        switch (type) {
            case ConstType::INT:
                intValue = o.intValue;
                break;
            case ConstType::DOUBLE:
                doubleValue = o.doubleValue;
                break;
            case ConstType::STRING:
                stringValue = std::string(o.stringValue);
                break;
            case ConstType::BOOL:
                boolValue = o.boolValue;
                break;
        }
    }
    Constant::Constant(int value) : type(ConstType::INT), intValue(value) {}
    Constant::Constant(double value) : type(ConstType::DOUBLE), doubleValue(value) {}
    Constant::Constant(std::string value) : type(ConstType::STRING), stringValue(std::string(value)) {}
    Constant::Constant(bool value) : type(ConstType::BOOL), boolValue(value) {}
    Constant::~Constant() {
        
    }
    Constant::ConstType Constant::getType() const {
        return type;
    }
    int Constant::asInt() const {
        if (type != ConstType::INT) {
            throw std::runtime_error("Constant is not of type INT");
        }
        return intValue;
    }
    double Constant::asFloat() const {
        if (type != ConstType::DOUBLE) {
            throw std::runtime_error("Constant is not of type DOUBLE");
        }
        return doubleValue;
    }
    std::string Constant::asString() const {
        if (type != ConstType::STRING) {
            throw std::runtime_error("Constant is not of type STRING");
        }
        return stringValue;
    }
    bool Constant::asBool() const {
        if (type != ConstType::BOOL) {
            throw std::runtime_error("Constant is not of type BOOL");
        }
        return boolValue;
    }

    /* ConstantList methods */
    int ConstantList::addConstant(Constant* constant) {
        int index = findConstant(constant);
        if (index != -1) {
            return index; // Return existing index if found
        }
        constants.push_back(constant);
        return constants.size() - 1;
    }
    int ConstantList::addConstant(int constant) {
        Constant* constObj = new Constant(constant);
        return addConstant(constObj);
    }
    int ConstantList::addConstant(double constant) {
        Constant* constObj = new Constant(constant);
        return addConstant(constObj);
    }
    int ConstantList::addConstant(bool constant) {
        Constant* constObj = new Constant(constant);
        return addConstant(constObj);
    }
    int ConstantList::addConstant(std::string constant) {
        Constant* constObj = new Constant(constant);
        return addConstant(constObj);
    }

    int ConstantList::size() const {
        return constants.size();
    }

    Constant* ConstantList::getConstant(int index) {
        return constants[index];
    }

    int ConstantList::findConstant(int constant) {
        for (size_t i = 0; i < constants.size(); i++) {
            Constant* c = constants[i];
            if (c->type == Constant::ConstType::INT && c->intValue == constant) {
                return i;
            }
        }
        return -1; // Not found
    }

    int ConstantList::findConstant(double constant) {
        for (size_t i = 0; i < constants.size(); i++) {
            Constant* c = constants[i];
            if (c->type == Constant::ConstType::DOUBLE && c->doubleValue == constant) {
                return i;
            }
        }
        return -1; // Not found
    }

    int ConstantList::findConstant(bool constant) {
        for (size_t i = 0; i < constants.size(); i++) {
            Constant* c = constants[i];
            if (c->type == Constant::ConstType::BOOL && c->boolValue == constant) {
                return i;
            }
        }
        return -1; // Not found
    }

    int ConstantList::findConstant(std::string constant) {
        for (size_t i = 0; i < constants.size(); i++) {
            Constant* c = constants[i];
            if (c->type == Constant::ConstType::STRING && c->stringValue == constant) {
                return i;
            }
        }
        return -1; // Not found
    }

    int ConstantList::findConstant(Constant* constant) {
        for (size_t i = 0; i < constants.size(); i++) {
            Constant* c = constants[i];
            if (c->type == constant->type) {
                switch (c->type) {
                    case Constant::ConstType::INT:
                        if (c->intValue == constant->intValue) return i;
                        break;
                    case Constant::ConstType::DOUBLE:
                        if (c->doubleValue == constant->doubleValue) return i;
                        break;
                    case Constant::ConstType::STRING:
                        if (c->stringValue == constant->stringValue) return i;
                        break;
                    case Constant::ConstType::BOOL:
                        if (c->boolValue == constant->boolValue) return i;
                        break;
                }
            }
        }
        return -1; // Not found
    }

    ConstantList::~ConstantList() {
        for (Constant* c : constants) {
            delete c;
        }
    }

    /* Compiler methods */
    Compiler::Compiler(Options options) : options(options), constantList() {
        bytecode = {};
        funcReturned = false;
    }

    Compiler::~Compiler() {
        
    }

    void Compiler::emit(Instruction byte) {
        bytecode.push_back(static_cast<uint8_t>(byte));
    }

    void Compiler::emit(uint8_t byte) {
        bytecode.push_back(byte);
    }

    void Compiler::emit(std::vector<uint8_t> bytes) {
        bytecode.insert(bytecode.end(), bytes.begin(), bytes.end());
    }

    void Compiler::insert(size_t position, uint8_t byte) {
        if (position > bytecode.size()) {
            throw std::out_of_range("Insert position is out of bytecode bounds.");
        }
        bytecode.insert(bytecode.begin() + position, byte);
    }

    void Compiler::insert(size_t position, std::vector<uint8_t> bytes) {
        if (position > bytecode.size()) {
            throw std::out_of_range("Insert position is out of bytecode bounds.");
        }
        bytecode.insert(bytecode.begin() + position, bytes.begin(), bytes.end());
    }

    void Compiler::compile(BlockStmt* root) {
        // Compilation logic will go here
        emit({'w','e','b','s',0x00,0x01});
        compileBlock(root);
        emit({0xFF}); // END SECTION instruction
        compileConstants();
        writeBytecode();
    }

    void Compiler::writeBytecode() {
        std::string filename = options.outputFile;
        if (filename.empty()) {
            filename = "a.out";
        }
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Could not open output file: " + filename);
        }
        outFile.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());

        outFile.close();
        if (options.verbose) {
            std::cout << "Wrote bytecode to " << filename << " (" << bytecode.size() << " bytes)" << std::endl;
        }

        if (options.debug) {
            std::ofstream debugFile(filename + ".dbg");
            if (!debugFile) {
                throw std::runtime_error("Could not open debug output file: " + filename + ".dbg");
            }

            // write bytecode as string
            for (size_t i = 0; i < bytecode.size(); i++) {
                debugFile << std::hex << (int)bytecode[i] << " ";
            }
            debugFile.close();
        }
    }

    void Compiler::compileConstants() {
        int position = 6; // After 'webs' header and version
        insert(position++, 0x01); // CONSTANT_POOL section
        uint32_t constCount = constantList.size();
        insert(position, {(uint8_t)((constCount >> 24) & 0xFF),
              (uint8_t)((constCount >> 16) & 0xFF),
              (uint8_t)((constCount >> 8) & 0xFF),
              (uint8_t)(constCount & 0xFF)});
        position += 4;
        std::cout << "Compiling with " << constCount << " constants." << std::endl;
        for (Constant* c : constantList.constants) {
            switch (c->type) {
                case Constant::ConstType::INT:
                    insert(position++, 0x01); // INT type
                    insert(position, {(uint8_t)((c->intValue >> 24) & 0xFF),
                          (uint8_t)((c->intValue >> 16) & 0xFF),
                          (uint8_t)((c->intValue >> 8) & 0xFF),
                          (uint8_t)(c->intValue & 0xFF)});
                    position += 4;
                    break;
                case Constant::ConstType::DOUBLE: {
                    insert(position++, 0x02); // DOUBLE type
                    uint64_t doubleBits;
                    std::memcpy(&doubleBits, &c->doubleValue, sizeof(doubleBits));
                    insert(position, {(uint8_t)((doubleBits >> 56) & 0xFF),
                          (uint8_t)((doubleBits >> 48) & 0xFF),
                          (uint8_t)((doubleBits >> 40) & 0xFF),
                          (uint8_t)((doubleBits >> 32) & 0xFF),
                          (uint8_t)((doubleBits >> 24) & 0xFF),
                          (uint8_t)((doubleBits >> 16) & 0xFF),
                          (uint8_t)((doubleBits >> 8) & 0xFF),
                          (uint8_t)(doubleBits & 0xFF)});
                    position += 8;
                    break;
                }
                case Constant::ConstType::STRING:
                    insert(position++, 0x04); // STRING type
                    insert(position, {(uint8_t)((c->stringValue.size() >> 24) & 0xFF),
                          (uint8_t)((c->stringValue.size() >> 16) & 0xFF),
                          (uint8_t)((c->stringValue.size() >> 8) & 0xFF),
                          (uint8_t)(c->stringValue.size() & 0xFF)});
                    position += 4;
                    for (char ch : c->stringValue) {
                        insert(position++, (uint8_t)ch);
                    }
                    std::cout << "Emitted string constant: " << c->stringValue << std::endl;
                    break;
                case Constant::ConstType::BOOL:
                    insert(position++, 0x03); // BOOL type
                    insert(position++, c->boolValue ? 1 : 0);
                    break;
            }
        }
    }

    void Compiler::compileBlock(BlockStmt* block) {
        for (Statement* stmt : block->statements) {
            compileStatement(stmt);
            if (stmt->type == NodeType::EXPR_STMT) {
                ExprStatement* exprStmt = static_cast<ExprStatement*>(stmt);
                if (exprStmt->expression->type == NodeType::FUNC_CALL) {
                    emit(Instruction::POP); // Pop function call result
                }
            }
        }
    }

    void Compiler::compileStatement(Statement* stmt) {
        switch (stmt->type) {
            case NodeType::BLOCK_STMT:
                compileBlock(static_cast<BlockStmt*>(stmt));
                break;
            case NodeType::FUNC_DECL:
                compileFunction(static_cast<FuncDecl*>(stmt));
                break;
            case NodeType::VAR_DECL:
                compileVarDecl(static_cast<VarDecl*>(stmt));
                break;
            case NodeType::IF_STMT:
                compileIfStmt(static_cast<IfStmt*>(stmt));
                break;
            case NodeType::WHILE_STMT:
                compileWhileStmt(static_cast<WhileStmt*>(stmt));
                break;
            case NodeType::FOR_STMT:
                compileForStmt(static_cast<ForStmt*>(stmt));
                break;
            case NodeType::RETURN_STMT:
                compileReturnStmt(static_cast<ReturnStmt*>(stmt));
                break;
            case NodeType::BREAK_STMT:
                compileBreakStmt(static_cast<BreakStmt*>(stmt));
                break;
            case NodeType::CONTINUE_STMT:
                compileContinueStmt(static_cast<ContinueStmt*>(stmt));
                break;
            case NodeType::EXPR_STMT: {
                std::cout << "Compiling expression statement." << std::endl;
                compileExpression(static_cast<ExprStatement*>(stmt)->expression);
                break;
            }
            case NodeType::CLASS_DECL:
                compileClassDecl(static_cast<ClassDecl*>(stmt));
                break;
            default:
                throw std::runtime_error("Unknown statement type in compilation.");
        }
    }

    void Compiler::compileExpression(Expression* expr) {
        switch (expr->type) {
            case NodeType::BINARY_EXPR:
                compileBinaryExpr(static_cast<BinaryExpr*>(expr));
                break;
            case NodeType::UNARY_EXPR:
                compileUnaryExpr(static_cast<UnaryExpr*>(expr));
                break;
            case NodeType::LITERAL_EXPR:
                compileLiteralExpr(static_cast<LiteralExpr*>(expr));
                break;
            case NodeType::IDENTIFIER_EXPR:
                compileIdentifier(static_cast<Identifier*>(expr));
                break;
            case NodeType::MEMBER_EXPR:
                compileMemberExpr(static_cast<MemberExpr*>(expr));
                break;
            case NodeType::SUBSCRIPT_EXPR:
                compileSubscriptExpr(static_cast<SubscriptExpr*>(expr));
                break;
            case NodeType::FUNC_CALL:
                compileFuncCall(static_cast<FuncCall*>(expr));
                break;
            case NodeType::ASSIGN:
                compileAssign(static_cast<Assign*>(expr));
                break;
            default:
                throw std::runtime_error("Unknown expression type in compilation.");
        }
    }

    void Compiler::compileClassDecl(ClassDecl* classDecl) {
        emit(0x02); // Class declaration
        emit({0x00, 0x00, 0x00, 0x00}); // Access Flags
        std::string name = classDecl->name.value;
        emit({(uint8_t)name.size()}); // Name length
        for (char c : name) {
            emit((uint8_t)c); // Name characters
        }
        emit({0x00}); // Superclass
        for (VarDecl* member : classDecl->members) {
            emit(0x03); // Field declaration
            emit({0x00, 0x00, 0x00, 0x00}); // Field access flags
            std::string memberName = member->name.value;
            emit({(uint8_t)memberName.size()}); // Field name length
            for (char c : memberName) {
                emit((uint8_t)c); // Field name characters
            }
        }
        for (FuncDecl* method : classDecl->methods) {
            if (method->name.value == classDecl->name.value) {
                compileConstructor(classDecl, method);
                continue;
            }
            compileFunction(method, true);
        }
    }

    void Compiler::compileConstructor(ClassDecl* classDecl, FuncDecl* func) {
        emit(0x05); // Constructor declaration opcode
        emit({0x00, 0x00, 0x00, 0x00}); // Access Flags

        funcReturned = false;

        std::string name = func->name.value;
        emit({(uint8_t)name.size()}); // Name length
        for (char c : name) {
            emit((uint8_t)c); // Name characters
        }

        size_t paramCount = func->parameters.size();
        emit({(uint8_t)paramCount}); // Number of parameters

        for (FunctionParam* param : func->parameters) {
            std::string paramName = param->name.value;
            emit({(uint8_t)paramName.size()}); // Param name length
            for (char c : paramName) {
                emit((uint8_t)c); // Param name characters
            }
        }

        int funcBodyStart = bytecode.size();
        // create variable 'this'
        emit(Instruction::LOAD_CONST);
        int classConstIndex = constantList.addConstant(classDecl->name.value);
        emit({(uint8_t)((classConstIndex >> 24) & 0xFF),
              (uint8_t)((classConstIndex >> 16) & 0xFF),
              (uint8_t)((classConstIndex >> 8) & 0xFF),
              (uint8_t)(classConstIndex & 0xFF)});
        emit(Instruction::NEW);
        emit(Instruction::STORE_VAR);
        std::string thisName = "this";
        emit({(uint8_t)thisName.size()});
        for (char c : thisName) {
            emit((uint8_t)c);
        }
        if (func->body) {
            compileStatement(func->body);
        }
        emit(Instruction::LOAD_VAR);
        emit({(uint8_t)thisName.size()});
        for (char c : thisName) {
            emit((uint8_t)c);
        }
        uint64_t funcSize = !funcReturned ? (bytecode.size() - funcBodyStart) + 1 : (bytecode.size() - funcBodyStart); // +1 for RET
        insert(funcBodyStart, {
            (uint8_t)((funcSize >> 56) & 0xFF),
            (uint8_t)((funcSize >> 48) & 0xFF),
            (uint8_t)((funcSize >> 40) & 0xFF),
            (uint8_t)((funcSize >> 32) & 0xFF),
            (uint8_t)((funcSize >> 24) & 0xFF),
            (uint8_t)((funcSize >> 16) & 0xFF),
            (uint8_t)((funcSize >> 8) & 0xFF),
            (uint8_t)(funcSize & 0xFF)
        });

        if (!funcReturned) {
            emit(Instruction::RET); // Constructors end with RET
        }
    }

    void Compiler::compileFunction(FuncDecl* func, bool isMethod) {
        if (isMethod) {
            emit(0x04); // Method declaration opcode
        } else {
            emit(0x05); // Function declaration opcode
        }
        emit({0x00, 0x00, 0x00, 0x00}); // Access Flags

        funcReturned = false;

        std::string name = func->name.value;
        emit({(uint8_t)name.size()}); // Name length
        for (char c : name) {
            emit((uint8_t)c); // Name characters
        }

        size_t paramCount = func->parameters.size();
        emit({(uint8_t)paramCount}); // Number of parameters

        for (FunctionParam* param : func->parameters) {
            std::string paramName = param->name.value;
            emit({(uint8_t)paramName.size()}); // Param name length
            for (char c : paramName) {
                emit((uint8_t)c); // Param name characters
            }
        }

        int funcBodyStart = bytecode.size();
        if (func->body) {
            compileStatement(func->body);
        }
        uint64_t funcSize = !funcReturned ? (bytecode.size() - funcBodyStart) + 1 : (bytecode.size() - funcBodyStart); // +1 for RET
        insert(funcBodyStart, {
            (uint8_t)((funcSize >> 56) & 0xFF),
            (uint8_t)((funcSize >> 48) & 0xFF),
            (uint8_t)((funcSize >> 40) & 0xFF),
            (uint8_t)((funcSize >> 32) & 0xFF),
            (uint8_t)((funcSize >> 24) & 0xFF),
            (uint8_t)((funcSize >> 16) & 0xFF),
            (uint8_t)((funcSize >> 8) & 0xFF),
            (uint8_t)(funcSize & 0xFF)
        });

        if (!funcReturned) {
            if (name == "main") {
                emit(Instruction::HALT); // Main function ends with HALT
            } else {
                emit(Instruction::RET); // Other functions end with RET
            }
        }
    }

    void Compiler::compileVarDecl(VarDecl* varDecl) {
        if (varDecl->initializer) {
            compileExpression(varDecl->initializer);
        }

        emit(Instruction::STORE_VAR); // Variable declaration opcode
        std::string name = varDecl->name.value;
        emit({(uint8_t)name.size()}); // Name length
        for (char c : name) {
            emit((uint8_t)c); // Name characters
        }
    }

    void Compiler::compileIfStmt(IfStmt* ifStmt) {
        compileExpression(ifStmt->condition);
        emit(Instruction::JZ); // Jump if zero opcode
        size_t jzPosition = bytecode.size();

        compileStatement(ifStmt->thenBranch);
        // size_t thenBranchEnd = bytecode.size();
        size_t afterThenPosition = bytecode.size();
        size_t jmpOffset = afterThenPosition - (jzPosition); // AKA Size of then branch + 4 bytes to accommodate jmpOffset itself
        insert(jzPosition, {
            (uint8_t)((jmpOffset >> 24) & 0xFF),
            (uint8_t)((jmpOffset >> 16) & 0xFF),
            (uint8_t)((jmpOffset >> 8) & 0xFF),
            (uint8_t)(jmpOffset & 0xFF)
        });

        if (ifStmt->elseBranch) {
            compileStatement(ifStmt->elseBranch);
        }
    }

    void Compiler::compileWhileStmt(WhileStmt* whileStmt) {
        size_t loopStart = bytecode.size();
        compileExpression(whileStmt->condition);
        emit(Instruction::JZ); // Jump if zero opcode
        size_t jzPosition = bytecode.size();

        compileStatement(whileStmt->body);
        emit(Instruction::JMP); // Unconditional jump opcode
        emit({
            (uint8_t)(((loopStart - (bytecode.size() + 8)) >> 24) & 0xFF),
            (uint8_t)(((loopStart - (bytecode.size() + 8)) >> 16) & 0xFF),
            (uint8_t)(((loopStart - (bytecode.size() + 8)) >> 8) & 0xFF),
            (uint8_t)((loopStart - (bytecode.size() + 8)) & 0xFF)
        });
        size_t afterBodyPosition = bytecode.size();
        size_t jzOffset = afterBodyPosition - (jzPosition);
        insert(jzPosition, {
            (uint8_t)((jzOffset >> 24) & 0xFF),
            (uint8_t)((jzOffset >> 16) & 0xFF),
            (uint8_t)((jzOffset >> 8) & 0xFF),
            (uint8_t)(jzOffset & 0xFF)
        });
    }

    void Compiler::compileForStmt(ForStmt* forStmt) {
        if (forStmt->initializer) {
            compileStatement(forStmt->initializer);
        }

        size_t loopStart = bytecode.size();

        if (forStmt->condition) {
            compileExpression(forStmt->condition);
        } else {
            emit(Instruction::LOAD_CONST);
            int constIndex = constantList.addConstant(1); // True condition
            emit({(uint8_t)((constIndex >> 24) & 0xFF),
                  (uint8_t)((constIndex >> 16) & 0xFF),
                  (uint8_t)((constIndex >> 8) & 0xFF),
                  (uint8_t)(constIndex & 0xFF)});
        }

        emit(Instruction::JZ); // Jump if zero opcode
        size_t jzPosition = bytecode.size();

        compileStatement(forStmt->body);

        if (forStmt->increment) {
            compileExpression(forStmt->increment);
            emit(Instruction::POP); // Pop increment result
        }

        emit(Instruction::JMP); // Unconditional jump opcode
        emit({
            (uint8_t)(((loopStart - (bytecode.size() + 8)) >> 24) & 0xFF),
            (uint8_t)(((loopStart - (bytecode.size() + 8)) >> 16) & 0xFF),
            (uint8_t)(((loopStart - (bytecode.size() + 8)) >> 8) & 0xFF),
            (uint8_t)((loopStart - (bytecode.size() + 8)) & 0xFF)
        });

        size_t loopEnd = bytecode.size();
        size_t jzOffset = loopEnd - (jzPosition);
        insert(jzPosition, {
            (uint8_t)((jzOffset >> 24) & 0xFF),
            (uint8_t)((jzOffset >> 16) & 0xFF),
            (uint8_t)((jzOffset >> 8) & 0xFF),
            (uint8_t)(jzOffset & 0xFF)
        });
    }

    void Compiler::compileReturnStmt(ReturnStmt* returnStmt) {
        if (returnStmt->value) {
            compileExpression(returnStmt->value);
        } else {
            emit(Instruction::LOAD_CONST);
            int constIndex = constantList.addConstant(0); // Assuming 0 represents 'null' or 'void'
            emit({(uint8_t)((constIndex >> 24) & 0xFF),
                  (uint8_t)((constIndex >> 16) & 0xFF),
                  (uint8_t)((constIndex >> 8) & 0xFF),
                  (uint8_t)(constIndex & 0xFF)});
        }
        emit(Instruction::RET); // Return opcode
    }

    void Compiler::compileBreakStmt(BreakStmt* breakStmt) {
        // Break statement compilation logic
    }

    void Compiler::compileContinueStmt(ContinueStmt* continueStmt) {
        // Continue statement compilation logic
    }

    void Compiler::compileMemberExpr(MemberExpr* memberExpr) {
        compileExpression(memberExpr->object);
        // store object in temp variable
        emit(Instruction::STORE_VAR);
        std::string tempVarName = "__temp_obj";
        emit({(uint8_t)tempVarName.size()});
        for (char c : tempVarName) {
            emit((uint8_t)c);
        }
        emit(Instruction::LOAD_VAR);
        emit({(uint8_t)tempVarName.size()});
        for (char c : tempVarName) {
            emit((uint8_t)c);
        }
        std::string memberName = memberExpr->memberName.value;
        std::cout << "Compiling member access: " << memberName << std::endl;
        uint32_t constantIndex = constantList.addConstant(memberName);
        emit(Instruction::LOAD_CONST);
        emit({(uint8_t)((constantIndex >> 24) & 0xFF),
              (uint8_t)((constantIndex >> 16) & 0xFF),
              (uint8_t)((constantIndex >> 8) & 0xFF),
              (uint8_t)(constantIndex & 0xFF)});
        emit(Instruction::LOAD_FIELD);
    }

    void Compiler::compileSubscriptExpr(SubscriptExpr* subscriptExpr) {
        compileExpression(subscriptExpr->array);
        compileExpression(subscriptExpr->index);
        emit(Instruction::LOAD_FIELD); // Using LOAD_FIELD for simplicity
    }

    void Compiler::compileAssign(Assign* assign) {        
        compileExpression(assign->value);
        if (assign->target->type == NodeType::IDENTIFIER_EXPR) {
            Identifier* id = static_cast<Identifier*>(assign->target);
            emit(Instruction::STORE_VAR); // Store variable opcode
            std::string name = id->name.value;
            emit({(uint8_t)name.size()}); // Name length
            for (char c : name) {
                emit((uint8_t)c); // Name characters
            }
        } else if (assign->target->type == NodeType::MEMBER_EXPR) {
            MemberExpr* memberExpr = static_cast<MemberExpr*>(assign->target);
            compileExpression(memberExpr->object);
            std::string memberName = memberExpr->memberName.value;
            uint32_t constantIndex = constantList.addConstant(memberName);
            emit(Instruction::LOAD_CONST);
            emit({(uint8_t)((constantIndex >> 24) & 0xFF),
                  (uint8_t)((constantIndex >> 16) & 0xFF),
                  (uint8_t)((constantIndex >> 8) & 0xFF),
                  (uint8_t)(constantIndex & 0xFF)});
            emit(Instruction::STORE_FIELD);
        } else if (assign->target->type == NodeType::SUBSCRIPT_EXPR) {
            SubscriptExpr* subscriptExpr = static_cast<SubscriptExpr*>(assign->target);
            compileExpression(subscriptExpr->array);
            compileExpression(subscriptExpr->index);
            emit(Instruction::STORE_FIELD); // Using STORE_FIELD for simplicity
        } else {
            throw std::runtime_error("Unsupported assignment target.");
        }
    }

    void Compiler::compileBinaryExpr(BinaryExpr* binExpr) {
        switch (binExpr->op.type) {
            case TokenType::INCREMENT: {
                if (binExpr->left->type != NodeType::IDENTIFIER_EXPR) {
                    throw std::runtime_error("Left operand of increment must be an identifier.");
                }
                Identifier* id = static_cast<Identifier*>(binExpr->left);
                emit(Instruction::LOAD_VAR);
                std::string name = id->name.value;
                emit({(uint8_t)name.size()}); // Name length
                for (char c : name) {
                    emit((uint8_t)c); // Name characters
                }
                emit(Instruction::LOAD_CONST);
                int constIndex = constantList.addConstant(1);
                emit({(uint8_t)((constIndex >> 24) & 0xFF),
                      (uint8_t)((constIndex >> 16) & 0xFF),
                      (uint8_t)((constIndex >> 8) & 0xFF),
                      (uint8_t)(constIndex & 0xFF)});
                emit(Instruction::ADD);
                emit(Instruction::DUP);
                emit(Instruction::STORE_VAR);
                emit({(uint8_t)name.size()}); // Name length
                for (char c : name) {
                    emit((uint8_t)c); // Name characters
                }
                return;
            }
            case TokenType::DECREMENT:
                if (binExpr->left->type != NodeType::IDENTIFIER_EXPR) {
                    throw std::runtime_error("Left operand of decrement must be an identifier.");
                }
                {
                    Identifier* id = static_cast<Identifier*>(binExpr->left);
                    emit(Instruction::LOAD_VAR);
                    std::string name = id->name.value;
                    emit({(uint8_t)name.size()}); // Name length
                    for (char c : name) {
                        emit((uint8_t)c); // Name characters
                    }
                    emit(Instruction::LOAD_CONST);
                    int constIndex = constantList.addConstant(1);
                    emit({(uint8_t)((constIndex >> 24) & 0xFF),
                          (uint8_t)((constIndex >> 16) & 0xFF),
                          (uint8_t)((constIndex >> 8) & 0xFF),
                          (uint8_t)(constIndex & 0xFF)});
                    emit(Instruction::SUB);
                    emit(Instruction::DUP);
                    emit(Instruction::STORE_VAR);
                    emit({(uint8_t)name.size()}); // Name length
                    for (char c : name) {
                        emit((uint8_t)c); // Name characters
                    }
                }
                return;
            default:
                break;
        }

        compileExpression(binExpr->left);
        compileExpression(binExpr->right);
        switch (binExpr->op.type) {
            case TokenType::PLUS:
                emit(Instruction::ADD);
                break;
            case TokenType::MINUS:
                emit(Instruction::SUB);
                break;
            case TokenType::STAR:
                emit(Instruction::MUL);
                break;
            case TokenType::SLASH:
                emit(Instruction::DIV);
                break;
            case TokenType::LESS:
                emit(Instruction::LT);
                break;
            case TokenType::GREATER:
                emit(Instruction::GT);
                break;
            case TokenType::EQUAL_EQUAL:
                emit(Instruction::EQ);
                break;
            case TokenType::BANG_EQUAL:
                emit(Instruction::NEQ);
                break;
            case TokenType::LESS_EQUAL:
                emit(Instruction::LTE);
                break;
            case TokenType::GREATER_EQUAL:
                emit(Instruction::GTE);
                break;
            case TokenType::AND:
                emit(Instruction::AND);
                break;
            case TokenType::OR:
                emit(Instruction::OR);
                break;
            default:
                throw std::runtime_error("Unknown binary operator in compilation.");
        }
    }

    void Compiler::compileUnaryExpr(UnaryExpr* unaryExpr) {
        compileExpression(unaryExpr->operand);
        switch (unaryExpr->op.type) {
            case TokenType::BANG:
                emit(Instruction::NOT);
                break;
            case TokenType::MINUS:
                emit(Instruction::MUL); // Assuming unary minus is multiplication by -1
                emit(Instruction::LOAD_CONST);
                {
                    int constIndex = constantList.addConstant(-1);
                    emit({(uint8_t)((constIndex >> 24) & 0xFF),
                          (uint8_t)((constIndex >> 16) & 0xFF),
                          (uint8_t)((constIndex >> 8) & 0xFF),
                          (uint8_t)(constIndex & 0xFF)});
                }
                break;
            default:
                throw std::runtime_error("Unknown unary operator in compilation.");
        }
    }

    void Compiler::compileLiteralExpr(LiteralExpr* literalExpr) {
        if (literalExpr->value.type == TokenType::NUMBER) {
            bool isDouble = literalExpr->value.value.find('.') != std::string::npos;
            if (isDouble) {
                double value = std::stod(literalExpr->value.value);
                uint32_t constIndex = constantList.addConstant(value);
                emit(Instruction::LOAD_CONST);
                emit({(uint8_t)((constIndex >> 24) & 0xFF),
                      (uint8_t)((constIndex >> 16) & 0xFF),
                      (uint8_t)((constIndex >> 8) & 0xFF),
                      (uint8_t)(constIndex & 0xFF)});
            } else {
                int value = std::stoi(literalExpr->value.value);
                uint32_t constIndex = constantList.addConstant(value);
                emit(Instruction::LOAD_CONST);
                emit({(uint8_t)((constIndex >> 24) & 0xFF),
                      (uint8_t)((constIndex >> 16) & 0xFF),
                      (uint8_t)((constIndex >> 8) & 0xFF),
                      (uint8_t)(constIndex & 0xFF)});
            }
        } else if (literalExpr->value.type == TokenType::STRING) {
            std::string value = literalExpr->value.value;
            std::cout << "Adding string constant: " << value << std::endl;
            uint32_t constIndex = constantList.addConstant(value);
            emit(Instruction::LOAD_CONST);
            emit({(uint8_t)((constIndex >> 24) & 0xFF),
                  (uint8_t)((constIndex >> 16) & 0xFF),
                  (uint8_t)((constIndex >> 8) & 0xFF),
                  (uint8_t)(constIndex & 0xFF)});
        } else if (literalExpr->value.type == TokenType::BOOL) {
            bool value = literalExpr->value.value == "true";
            int constIndex = constantList.addConstant(value);
            emit(Instruction::LOAD_CONST);
            emit({(uint8_t)((constIndex >> 24) & 0xFF),
                  (uint8_t)((constIndex >> 16) & 0xFF),
                  (uint8_t)((constIndex >> 8) & 0xFF),
                  (uint8_t)(constIndex & 0xFF)});
        } else {
            throw std::runtime_error("Unknown literal type in compilation.");
        }
    }

    void Compiler::compileIdentifier(Identifier* identifier) {
        emit(Instruction::LOAD_VAR); // Load variable opcode
        std::string name = identifier->name.value;
        emit({(uint8_t)name.size()}); // Name length
        for (char c : name) {
            emit((uint8_t)c); // Name characters
        }
    }
    void Compiler::compileFuncCall(FuncCall* funcCall) {
        for (Expression* arg : funcCall->arguments) {
            compileExpression(arg);
        }
        emit(Instruction::LOAD_CONST);
        int constIndex = constantList.addConstant((int)funcCall->arguments.size());
        emit({(uint8_t)((constIndex >> 24) & 0xFF),
              (uint8_t)((constIndex >> 16) & 0xFF),
              (uint8_t)((constIndex >> 8) & 0xFF),
              (uint8_t)(constIndex & 0xFF)});
        if (funcCall->callee->type == NodeType::IDENTIFIER_EXPR) {
            Identifier* id = static_cast<Identifier*>(funcCall->callee);
            std::string funcName = id->name.value;
            uint32_t funcConstIndex = constantList.addConstant(funcName);
            emit(Instruction::LOAD_CONST);
            emit({(uint8_t)((funcConstIndex >> 24) & 0xFF),
                  (uint8_t)((funcConstIndex >> 16) & 0xFF),
                  (uint8_t)((funcConstIndex >> 8) & 0xFF),
                  (uint8_t)(funcConstIndex & 0xFF)});
        } else {
            compileExpression(funcCall->callee);
        }
        if (funcCall->callee->type == NodeType::MEMBER_EXPR || funcCall->callee->type == NodeType::SUBSCRIPT_EXPR) {
            // load temp var object
            emit(Instruction::LOAD_VAR);
            std::string tempVarName = "__temp_obj";
            emit({(uint8_t)tempVarName.size()});
            for (char c : tempVarName) {
                emit((uint8_t)c);
            }
            emit(Instruction::SWAP);
            emit(Instruction::CALL_METHOD); // Call method opcode
        } else {
            emit(Instruction::CALL); // Call function opcode
        }
    }
}