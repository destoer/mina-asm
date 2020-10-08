#include <headers/lib.h>

//https://github.com/ladystarbreeze/mina-isa/blob/main/MINA_Instruction_Set_Architecture.md
/*
S-type: [group(4)][opcode(4)][src1(4)][src2 (4)][dest(4)][ SBZ  (4)][ SBZ  (8)]
I-type: [group(4)][opcode(4)][src1(4)][shift(4)][dest(4)][ imm  (4)][ imm  (8)]
M-type: [group(4)][opcode(4)][SBZ (4)][ imm (4)][dest(4)][ imm  (4)][ imm  (8)]
F-type: [group(4)][opcode(4)][src1(4)][src2 (4)][dest(4)][rshift(4)][ SBZ  (8)] // only for funnel shifts
B-type: [group(4)][opcode(4)][              offset             (16)][offset(8)]
*/

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
    Instr(int g, int o, instr_type t) : type(t), group(g),opcode(o)
    {

    }

    instr_type type;

    int group;
    int opcode;

    
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
    Assembler(std::string filename);
    
    void assemble_file();

    
private:
    std::vector<Token> parse_tokens(std::string instr);
    uint32_t assemble_opcode(std::string instr);

    std::string file;

    const std::unordered_map<std::string, Instr> instr_table = 
    {
        {"mov",Instr(0b0101,0b1000,instr_type::S)}
    };

    const std::unordered_map<std::string, Directive> directive_table;

    const std::unordered_map<std::string, uint32_t> register_table = 
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
        {"r14", 14}
    };


    std::unordered_map<std::string, Symbol> symbol_table;
};


Assembler::Assembler(std::string filename)
{
    file = read_file(filename);

    std::cout << "file: " << file << "\n";

    if(file == "")
    {
        printf("file %s could not be read!\n",filename.c_str());
        exit(1);
    }

}

void Assembler::assemble_file()
{
    // this should do a 1st pass and resolve all labels
    // but for now lets just get a straight opcode assembled
    std::stringstream file_stream;

    file_stream << file;

    std::string line;
    while(getline(file_stream,line))
    {
        printf("op: %s = %08x\n",line.c_str(),assemble_opcode(line));
    }
}


std::vector<Token> Assembler::parse_tokens(std::string instr)
{
    std::vector<Token> tokens;

    // ok now we need to parse out the line 
    size_t i = 0;
    const auto len = instr.size();
    while(i < len)
    {
        char c = instr[i++];

        // check first char of token
        switch(c)
        {
            // ; is a comment that means we are done for this line
            case ';':
            {
                return tokens;
            }

            // effectively whitespace dont care
            case ' ':
            case '\t':
            {
                break;
            }

            case ',':
            {
                // we want to reject this if we havent found any tokens yet
                if(tokens.size() == 0)
                {
                    printf("unexpected char ',' in line: %s\n",instr.c_str());
                }
                break;
            }

            default:
            {
                // start of immediate
                if(isdigit(c))
                {
                    // if it begins with a zero
                    // check the next char incase the literal is in hex
                }

                else if(isalpha(c))
                {
                    std::string literal(1,c);

                    //while(i < len && c != ' ' && c != ',')
                    for(; i < len && instr[i] != ' ' && instr[i] != ','; i++)
                    {
                        literal += instr[i];
                    }

                    //printf("found literal %s\n",literal.c_str());

                    // ok we have a literal now lets figure out what it is
                    // first check if it is a instr
                    // then if it is a directive
                    // then a register
                    // else a symbol
                    // otherwhise it is unrecognized

                    if(instr_table.count(literal))
                    {
                        tokens.push_back(Token(literal,token_type::instr));
                    }

                    else if(directive_table.count(literal))
                    {
                        tokens.push_back(Token(literal,token_type::directive));
                    }

                    else if(register_table.count(literal))
                    {
                        tokens.push_back(Token(literal,token_type::reg));
                    }

                    else if(symbol_table.count(literal))
                    {
                        tokens.push_back(Token(literal,token_type::sym));
                    }
                    
                    else
                    {
                        printf("unrecognised symbol: %s\n",literal.c_str());
                        exit(1);
                    }

                }

                else // something has gone wrong
                {
                    printf("illegal token: %c!", c);
                    exit(1);
                }


                break;
            }
        }
    }   

    return tokens; 
}

// debug dump tokens
void dump_token_debug(std::vector<Token> tokens)
{
    for(const auto &t : tokens)
    {
        switch(t.type)
        {

            case token_type::imm:
            {
                printf("immediate: %s\n",t.literal.c_str());
                break;
            }

            case token_type::sym:
            {
                printf("symbol: %s\n",t.literal.c_str());
                break;
            }

            case token_type::instr:
            {
                printf("instruction: %s\n",t.literal.c_str());
                break;
            }

            case token_type::directive:
            {
                printf("directive: %s\n",t.literal.c_str());
                break;
            }

            case token_type::reg:
            {
                printf("register: %s\n",t.literal.c_str());
                break;
            }
                        
        }
    }
}

uint32_t Assembler::assemble_opcode(std::string instr)
{
    const auto tokens = parse_tokens(instr);

    
    //dump_token_debug(tokens);

    // for now assume token is in instr in pratcice
    // we need a parse line function that dispatches on the first token type
    // and asserts we actually got any

    // ok pick up switching on the instr type tomorrow
    //switch(instr_type)

    return 0xfff;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("usage: %s <file to assemble>\n",argv[1]);
        return 1;
    }

    Assembler assembler(argv[1]);

    assembler.assemble_file();

}