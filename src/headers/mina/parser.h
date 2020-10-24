#pragma once
#include <headers/lib.h>
#include <mina/token.h>


struct AstNode
{
    AstNode() {}

    AstNode(AstNode *l, Token d, AstNode *r) : data(d), left(l), right(r)
    {

    }

    Token data;
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};

// todo improve error reporting on this
// as we currently dont have access to line information...
struct Ast
{
    using LED_FPTR = AstNode* (Ast::*)(Token &t, AstNode *left);

    Ast(const std::vector<Token> &t);
    ~Ast();


    AstNode *line();

    // helpers
    void print(AstNode *node);
    Token next();
    Token peek();
    void skip(token_type type);
    void delete_ast(AstNode *root);





    // pratt parser
    int32_t lbp(const Token &t);
    AstNode *expression(int32_t rbp);
    AstNode *nud(Token &t);
    AstNode *led(Token &t,AstNode *left);
    AstNode *led_binary_op(Token &t,AstNode *left);
    AstNode *led_comma(Token &t,AstNode *left);


    // really would rather use designated initializers for this...
    int32_t lbp_table[TOKEN_SIZE] = 
    {
        0, // imm
        0, // sym
        -1, // instr
        -1, // directive
        -1, // str
        -1, // reg
        12, // plus
        12, // minus
        13, // multiply
        13, // divide
        13, // modulus
        -1, // left_paren
        0, // right_paren
        2, // equals
        0, // comma
        0, // eof
    };


    static constexpr LED_FPTR led_table[TOKEN_SIZE] = 
    {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        &Ast::led_binary_op,
        &Ast::led_binary_op,
        &Ast::led_binary_op,
        &Ast::led_binary_op,
        &Ast::led_binary_op,
        nullptr,
        nullptr,
        &Ast::led_binary_op,
        nullptr,
        nullptr
    };

    AstNode *root;
    std::vector<Token> tokens;
    Token token;
    size_t tok_idx;
    int32_t brace_count;
    size_t operands;
};