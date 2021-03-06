#pragma once
#include <headers/lib.h>
#include <mina/token.h>
#include <mina/parser.h>

static constexpr uint32_t SRC1_OFFSET = 20;
static constexpr uint32_t SRC2_OFFSET = 16;
static constexpr uint32_t DST_OFFSET = 12;

// forward declare so we can take pointers on it
class Assembler;

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

// i think cause theres gonna be very little in common between these
// its best just have a function pointer to some handler for them
using DIRECTIVE_FPTR = void (Assembler::*)(const std::vector<Token> &tokens, uint32_t pass);

struct Directive
{
    Directive() {}

    Directive(DIRECTIVE_FPTR c) : callback(c)
    {

    }

    const DIRECTIVE_FPTR callback = nullptr;

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
    uint32_t assemble_opcode(const std::vector<Token> &tokens);

    void write_binary(const std::string &filename);
    std::vector<Token> parse_tokens(const std::string &instr);

    // sum expr off ast (should be on the left of a node)
    // (unless its at the root of course)
    int32_t sum(AstNode *node);

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
    void die(const char *format, ...);

    void assemble_line(const std::string &line);

    void first_pass();

    void dump_symbol_table_debug();

    uint32_t read_int_operand(const Token &token);
    uint32_t handle_i_implicit_shift(uint32_t v,uint32_t shift);

    int32_t read_op(AstNode *&root,operand_type type);
    void decode_imm(std::string instr, size_t &i,std::vector<Token> &tokens);
    uint32_t decode_s_instr(const Instr &instr_entry,const std::vector<Token> &tokens);
    uint32_t decode_b_instr(const Instr &instr_entry,const std::vector<Token> &tokens);
    uint32_t decode_i_instr(const Instr &instr_entry,const std::vector<Token> &tokens);
    uint32_t decode_m_instr(const Instr &instr_entry,const std::vector<Token> &tokens);
    uint32_t decode_f_instr(const Instr &instr_entry,const std::vector<Token> &tokens);


    // directives
    void define_string(const std::vector<Token> &tokens, uint32_t pass);
    void include_text_file(const std::vector<Token> &tokens, uint32_t pass);
    void include_binary_file(const std::vector<Token> &tokens, uint32_t pass);
    void equ(const std::vector<Token> &tokens, uint32_t pass);
    void dw(const std::vector<Token> &tokens, uint32_t pass);

    std::vector<std::string> file_lines;

    std::unordered_map<std::string, Directive> directive_table = 
    {
        {"string",&Assembler::define_string},
        {"include",&Assembler::include_text_file},
        {"file",&Assembler::include_binary_file},
        {"equ",&Assembler::equ},
        {"dw", &Assembler::dw}
    };

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


    static constexpr uint32_t reg_branch_shift[0x10] = 
    {
        2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };

