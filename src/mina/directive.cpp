#include <mina/mina.h>

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
    Ast ast(tokens);

    if(ast.operands != 2)
    {
        die("equ: expected 2 operands but got %zd:\n",tokens.size());
    }

    if(ast.root->left == nullptr || ast.root->right == nullptr)
    {
        die("equ: empty operands!?");
    }

    // try and figure out what the fuck is up with this tomorrow
    if(pass == 1)
    {   
        const std::string literal = ast.root->data.literal;
        

        if(symbol_table.count(literal))
        {
            die("equ: symbol redefinition of: %s\n",literal.c_str());
        }

        auto root = ast.root->left;
        const auto v = sum(root->left);
        symbol_table[literal] = v;
    }
}

// we can still saftely parse this on raw tokens cause we can only have a single str as op
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

void Assembler::dw(const std::vector<Token> &tokens, uint32_t pass)
{
    Ast ast(tokens);
    //ast.print(ast.root);

    if(ast.operands == 0)
    {
        die("dw no operands!?");
    }

    if(pass == 1)
    {   
        offset += ast.operands * 4;
    }


    else if(pass == 2)
    {
        auto cur = ast.root->right;
        for(size_t i = 0; i < ast.operands; i++)
        {
            // ok know we need to keep right side and pulling exprs off the left
            const auto v = read_op(cur,operand_type::val);
            write_vec(output,offset,v);
            offset += 4;
        }        
    }

}