#include <mina/assembler.h>

//https://github.com/ladystarbreeze/mina-isa/blob/main/MINA_Instruction_Set_Architecture.md
/*
S-type: [group(4)][opcode(4)][src1(4)][src2 (4)][dest(4)][ SBZ  (4)][ SBZ  (8)]
I-type: [group(4)][opcode(4)][src1(4)][shift(4)][dest(4)][ imm  (4)][ imm  (8)]
M-type: [group(4)][opcode(4)][SBZ (4)][ imm (4)][dest(4)][ imm  (4)][ imm  (8)]
F-type: [group(4)][opcode(4)][src1(4)][src2 (4)][dest(4)][rshift(4)][ SBZ  (8)] // only for funnel shifts
B-type: [group(4)][opcode(4)][              offset             (16)][offset(8)]
*/



Assembler::Assembler(const std::string &filename)
{
    init(filename);
}

void Assembler::init(const std::string &filename)
{
    file = read_file(filename);

    std::cout << "file: " << "\n" << file << "\n\n";

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

        auto token = tokens[0];

        switch(token.type)
        {
            case token_type::instr:
            {
                offset += 4; // fixed size instr all are 4 bytes
                break;
            }

            // potential label definiton
            case token_type::sym:
            {
                // search for a ':'
                // if we dont find it then it is invalid
                const auto idx = token.literal.find(':');
                if(idx == std::string::npos)
                {
                    printf("unknown instr: %s(%s)\n",token.literal.c_str(),line.c_str());
                    exit(1);
                }


                // valid label add it to the symbol table if not allready defined
                if(symbol_table.count(token.literal))
                {
                    printf("redefinition of symbol: %s\n",token.literal.c_str());
                    exit(1);
                }

                token.literal = token.literal.substr(0,idx); 

                symbol_table[token.literal] = Symbol(offset);

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

void Assembler::write_binary(const std::string &filename)
{
    write_file(filename,output);
}

template<typename F>
bool verify_immediate_internal(const std::string &instr, size_t &i, F lambda)
{
    const auto len = instr.size();

    for(; i < len; i++)
    {
        // token terminated
        if(instr[i] == ',' || instr[i] == ' ')
        {
            break;
        }

        if(!lambda(instr[i]))
        {
            return false;
        }
    }

    return true;
}


bool verify_immediate(const std::string &instr, std::string &literal)
{
    const auto len = instr.size();

    // an empty immediate aint much use to us
    if(!len)
    {
        return false;
    }

    size_t i = 0;

    const auto c = instr[0];

    // allow - or +
    if(c == '-' || c == '+')
    {
        i = 1;
        // no digit after the sign is of no use
        if(len == 1)
        {
            return false;
        }
    }

    bool valid = false;


    // have prefix + one more digit at minimum
    const auto prefix = i+2 < len?  instr.substr(i,2) : "";

    // verify we have a valid hex number
    if(prefix == "0x")
    {
        // skip past the prefix
        i += 2;
        valid = verify_immediate_internal(instr,i,[](const char c) 
        {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
        });
    }

    // verify its ones or zeros
    else if(prefix == "0b")
    {
        // skip past the prefix
        i += 2;                
        valid = verify_immediate_internal(instr,i,[](const char c) 
        {
            return c == '0' || c == '1';
        });
    }

    // verify we have all digits
    else
    {
        valid = verify_immediate_internal(instr,i,[](const char c) 
        {
            return c >= '0' && c <= '9';
        });
    }
    

    if(valid)
    {
        literal = instr.substr(0,i);
    }

    return valid;    
}

void Assembler::decode_imm(std::string instr, size_t &i,std::vector<Token> &tokens)
{
    std::string literal = "";

    i -= 1;
    const auto success = verify_immediate(instr.substr(i),literal);

    if(!success)
    {
        printf("invalid immediate: %s\n",instr.c_str());
        exit(1);
    }

    i += literal.size();

    tokens.push_back(Token(literal,token_type::imm));    
}

std::vector<Token> Assembler::parse_tokens(const std::string &instr)
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

            // specify sign on immediate
            case '+':
            case '-':
            {
                if(i < len)
                {
                    c = instr[i];
                    if(isdigit(c))
                    {
                        decode_imm(instr,i,tokens);
                    }

                    else
                    {
                        printf("expected char '-' in line: %s\n",instr.c_str());
                    }
                }

                else
                {
                    printf("unexpected char '-' at end of line: %s\n",instr.c_str());
                }
                break;
            }

            default:
            {
                // start of immediate
                if(isdigit(c))
                {
                    decode_imm(instr,i,tokens);
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

                    else
                    {
                        tokens.push_back(Token(literal,token_type::sym));
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

void Assembler::dump_symbol_table_debug()
{
    puts("symbol table:\n");

    for(const auto &s : symbol_table)
    {
        printf("%s = %x\n",s.first.c_str(),s.second.value);
    }
}

void Assembler::assemble_line(const std::string &instr)
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

                printf("%s = %08x\n",instr.c_str(),opcode);
                break;
            }

            // label start (dont care)
            case token_type::sym:
            {
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

uint32_t Assembler::decode_s_instr(const Instr &instr_entry,const std::string &instr,const std::vector<Token> &tokens)
{
    uint32_t opcode = 0;

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
            opcode |= register_table[tokens[1].literal] << DST_OFFSET;
            break;
        }


        case 2: // src2 empty, dest = op1, src1 = op2
        {
            opcode |= (register_table[tokens[1].literal] << DST_OFFSET) | 
                (register_table[tokens[2].literal] << SRC1_OFFSET);
            break;
        }

        case 3: // dest = op1, src1 = op2, src2 = op3
        {
            opcode |= (register_table[tokens[1].literal] << DST_OFFSET) | 
                (register_table[tokens[2].literal] << SRC1_OFFSET) |
                (register_table[tokens[3].literal] << SRC2_OFFSET);                        
            break;
        }

        default: 
        {
            printf("S type unhandled operand count %d(%s)\n",instr_entry.operand_count,instr.c_str());
            break;
        }
    }    

    return opcode;
}

uint32_t Assembler::decode_b_instr(const Instr &instr_entry,const std::string &instr,const std::vector<Token> &tokens)
{   
    UNUSED(instr_entry);

    uint32_t opcode = 0;

    // we should only have one operand for a branch
    if(tokens.size() -1 != 1)
    {
        printf("branch: expected 1 operand but got: %zd\n",tokens.size()-1);
        exit(1);
    }

    int32_t v;
    // ok now we have two options we can either get a immediate or a symbol
    if(tokens[1].type == token_type::sym)
    {
        if(!symbol_table.count(tokens[1].literal))
        {
            printf("unrecognised symbol: %s(%s)\n",
                tokens[1].literal.c_str(),instr.c_str());
            exit(1);
        }

        const uint32_t cur = offset;
        const auto target = symbol_table[tokens[1].literal].value;
        v = target - cur;
    }

    else if(tokens[1].type == token_type::imm)
    {
        v = std::stoi(tokens[1].literal,0,0);
    }
    
    else
    {
        printf("invalid operand for branch: %s(%s)\n",tokens[1].literal.c_str(),instr.c_str());
        exit(1);
    }


    const auto sign = is_set(v,(sizeof(v)*8)-1);
    const int32_t branch =  (v >> 2) & (set_bit(0,24) - 1);        
    const int32_t final_branch = sign? set_bit(branch,23) : deset_bit(branch,23);

    //24 bit imm added to pc to reach target
    // left shifed by two then sign extended
    if(abs(v) > set_bit(0,22))
    {
        printf("cannot represent relative branch in 26 bit signed op: %x\n",v);
        exit(1);
    }

    opcode |= final_branch;

    return opcode;
}

// look for a more mathy way to do this (brute force with a loop is slow for obvious reasons)
// needs unit tests
bool encode_i_type_operand(int32_t &v, uint32_t &s)
{
    // ok so we have a 4 bit shift and 12 bit signed imm
    // to encode V into

    const auto sign = is_set(v,(sizeof(v)*8)-1);

    // what this effectively means is we need to check if the number can fit into a 12bit imm
    // and keep shifting it down up to 15 unti it does if it cant then it is invalid
    for(s = 0; s < 16; s++)
    {
        if(abs(v) <= set_bit(0,10)-1)
        {
            v = sign? set_bit(v,11) : deset_bit(v,11); 
            v &= set_bit(0,12)-1;
            return true;
        }

        // if we have a right side carry then its not possible to encode
        if(is_set(abs(v),0))
        {
            return false;
        }

        v >>= 1;
    }

    return false;
}

uint32_t Assembler::decode_i_instr(const Instr &instr_entry,const std::string &instr,const std::vector<Token> &tokens)
{   
    UNUSED(instr_entry); UNUSED(tokens); UNUSED(instr);
    // ok we are expecting eg addi r0, 0x1

    uint32_t opcode = 0;

    // at some point we wanna handle having operands implicitly
    // eg addi r0, r0, 1 = addi r0, 1
    if(tokens.size()-1 != instr_entry.operand_count)
    {
        printf("imm: expected %d operands but got: %zd\n",instr_entry.operand_count,tokens.size()-1);
        exit(1);
    }    

    switch(instr_entry.operand_count)
    {
        case 3:
        {
            // ok we are expecting two regs followed by a imm or symbol
            if(tokens[1].type != token_type::reg || tokens[2].type != token_type::reg)
            {
                printf("imm: expected reg for first operand %s\n",instr.c_str());
            }

            int32_t v = 0;
            uint32_t s = 0;

            if(tokens[3].type == token_type::imm)
            {
                v = std::stoi(tokens[3].literal,0,0); 
            }

            else if(tokens[3].type == token_type::sym)
            {
                puts("imm symbol unhandled");
                exit(1);
            }

            else
            {
                printf("imm: expected sybmol or imm for 2nd operand: %s\n",instr.c_str());
                exit(1);
            }

            const auto success = encode_i_type_operand(v,s);

            // TODO add psuedo op that will encode ones too large
            // into several instrs
            if(!success)
            {
                printf("cannot encode i type operand: %d\n",v);
                exit(1);
            }

            opcode |= register_table[tokens[1].literal]  << DST_OFFSET // dst
                | register_table[tokens[2].literal] << SRC1_OFFSET // src
                | v | (s << 16); // encode imm and shift
            break;
        }

        default:
        {
            printf("imm unhandled operand len %d\n",instr_entry.operand_count);
            exit(1);
        }
    }

    return opcode;
}

uint32_t Assembler::assemble_opcode(const std::string &instr,const std::vector<Token> &tokens)
{
    
    //dump_token_debug(tokens);
   
    const auto instr_entry = instr_table[tokens[0].literal];

    // every opcode has the group and opcode field
    uint32_t opcode = (instr_entry.group << 28) | (instr_entry.opcode << 24);

    switch(instr_entry.type)
    {

        case instr_type::S: // register to register opcodes
        {
            opcode |= decode_s_instr(instr_entry,instr,tokens);
            break;
        }


        case instr_type::B: // relative branch
        {
            opcode |= decode_b_instr(instr_entry,instr,tokens);
            break;
        }

        case instr_type::I: // immediate
        {
            opcode |= decode_i_instr(instr_entry,instr,tokens);
            break;
        }

        default:
        {
            printf("unhandled opcode type: %d(%s)\n",static_cast<int>(instr_entry.type),instr.c_str());
            dump_symbol_table_debug();
            exit(1);
        }

        break;
        
    }

    return opcode;
}
