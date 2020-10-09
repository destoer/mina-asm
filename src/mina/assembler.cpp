#include <mina/assembler.h>

//https://github.com/ladystarbreeze/mina-isa/blob/main/MINA_Instruction_Set_Architecture.md
/*
S-type: [group(4)][opcode(4)][src1(4)][src2 (4)][dest(4)][ SBZ  (4)][ SBZ  (8)]
I-type: [group(4)][opcode(4)][src1(4)][shift(4)][dest(4)][ imm  (4)][ imm  (8)]
M-type: [group(4)][opcode(4)][SBZ (4)][ imm (4)][dest(4)][ imm  (4)][ imm  (8)]
F-type: [group(4)][opcode(4)][src1(4)][src2 (4)][dest(4)][rshift(4)][ SBZ  (8)] // only for funnel shifts
B-type: [group(4)][opcode(4)][              offset             (16)][offset(8)]
*/



Assembler::Assembler(std::string filename)
{
    init(filename);
}

void Assembler::init(std::string filename)
{
    file = read_file(filename);

    std::cout << "file: " << file << "\n";

    if(file == "")
    {
        printf("file %s could not be read!\n",filename.c_str());
        exit(1);
    }

    offset = 0;    
}

// first assembler pass resolve all symbols and figure out required size of the binary
void Assembler::first_pass()
{

    offset = 0;

    std::stringstream file_stream;

    file_stream << file;

    std::string line;
    while(getline(file_stream,line))
    {
        const auto tokens = parse_tokens(line);

        // nothing to do
        if(tokens.size() == 0)
        {
            continue;
        }

        switch(tokens[0].type)
        {
            case token_type::instr:
            {
                offset += 4; // fixed size instr all are 4 bytes
                break;
            }

            default:
            {
                puts("first parse unhandled token");
                exit(1);
            }
        }        
    }

    output.resize(offset);
}

void Assembler::assemble_file()
{
    if(file == "")
    {
        printf("assembler: no file loaded");
        return;
    }

    first_pass();

    offset = 0;

    std::stringstream file_stream;

    file_stream << file;

    std::string line;
    while(getline(file_stream,line))
    {
        assemble_line(line);
    }
}

void Assembler::write_binary(std::string filename)
{
    write_file(filename,output);
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
                    puts("immediate unhandled");
                }

                else if(isalpha(c))
                {
                    std::string literal(1,c);

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

void Assembler::assemble_line(std::string instr)
{
    const auto tokens = parse_tokens(instr);

    // nothing to do
    if(tokens.size() == 0)
    {
        return;
    }

    else
    {
        switch(tokens[0].type)
        {
            case token_type::instr:
            {
                const auto opcode = assemble_opcode(instr,tokens);

                write_vec(output,offset,opcode);

                offset += 4; // instr size of 4

                printf("op: %s = %08x\n",instr.c_str(),opcode);
                break;
            }

            default:
            {
                printf("unknown token start %d\n",static_cast<int>(tokens[0].type));
                exit(1);
            }
        }
    }
}

uint32_t Assembler::assemble_opcode(std::string instr,std::vector<Token> tokens)
{
    
    //dump_token_debug(tokens);
   
    const auto instr_entry = instr_table[tokens[0].literal];

    // every opcode has the group and opcode field
    uint32_t opcode = (instr_entry.group << 28) | (instr_entry.opcode << 24);

    switch(instr_entry.type)
    {
        default:
        {
            case instr_type::S: // register to register opcodes
            {
                // verify we actually have enough operands to assemble this
                if(instr_entry.operand_count != tokens.size()-1)
                {
                    printf("[S-type-instr] expected %d operands got %zd (%s)\n",
                        instr_entry.operand_count,tokens.size()-1,instr.c_str());
                    exit(1);
                }

                // verfiy all operands are registers
                for(size_t i = 1; i < instr_entry.operand_count; i++)
                {
                    if(tokens[i].type != token_type::reg)
                    {
                        printf("[S-type-instr] expected register operand");
                        exit(1);
                    }
                }


                switch(instr_entry.operand_count)
                {
                    case 1: // dest = op1, everything else zero
                    {
                        opcode |= register_table[tokens[1].literal] << 12;
                        break;
                    }


                    case 2: // src2 empty, dest = op1, src1 = op2
                    {
                        opcode |= (register_table[tokens[1].literal] << 12) | 
                            (register_table[tokens[2].literal] << 20);
                        break;
                    }

                    case 3: // dest = op1, src1 = op2, src2 = op3
                    {
                        opcode |= (register_table[tokens[1].literal] << 12) | 
                            (register_table[tokens[2].literal] << 20) |
                            (register_table[tokens[3].literal] << 16);                        
                        break;
                    }

                    default: 
                    {
                        printf("S type unhandled operand count %d\n",instr_entry.operand_count);
                        break;
                    }
                }
                break;
            }

            break;
        }
    }

    return opcode;
}
