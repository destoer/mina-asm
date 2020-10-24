#pragma once

static constexpr uint32_t TOKEN_SIZE = 16;
enum class token_type
{
    imm,
    sym,
    instr,
    directive,
    str,
    reg,
    plus,
    minus,
    multiply,
    divide,
    modulus,
    left_paren,
    right_paren,
    equals,
    comma,
    eof,
};

enum class operand_type
{
    reg,
    val,
};

inline const char *token_names[TOKEN_SIZE] = 
{
    "immediate",
    "symbol",
    "instr",
    "directive",
    "str",
    "reg",
    "plus",
    "minus",
    "multiply",
    "divide",
    "modulus",
    "left_paren",
    "right_paren",
    "equals",
    "comma",
    "eof"
};


struct Token
{
    Token() {}

    Token(token_type t) : type(t), literal("")
    {

    }

    Token(std::string l, token_type t) : type(t), literal(l)
    {

    }

    token_type type;
    std::string literal;
};