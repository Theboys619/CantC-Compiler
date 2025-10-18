#pragma once

#include "common.h"

namespace CantC {
    enum class Instruction : uint8_t {
        NOP, HALT,

        LOAD_CONST, LOAD_VAR, STORE_VAR, LOAD_FIELD, STORE_FIELD,
        NEW,
        
        ADD, SUB, MUL, DIV,
        AND, OR, NOT,
        LT, GT, EQ, NEQ, LTE, GTE,

        POP, SWAP, DUP,

        JMP, JZ, JNZ,
        CALL, CALL_METHOD, RET,
    };
}