    static constexpr uint32_t mem_shift[0x10] =
    {
        2,1,0,2,1,0,2,2,0,0,0,0,0,0,0,0
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

        {"rbra",Instr(instr_group::reg_branch,0b0000,2,instr_type::I)},
        {"rcall",Instr(instr_group::reg_branch,0b0001,2,instr_type::I)},
        {"ret",Instr(instr_group::reg_branch,0b0010,0,instr_type::I)},

        // register - register

        {"robra",Instr(instr_group::reg_branch,0b1000,2,instr_type::S)},
        {"rocall",Instr(instr_group::reg_branch,0b1001,2,instr_type::S)},


        // memory group

        // register - immediate
        {"ld",Instr(instr_group::mem,0b0000,3,instr_type::I)},
        {"ldh",Instr(instr_group::mem,0b0001,3,instr_type::I)},
        {"ldb",Instr(instr_group::mem,0b0010,3,instr_type::I)},
        {"st",Instr(instr_group::mem,0b0011,3,instr_type::I)},
        {"sth",Instr(instr_group::mem,0b0100,3,instr_type::I)},
        {"stb",Instr(instr_group::mem,0b0101,3,instr_type::I)},
        {"ldc",Instr(instr_group::mem,0b0110,2,instr_type::I)},
        {"stc",Instr(instr_group::mem,0b0111,2,instr_type::I)},

        // register - register
        {"rld",Instr(instr_group::mem,0b1000,3,instr_type::S)},
        {"rldh",Instr(instr_group::mem,0b1001,3,instr_type::S)},
        {"rldb",Instr(instr_group::mem,0b1010,3,instr_type::S)},
        {"rst",Instr(instr_group::mem,0b1011,3,instr_type::S)},
        {"rsth",Instr(instr_group::mem,0b1100,3,instr_type::S)},
        {"rstb",Instr(instr_group::mem,0b1101,3,instr_type::S)},
        {"pop",Instr(instr_group::mem,0b1110,1,instr_type::S)},
        {"push",Instr(instr_group::mem,0b1111,1,instr_type::S)},        

         // MOV (group 0b0101)

        // mov register - immediate
        {"movi",Instr(instr_group::mov,0b0000,2,instr_type::I)},
        {"mti",Instr(instr_group::mov,0b0001,2,instr_type::I)},        
        {"mfi",Instr(instr_group::mov,0b0010,2,instr_type::I)},

        // mov register - special
        {"movl",Instr(instr_group::mov,0b0011,2,instr_type::M)},
        {"movu",Instr(instr_group::mov,0b0100,2,instr_type::M)},

        // mov register - register
        {"mov",Instr(instr_group::mov,0b1000,2,instr_type::S)},
        {"mt",Instr(instr_group::mov,0b1001,2,instr_type::S)},
        {"mf",Instr(instr_group::mov,0b1010,2,instr_type::S)},
        {"mtoc",Instr(instr_group::mov,0b1011,1,instr_type::S)},
        {"mfrc",Instr(instr_group::mov,0b1100,1,instr_type::S)},
        {"mtou",Instr(instr_group::mov,0b1101,2,instr_type::S)},
        {"mfou",Instr(instr_group::mov,0b1110,2,instr_type::S)},

        // SHIFT (group 0b0110)

        // register - immediate
        {"lsl",Instr(instr_group::shift,0b0000,3,instr_type::I)},
        {"lsr",Instr(instr_group::shift,0b0001,3,instr_type::I)},
        {"asr",Instr(instr_group::shift,0b0010,3,instr_type::I)},
        {"ror",Instr(instr_group::shift,0b0011,3,instr_type::I)},        

        // shift register - register
        {"rlsl",Instr(instr_group::shift,0b1000,3,instr_type::S)},
        {"rlsr",Instr(instr_group::shift,0b1001,3,instr_type::S)},
        {"rasr",Instr(instr_group::shift,0b1010,3,instr_type::S)},
        {"rror",Instr(instr_group::shift,0b1011,3,instr_type::S)},

        // special
        {"flsl",Instr(instr_group::shift,0b1100,4,instr_type::F)},
        {"flsr",Instr(instr_group::shift,0b1101,4,instr_type::F)},        

        // control group

        // register - immedaite

        {"stop",Instr(instr_group::cnt,0b0000,0,instr_type::I)},
        {"wfi",Instr(instr_group::cnt,0b0001,0,instr_type::I)},
        {"sett",Instr(instr_group::cnt,0b0010,0,instr_type::I)},
        {"clrt",Instr(instr_group::cnt,0b0011,0,instr_type::I)},
        {"switch",Instr(instr_group::cnt,0b0100,0,instr_type::I)},

        // register - register
        {"svcall",Instr(instr_group::cnt,0b1000,1,instr_type::S)},
        {"fault",Instr(instr_group::cnt,0b1001,2,instr_type::S)},
        {"mtof",Instr(instr_group::cnt,0b1010,1,instr_type::S)},
        {"mfrf",Instr(instr_group::cnt,0b1011,1,instr_type::S)},

        // branch pc reliatve (group 1000)

        // rel branch
        {"bra",Instr(instr_group::rel_branch,0b0000,1,instr_type::B)},
        {"bt",Instr(instr_group::rel_branch,0b0001,1,instr_type::B)},
        {"bf",Instr(instr_group::rel_branch,0b0010,1,instr_type::B)},

        // rel call
        {"call",Instr(instr_group::rel_branch,0b1000,1,instr_type::B)},
        {"ct",Instr(instr_group::rel_branch,0b1001,1,instr_type::B)},
        {"cf",Instr(instr_group::rel_branch,0b1010,1,instr_type::B)}
    };


    std::unordered_map<std::string, Symbol> symbol_table;

    // need to record size of directives that write to output
    // and are external to the source file
    std::unordered_map<uint32_t,int32_t> directive_size;

    uint32_t offset;
    uint32_t line;
};
