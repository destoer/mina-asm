#include <mina/assembler.h>

// align to 4 byte boundary
uint32_t align(uint32_t v)
{
    return (v + 3) & ~3;
}

void Assembler::define_string(const std::vector<Token> &tokens, uint32_t pass)
{
    if(tokens.size() != 2)
    {
        printf("string: expected 2 operands but got %zd:\n",tokens.size());
        exit(1);
    }

    const auto literal = tokens[1].literal;

    if(pass == 1)
    {
        // verify we have a string in the right place
        // and alloc space for it
        if(tokens[1].type == token_type::str)
        {
            offset += align(tokens[1].literal.size() + 1);
        }

        else
        {
            printf("string: expected string as param\n");
            exit(1);
        }
    }

    else if(pass == 2)
    {
        memcpy(&output[offset],literal.data(),literal.size());
        const auto size = literal.size() + 1;
        output[literal.size()] = '\0';
        offset += align(size);
    }
}

void Assembler::include_text_file(const std::vector<Token> &tokens, uint32_t pass)
{
    if(tokens.size() != 2)
    {
        printf("include: expected 2 operands but got %zd:\n",tokens.size());
        exit(1);
    }


    const auto literal = tokens[1].literal;

    if(pass == 1)
    {
        // insert file at current line
        if(tokens[1].type == token_type::str)
        {
            const auto str = read_file(tokens[1].literal);
            if(str == "")
            {
                printf("could not include file: %s\n",tokens[1].literal.c_str());
                exit(1);
            }
            const auto include = read_string_lines(str);
            file_lines.insert(file_lines.begin()+line,include.begin(),include.end());
        }

        else
        {
            printf("include: expected string as param\n");
            exit(1);
        }
    }

}

void Assembler::equ(const std::vector<Token> &tokens, uint32_t pass)
{
    if(tokens.size() != 3)
    {
        printf("equ: expected 2 operands but got %zd:\n",tokens.size());
        exit(1);
    }



    if(pass == 1)
    {    
        if(tokens[0].type != token_type::sym || tokens[2].type != token_type::imm)
        {
            printf("equ: expected <symbol> equ <imm>");
            exit(1);
        }

        if(symbol_table.count(tokens[0].literal))
        {
            printf("equ: symbol redefinition of: %s\n",tokens[0].literal.c_str());
            exit(1);
        }

        symbol_table[tokens[0].literal] = convert_imm(tokens[2].literal);
    }
}

void Assembler::include_binary_file(const std::vector<Token> &tokens, uint32_t pass)
{
    if(tokens.size() != 2)
    {
        printf("file: expected 2 operands but got %zd:\n",tokens.size());
        exit(1);        
    }

    if(pass == 1)
    {   
        if(tokens[1].type == token_type::str)
        {
            const auto size = get_file_size(tokens[1].literal);

            if(!size)
            {
                printf("file: could not open binary file %s\n",tokens[1].literal.c_str());
                exit(1);
            }

            directive_size[offset] = size;
            offset += align(size);
        }

        else
        {
            printf("file: expected string as param\n");
            exit(1);
        }        
    }

    else if(pass == 2)
    {
        if(!directive_size.count(offset))
        {
            printf("file: could not read file size from table");
            exit(1);
        }

        const auto old_size = directive_size[offset];


        std::ifstream fp(tokens[1].literal,std::ios::binary);

        if(!fp)
        {
            printf("file: could not open binary file %s\n",tokens[1].literal.c_str());
            exit(1);
        }



        fp.seekg(0,std::ios::end);
        const auto size = fp.tellg();
        fp.seekg(0,std::ios::beg);

        if(old_size != size)
        {
            printf("file: size of file changed on disk: %s\n",tokens[1].literal.c_str());
            exit(1);
        }


        fp.clear();

        fp.read((char*)&output[offset],size);

        fp.close();

        offset += align(size);
    }

}