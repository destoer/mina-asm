#include <mina/mina.h>
#include <stdarg.h>

//https://github.com/ladystarbreeze/mina-isa/blob/main/MINA_Instruction_Set_Architecture.md
/*
S-type: [group(4)][opcode(4)][src1(4)][src2 (4)][dest(4)][ SBZ  (4)][ SBZ  (8)]
I-type: [group(4)][opcode(4)][src1(4)][shift(4)][dest(4)][ imm  (4)][ imm  (8)]
M-type: [group(4)][opcode(4)][SBZ (4)][ imm (4)][dest(4)][ imm  (4)][ imm  (8)]
F-type: [group(4)][opcode(4)][src1(4)][src2 (4)][dest(4)][rshift(4)][ SBZ  (8)] // only for funnel shifts
B-type: [group(4)][opcode(4)][              offset             (16)][offset(8)]
*/



// TODO
// impl proper syntax parsing with a ast
// and verify valid lines that way
// rewrite error messages

Assembler::Assembler(const std::string &filename)
{
    init(filename);
}


void Assembler::init(const std::string &filename)
{
    file_lines = read_string_lines(read_file(filename));

    if(!file_lines.size())
    {
        printf("file %s could not be read!\n",filename.c_str());
        exit(1);
    }

    offset = 0;    
    line = 0;
}


void Assembler::die(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);
    
    // if we are running tests we dont want this to try print source line info
    if(line && file_lines.size())
    {
        printf("line %d: '%s'\n",line-1, file_lines[line-1].c_str());
    }

    exit(1);
}

// first assembler pass resolve all symbols and figure out required size of the binary
void Assembler::first_pass()
{

    offset = 0;
    line = 0;

    // lines have to be parsed this way incase the include directive
    // jams extra lines into it
    for(size_t i = 0; i < file_lines.size(); i++)
    {
        line += 1;
        const auto tokens = parse_tokens(file_lines[i]);

        // nothing to do
        if(tokens.size() == 0)
        {
            continue;
        }

        Ast ast(tokens);

        auto token = ast.root->data;

        switch(token.type)
        {
            case token_type::instr:
            {
                offset += 4; // fixed size instr all are 4 bytes
                break;
            }

            case token_type::directive:
            {
                //dump_token_debug(tokens);

                const auto directive = directive_table[tokens[0].literal];
                std::invoke(directive.callback,this,tokens,1);
                break;
            }

            // potential label definiton
            case token_type::sym:
            {
                // search for a ':'
                // if we dont find it then we have to check to see if the next token is a directive
                // and if so invoke it
                // this will need to be changed when we add proper syntax parsing
                auto idx = token.literal.find(':');
                if(idx == std::string::npos)
                {
                    if(tokens.size() < 3)
                    {
                        die("unexpected symbol\n");

                    }

                    if(tokens[1].type != token_type::directive)
                    {
                        die("unexpected symbol\n");              
                    }

                    const auto directive = directive_table[tokens[1].literal];
                    std::invoke(directive.callback,this,tokens,1);
                    break;
                }


                // valid label add it to the symbol table if not allready defined
                if(symbol_table.count(token.literal))
                {
                    die("redefinition of symbol: %s\n",token.literal.c_str());
                }

                token.literal = token.literal.substr(0,idx); 

                symbol_table[token.literal] = Symbol(offset);

                break;
            }
            
            case token_type::equals:
            {
                const auto literal = ast.root->left->data.literal;

                if(symbol_table.count(literal))
                {
                    die("redefinition of symbol: %s\n",literal.c_str());
                }

                symbol_table[literal] = sum(ast.root->right);

                break;
            }

            default:
            {
                die("first parse unhandled token\n");
            }
        }        
    }

    output.resize(offset);
}

void Assembler::assemble_file()
{
    if(!file_lines.size())
    {
        printf("assembler: no file loaded");
        return;
    }

    first_pass();

    offset = 0;
    line = 0;

    for(size_t i = 0; i < file_lines.size(); i++)
    {
        line += 1;
        assemble_line(file_lines[i]);
    }
}

