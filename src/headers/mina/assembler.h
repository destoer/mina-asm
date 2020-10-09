#pragma once
#include <headers/lib.h>

enum class instr_type
{
    S,
    I,
    M,
    F,
    B
};

// this will give us the info we need on our instr
// for now we will just try make this generic but may have to
// go down the path of using function pointers down the line
// i.e when we want mov to auto decode operands
struct Instr
{
    Instr() {}
    Instr(int g, int o,int c, instr_type t) : 
        type(t), group(g), opcode(o), operand_count(c)
    {

    }

    instr_type type;

    uint32_t group;
    uint32_t opcode;
    uint32_t operand_count;
    
};

struct Symbol
{

};

struct Directive
{

};


enum class token_type
{
    imm,
    sym,
    instr,
    directive,
    reg
};

struct Token
{
    Token(std::string l, token_type t) : type(t), literal(l)
    {

    }

    token_type type;
    std::string literal;
};



class Assembler
{
public:
    Assembler() {};

    Assembler(std::string filename);
    void init(std::string filename);
    
    void assemble_file();
    uint32_t assemble_opcode(std::string instr,std::vector<Token> tokens);

    void write_binary(std::string filename);
    std::vector<Token> parse_tokens(std::string instr);

    // where we are actually dumping the assembled data
    std::vector<uint8_t> output;

    bool instr_exists(std::string instr) const
    {
        return instr_table.count(instr);
    }

    Instr get_instr_entry(std::string instr)
    {
        return instr_table[instr];
    }

private:
    void assemble_line(std::string line);

    void first_pass();

    std::string file = "";

    const std::unordered_map<std::string, Directive> directive_table;

    std::unordered_map<std::string, uint32_t> register_table = 
    {
        {"r0", 0},
        {"r1", 1},
        {"r2", 2},
        {"r3", 3},
        {"r4", 4},
        {"r5", 5},
        {"r6", 6},
        {"r7", 7},
        {"r8", 8},
        {"r9", 9},
        {"r10", 10},
        {"r11", 11},
        {"r12", 12},
        {"r13", 13},
        {"r14", 14},
        {"r15", 15},
        {"sp", 15}
    };

    // is there any way to get this to be const?
    std::unordered_map<std::string, Instr> instr_table = 
    {
        // MOV (group 0b0101)

        // mov register - register
        {"mov",Instr(0b0101,0b1000,2,instr_type::S)},
        {"mt",Instr(0b0101,0b1001,2,instr_type::S)},
        {"mf",Instr(0b0101,0b1010,2,instr_type::S)},
        {"mtoc",Instr(0b0101,0b1011,1,instr_type::S)},
        {"mfrc",Instr(0b0101,0b1100,1,instr_type::S)},
        {"mtou",Instr(0b0101,0b1101,2,instr_type::S)},
        {"mfou",Instr(0b0101,0b1110,2,instr_type::S)},

        // SHIFT (group 0b0110)

        // shift register - register
        {"rlsl",Instr(0b0110,0b1000,3,instr_type::S)},
        {"rlsr",Instr(0b0110,0b1001,3,instr_type::S)},
        {"rasr",Instr(0b0110,0b1010,3,instr_type::S)},
        {"rror",Instr(0b0110,0b1011,3,instr_type::S)},
    };


    std::unordered_map<std::string, Symbol> symbol_table;

    uint32_t offset;
};
