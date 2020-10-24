#include <mina/mina.h>


struct SumTest
{
    SumTest(const std::string &e, int32_t a) : expression(e), ans(a)
    {

    }

    std::string expression;
    int32_t ans;
};


static const SumTest sum_tests[] = 
{

    // should panic
    //{"-*-",0},
    //{"4)",4},
    //{"((4+5)))+4",13},
    //{")(3+55",58},
    {"x = 4 / 2",2},
    {"x = 4 % 3",1},
    {"x = ((5+5)*2) + 4",24},
    {"x = (5 + 3) * 4",32},
    {"x = 3 + 4 * -5", -17},
    {"x = 5 - 5 * 2", -5},
    {"x = 3 + 4 + 5", 12},
    {"x = -5 + 5",0},
    {"x = +5 - 5 + 4",4},
    {"x = -5",-5},
    {"x = +5",5}
};

static constexpr uint32_t SUM_TEST_SIZE = sizeof(sum_tests) / sizeof(sum_tests[0]);

void expr_test()
{
    Assembler assembler;

    puts("expression test");

    for(uint32_t i = 0; i < SUM_TEST_SIZE; i++)
    {
        const auto test = sum_tests[i];
        const auto tokens = assembler.parse_tokens(test.expression);
        Ast ast(tokens);

        // this is kind of cheating using an equals to force it not to 
        // reject the expr but its fine for now
        const auto ans = assembler.sum(ast.root->right);


        if(ans == test.ans)
        {
            printf("pass: '%s' %d = %d\n",test.expression.c_str(),ans,test.ans);
        }

        else
        {
            printf("fail: '%s' %d != %d\n",test.expression.c_str(),ans,test.ans);
            dump_token_debug(tokens);
            ast.print(ast.root);
            exit(1);
        }
    }
}