void Assembler::write_binary(const std::string &filename)
{
    write_file(filename,output);
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

            case '"': // start of string literal
            {
                const auto idx = instr.find("\"",i);

                if(idx == std::string::npos)
                {
                    die("unteriminated string literal");
                }

                std::string literal = instr.substr(i,idx-i);
                i += literal.size()+1;

                tokens.push_back(Token(literal,token_type::str));
                break;
            }

            // effectively whitespace dont care
            case '\n':
            case '\r':
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
                    die("unexpected char ',' in line\n");
                }

                tokens.push_back(Token("",token_type::comma));

                break;
            }

            // this along with arithmetic operators, spaces, commands
            // will have to actually verified for syntax atm
            // for simplicltly we are just ignoring them while we get the encoding down
            case ']':
            case '[':
            {
                break;
            }

            // do we need to do stuff for unary plus and minus here
            case '+':
            {
                tokens.push_back(Token("",token_type::plus));
                break;
            }

            case '-':
            {
                tokens.push_back(Token("",token_type::minus));
                break;
            }

            case '*':
            {
                tokens.push_back(Token("",token_type::multiply));
                break;
            }

            case '/':
            {
                tokens.push_back(Token("",token_type::divide));
                break;
            }


            case '%':
            {
                tokens.push_back(Token("",token_type::modulus));
                break;
            }

            case '=':
            {
                tokens.push_back(Token("",token_type::equals));
                break;
            }

            case '(':
            {
                tokens.push_back(Token("",token_type::left_paren));
                break;
            }

            case ')':
            {
                tokens.push_back(Token("",token_type::right_paren));
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

                    for(; i < len && instr[i] != ' ' && instr[i] != ',' && instr[i] != ']'; i++)
                    {
                        literal += instr[i];
                    }

                    //printf("found literal %s\n",literal.c_str());

                    // ok we have a literal now lets figure out what it is
                    // first check if it is a instr
                    // then if it is a directive
                    // then a register
                    // else a symbol

                    if(instr_table.count(literal))
                    {
                        tokens.push_back(Token(literal,token_type::instr));
                    }

                    else if(directive_table.count(literal))
                    {
                        if(directive_table[literal].callback == nullptr)
                        {
                            die("directive %s invalid callback!\n",tokens[0].literal.c_str());
                        }

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
                    die("illegal token: '%c'", c);
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
        printf("%s: %s\n",token_names[static_cast<int>(t.type)],t.literal.c_str());
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
        // TODO: cosider switching this over to using the ast
        switch(tokens[0].type)
        {
            case token_type::instr:
            {
                const auto opcode = assemble_opcode(tokens);

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

            case token_type::directive:
            {
                const auto directive = directive_table[tokens[0].literal];
                std::invoke(directive.callback,this,tokens,2);
                break;
            }

            default:
            {
                die("unknown token start %d\n",static_cast<int>(tokens[0].type));
            }
        }
    }
}


uint32_t Assembler::read_int_operand(const Token &token)
{
    if(token.type == token_type::imm)
    {
        return convert_imm(token.literal);
    }

    else if(token.type == token_type::sym)
    {
        if(!symbol_table.count(token.literal))
        {
            die("could not find symbol for imm: %s\n",token.literal.c_str());
        }

        return symbol_table[token.literal].value - offset;
    }

    else
    {
        die("expected imm or symbol for operand but got: %s\n",token_names[static_cast<int>(token.type)]);
        return 0;
    }
} 


uint32_t Assembler::assemble_opcode(const std::vector<Token> &tokens)
{
    
    //dump_token_debug(tokens);
   
    const auto instr_entry = instr_table[tokens[0].literal];

    // every opcode has the group and opcode field
    uint32_t opcode = (static_cast<uint32_t>(instr_entry.group) << 28) | (instr_entry.opcode << 24);


    using INSTR_ASSEMBLE = uint32_t (Assembler::*)(const Instr &instr_entry,const std::vector<Token> &tokens);

    INSTR_ASSEMBLE instr_assemble_table[5] = 
    {
        &Assembler::decode_s_instr,
        &Assembler::decode_i_instr,
        &Assembler::decode_m_instr,
        &Assembler::decode_f_instr,
        &Assembler::decode_b_instr
    };

    opcode |= std::invoke(instr_assemble_table[static_cast<int>(instr_entry.type)],this,instr_entry,tokens);

    return opcode;
}


int32_t Assembler::sum(AstNode *node)
{
    if(node != nullptr)
    {
        switch(node->data.type)
        {
            case token_type::imm:
            {
                return convert_imm(node->data.literal);
            }

            case token_type::sym:
            {
                const auto literal = node->data.literal;
                if(!symbol_table.count(literal))
                {
                    die("sum: undefined symbol: %s\n",literal.c_str());
                }

                return symbol_table[literal].value;
            }

            case token_type::plus:
            {
                if(node->right != nullptr)
                {
                    return sum(node->left) + sum(node->right);
                }

                // unary plus
                return +sum(node->left);
            }

            case token_type::minus:
            {
                if(node->right != nullptr)
                {
                    return sum(node->left) - sum(node->right);
                }

                // unary minus
                return -sum(node->left);     
            }

            case token_type::multiply:
            {
                return sum(node->left) * sum(node->right);
            }

            case token_type::divide:
            {
                return sum(node->left) / sum(node->right);
            }

            case token_type::modulus:
            {
                return sum(node->left) % sum(node->right);
            }

            default:
            {
                printf("sum illegal tok:%d\n",static_cast<int>(node->data.type));
                exit(1);
                break;
            }
        }
    }

    else
    {
        return 0;
    }
}


int32_t Assembler::read_op(AstNode *&root,operand_type type)
{
    if(root == nullptr || root->left == nullptr)
    {
        die("empty expr!?\n");
    }


    switch(type)
    {
        case operand_type::reg:
        {
            if(root->left->data.type != token_type::reg)
            {
                die("expected register\n");
            }

            const auto reg = register_table[root->left->data.literal];
            root = root->right;
            return reg;
        }

        case operand_type::val:
        {
            const auto v = sum(root->left);
            root = root->right;
            return v;
        }
    }

    printf("read_op fall through!?");
    exit(1);
}
