#pragma once
#include <headers/lib.h>

static constexpr uint32_t SRC1_OFFSET = 20;
static constexpr uint32_t SRC2_OFFSET = 16;
static constexpr uint32_t DST_OFFSET = 12;

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
    Symbol() {}
    Symbol(uint32_t v) : value(v)
    {
        
    }

    uint32_t value;
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

void dump_token_debug(std::vector<Token> tokens);
bool verify_immediate(const std::string &instr, std::string &literal);
bool encode_i_type_operand(int32_t &v, uint32_t &s);

class Assembler
{
public:
    Assembler() {};

    Assembler(const std::string &filename);
    void init(const std::string &filename);
    
    void assemble_file();
    uint32_t assemble_opcode(const std::string &instr,const std::vector<Token> &tokens);

    void write_binary(const std::string &filename);
    std::vector<Token> parse_tokens(const std::string &instr);

    // where we are actually dumping the assembled data
    std::vector<uint8_t> output;

    bool instr_exists(const std::string &instr) const
    {
        return instr_table.count(instr);
    }

    Instr get_instr_entry(const std::string &instr)
    {
        return instr_table[instr];
    }

private:
    void assemble_line(const std::string &line);

    void first_pass();

    void dump_symbol_table_debug();

    void decode_imm(std::string instr, size_t &i,std::vector<Token> &tokens);
    uint32_t decode_s_instr(const Instr &instr_entry,const std::string &instr,const std::vector<Token> &tokens);
    uint32_t decode_b_instr(const Instr &instr_entry,const std::string &instr,const std::vector<Token> &tokens);
    uint32_t decode_i_instr(const Instr &instr_entry,const std::string &instr,const std::vector<Token> &tokens);

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
        // Arithmetic instrs (group 0000)
        {"addi",Instr(0b0000,0b0000,3,instr_type::I)},


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

        // branch pc reliatve (group 1000)
        {"bra",Instr(0b1000,0b0000,1,instr_type::B)}
    };


    std::unordered_map<std::string, Symbol> symbol_table;

    uint32_t offset;
};
