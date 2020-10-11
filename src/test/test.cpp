#include <test/test.h>
#include <mina/assembler.h>

void run_tests()
{
    puts("running tests...");

    int32_t v = 0xff000000;
    uint32_t s = 0;
    assert(encode_i_type_operand(v,s));

    instr_test();
    imm_operand_test();
}