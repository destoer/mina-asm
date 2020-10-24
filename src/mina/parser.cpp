#include <mina/mina.h>

void Ast::print(AstNode *node)
{
    if(node != nullptr)
    {
        printf("node: cur: %p left: %p right: %p: %s:%s\n",
            node,node->left,node->right,token_names[static_cast<int>(node->data.type)],node->data.literal.c_str());

        if(node->left != nullptr)
        {
            print(node->left);
        }

        if(node->right != nullptr)
        {
            print(node->right);
        }
    }
}

Ast::Ast(const std::vector<Token> &t)
{
    tok_idx= 0;
    brace_count = 0;
    operands = 0;
    tokens = t;
    token = next();
    root = line();

    if(brace_count != 0)
    {
        printf("unterminated expression\n");
        exit(1);
    }
}


// todo handle instrs and directives that have nothing after them
AstNode *Ast::line()
{
    AstNode *node = new AstNode(nullptr,token,nullptr);

    switch(token.type)
    {
        // shove expr on left side wiht right side having a comma
        // to indicate we have another param
        case token_type::directive:
        case token_type::instr:
        {
            // no operands no need to further parse
            if(tokens.size() == 1)
            {
                break;
            }


            auto cur = node;

            // expect atleast one param
            do
            {
                cur->right = new AstNode(nullptr,token_type::comma,nullptr);
                cur = cur->right;
                token = next();
                cur->left = expression(0);
                operands += 1;
            } while(token.type == token_type::comma);

            break;
        }

        
        // we need to look at the 2nd  token to see if its part of an expr
        // or its a symbol eg equ

        case token_type::sym:
        {
            // here we are expecting something like x = 5 * 5
            if(peek().type != token_type::directive)
            {
                node = expression(0);
                operands = 1;
            }

            // eg  x equ 5
            else
            {
                const auto directive = next();
                node->right = new AstNode(nullptr,directive,nullptr);

                token = next();
                node->left = expression(0);
                
                operands = 2;
            }
            break;
        }

        default:
        {
            printf("line: unexpected token: %s\n",token_names[static_cast<int>(token.type)]);
            exit(1);            
        }
    }

    return node;
}

// helpers

void Ast::delete_ast(AstNode *node)
{
    if(node != nullptr)
    {
        if(node->left != nullptr)
        {
            delete_ast(node->left);
        }

        if(node->right != nullptr)
        {
            delete_ast(node->right);
        }

        delete node;
        node = nullptr;
    }
}

Ast::~Ast()
{
    delete_ast(root);
}


Token Ast::peek()
{
    if(tok_idx >= tokens.size())
    {
        return Token(token_type::eof);
    }

    return tokens[tok_idx];    
}

Token Ast::next()
{
    if(tok_idx >= tokens.size())
    {
        return Token(token_type::eof);
    }

    const auto tok =  tokens[tok_idx++];

    // easiest just to jam a state machine in here
    if(tok.type == token_type::left_paren)
    {
        brace_count += 1;
    }

    else if(tok.type == token_type::right_paren)
    {
        brace_count -= 1;
    }

    return tok;
}

void Ast::skip(token_type type)
{
    if(type != token.type)
    {
        // we need to get better error reporting out of the ast
        // should probably throw so our assembler die function
        // cant report the error better?
        printf("expected: %s got %s\n",token_names[static_cast<int>(type)],token_names[static_cast<int>(token.type)]);
        exit(1);
    }

    token = next();
}







// pratt parser for expressions


// null determination i.e unary minus -5
// does not require something to come before it
AstNode *Ast::nud(Token &t)
{
    switch(t.type)
    {
        case token_type::imm:
        case token_type::sym:
        case token_type::reg:
        case token_type::str:
        {
            return new AstNode(nullptr,t,nullptr);
        }

        // unary operators have a very high binding power
        case token_type::minus:
        case token_type::plus:
        {
            return new AstNode(expression(100),t,nullptr);
        }

        case token_type::left_paren:
        {
            const auto expr = expression(0);

            skip(token_type::right_paren);

            return expr;
        }

        case token_type::eof:
        {
            return nullptr;
        }

        default:
        {
            printf("nud: unexpected token: %s\n",token_names[static_cast<int>(t.type)]);
            exit(1);
        }
    }

    // should not be reached
    return nullptr;
}



// standard binary operators eg 5 + 5 , 6 * 4
AstNode *Ast::led_binary_op(Token &t,AstNode *left)
{
    return new AstNode(left,t,expression(lbp(t.type)));
}


// left denotion
// takes left and consumes a right from token stream
// eg binary operators 5 + 5
AstNode *Ast::led(Token &t,AstNode *left)
{
    const auto idx = static_cast<size_t>(t.type);
    const auto func = led_table[idx];

    if(func == nullptr)
    {
        printf("led: unexpected token: %s",token_names[idx]);
        exit(1);
    }
    
    return std::invoke(func,this,t,left);
}


// should be a lut later :D
int32_t Ast::lbp(const Token &t)
{
    const auto bp = lbp_table[static_cast<int>(t.type)];

    if(bp == -1)
    {
        printf("lbp: illegal token: %s",token_names[static_cast<int>(t.type)]);
        exit(1);
    }           


    return bp;
}

// pratt parser
// http://effbot.org/zone/simple-top-down-parsing.htm
// https://web.archive.org/web/20151223215421/http://hall.org.ua/halls/wizzard/pdf/Vaughan.Pratt.TDOP.pdf
AstNode *Ast::expression(int32_t rbp)
{
    auto t =  token;
    token = next();

    // parse out any unary operators
    auto left = nud(t);
    
    while(rbp < lbp(token))
    {
        t = token;
        token = next();
        left = led(t,left);
    }

    return left;
}