#include <mina/assembler.h>
#include <test/instr.h>


struct ImmediateTest
{
    ImmediateTest(std::string i,std::string e, bool b) : imm(i),expected(e), success(b)
    {

    }

    std::string imm;
    std::string expected;
    bool success;
};

static constexpr uint32_t IMMEDIATE_TEST_SIZE = 10;

ImmediateTest immediate_test_table[IMMEDIATE_TEST_SIZE] = 
{
    {"55","55",true},
    {"46 ","46",true},
    {"9444,","9444",true},
    {"0x55","0x55",true},
    {"0xf","0xf",true},
    {"0","0", true},
    {"0b1001","0b1001",true},
    {"a","",false},
    {"55555Z","",false},
    {"0b","",false}
};

void immediate_test()
{
    std::string literal = "";
    for(uint32_t i = 0; i < IMMEDIATE_TEST_SIZE; i++)
    {
        const auto test = immediate_test_table[i];
        const auto success = verify_immediate(test.imm,literal);

        // make sure to only compare expected if input is valid
        if((test.success && literal == test.expected) || success == test.success)
        {
            printf("immediate test pass: %s\n",test.expected.c_str());
        }

        else
        {
            printf("immediate test fail: %s != %s\n",test.expected.c_str(),literal.c_str());
        }
    }
    
}

void run_tests()
{
    puts("running tests...");


    instr_test();
    immediate_test();
}