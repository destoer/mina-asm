#pragma once
#include <headers/lib.h>

static constexpr uint32_t SRC1_OFFSET = 20;
static constexpr uint32_t SRC2_OFFSET = 16;
static constexpr uint32_t DST_OFFSET = 12;


enum class instr_group
{
arith = 0b0000,
logic = 0b0001,
cmp = 0b0010,
reg_branch = 0b0011,
mem = 0b0100,
mov = 0b0101,
shift = 0b0110,
cnt = 0b0111,
rel_branch = 0b1000,

};

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
    Instr(instr_group g, uint32_t o,uint32_t c, instr_type t) : 
        type(t), group(g), opcode(o), operand_count(c)
    {

    }

    instr_type type;

    instr_group group;
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
int32_t convert_imm(const std::string &imm);

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

        // register - immediate
        {"addi",Instr(instr_group::arith,0b0000,3,instr_type::I)},
        {"multi",Instr(instr_group::arith,0b0001,3,instr_type::I)},
        {"divi",Instr(instr_group::arith,0b0010,3,instr_type::I)},
        {"remi",Instr(instr_group::arith,0b0011,3,instr_type::I)},
        {"slti",Instr(instr_group::arith,0b0100,3,instr_type::I)},
        {"sltiu",Instr(instr_group::arith,0b0101,3,instr_type::I)},
        {"nop",Instr(instr_group::arith,0b0110,0,instr_type::I)},
        {"pcaddi",Instr(instr_group::arith,0b0111,2,instr_type::I)},

        // register - register
        {"add",Instr(instr_group::arith,0b1000,3,instr_type::S)},
        {"mult",Instr(instr_group::arith,0b1001,3,instr_type::S)},
        {"div",Instr(instr_group::arith,0b1010,3,instr_type::S)},
        {"rem",Instr(instr_group::arith,0b1011,3,instr_type::S)},
        {"slt",Instr(instr_group::arith,0b1100,3,instr_type::S)},
        {"sltu",Instr(instr_group::arith,0b1101,3,instr_type::S)},
        {"pcadd",Instr(instr_group::arith,0b1111,2,instr_type::S)},


        // Logical instrucitons 
        // group 0b0001
        
        // register - immediate
        {"andi",Instr(instr_group::logic,0b0000,3,instr_type::I)},
        {"ori",Instr(instr_group::logic,0b0001,3,instr_type::I)},
        {"xori",Instr(instr_group::logic,0b0010,3,instr_type::I)},
        {"nandi",Instr(instr_group::logic,0b0011,3,instr_type::I)},

        // register - register
        {"and",Instr(instr_group::logic,0b1000,3,instr_type::S)},
        {"or",Instr(instr_group::logic,0b1001,3,instr_type::S)},
        {"xor",Instr(instr_group::logic,0b1010,3,instr_type::S)},
        {"nand",Instr(instr_group::logic,0b1011,3,instr_type::S)},
        {"popcnt",Instr(instr_group::logic,0b1100,2,instr_type::S)},
        {"clo",Instr(instr_group::logic,0b1101,2,instr_type::S)},
        {"plo",Instr(instr_group::logic,0b1110,2,instr_type::S)},


        // cmp (group 0b0010)

        // register - imm
        {"cmpi/eq",Instr(instr_group::cmp,0b0000,2,instr_type::I)},
        {"cmpi/lo",Instr(instr_group::cmp,0b0001,2,instr_type::I)},
        {"cmpi/ls",Instr(instr_group::cmp,0b0010,2,instr_type::I)},
        {"cmpi/lt",Instr(instr_group::cmp,0b0011,2,instr_type::I)},
        {"cmpi/le",Instr(instr_group::cmp,0b0100,2,instr_type::I)},


        // register - register
        {"cmp/eq",Instr(instr_group::cmp,0b1000,2,instr_type::S)},
        {"cmp/lo",Instr(instr_group::cmp,0b1001,2,instr_type::S)},
        {"cmp/ls",Instr(instr_group::cmp,0b1010,2,instr_type::S)},
        {"cmp/lt",Instr(instr_group::cmp,0b1011,2,instr_type::S)},
        {"cmp/le",Instr(instr_group::cmp,0b1100,2,instr_type::S)},


        // register branch instrs

        // register - immediate

        // there is an implict shift by 2 added for these opcodes 
        // as well as the load ones (shift by 1 aswell)
        // what is the best way to handle this without just adding
        // a specialised handler in the form of a fptr to the instr?
        // the only other option is to add a table for loads and branches
        // that contains an implicit shift ammount
        // blank data to encode some "random" integer param which we will implicitly use for a shift
        // in the reg branch group...
        {"RBRA",Instr(instr_group::reg_branch,0b0000,2,instr_type::I)},


        // register branch

        // MOV (group 0b0101)

        // mov register - register
        {"mov",Instr(instr_group::mov,0b1000,2,instr_type::S)},
        {"mt",Instr(instr_group::mov,0b1001,2,instr_type::S)},
        {"mf",Instr(instr_group::mov,0b1010,2,instr_type::S)},
        {"mtoc",Instr(instr_group::mov,0b1011,1,instr_type::S)},
        {"mfrc",Instr(instr_group::mov,0b1100,1,instr_type::S)},
        {"mtou",Instr(instr_group::mov,0b1101,2,instr_type::S)},
        {"mfou",Instr(instr_group::mov,0b1110,2,instr_type::S)},

        // SHIFT (group 0b0110)

        // shift register - register
        {"rlsl",Instr(instr_group::shift,0b1000,3,instr_type::S)},
        {"rlsr",Instr(instr_group::shift,0b1001,3,instr_type::S)},
        {"rasr",Instr(instr_group::shift,0b1010,3,instr_type::S)},
        {"rror",Instr(instr_group::shift,0b1011,3,instr_type::S)},

        // branch pc reliatve (group 1000)
        {"bra",Instr(instr_group::rel_branch,0b0000,1,instr_type::B)}
    };


    std::unordered_map<std::string, Symbol> symbol_table;

    uint32_t offset;
};
