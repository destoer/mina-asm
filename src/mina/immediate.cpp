#include <mina/assembler.h>

template<typename F>
bool verify_immediate_internal(const std::string &instr, size_t &i, F lambda)
{
    const auto len = instr.size();

    for(; i < len; i++)
    {
        // token terminated
        if(instr[i] == ',' || instr[i] == ' ' || instr[i] == ']')
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

int32_t convert_imm(const std::string &imm)
{
    if(imm.size() >= 3 && imm.substr(0,2) == "0b")
    {
        return std::stoi(imm.substr(2),0,2);
    }

    // stoi wont auto detect base for binary strings?
    return std::stoi(imm,0,0);
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
