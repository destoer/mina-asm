#include <test/instr.h>

static constexpr uint32_t INSTR_TEST_SIZE = 4;
InstrTest instr_test_table[INSTR_TEST_SIZE] = 
{
    // mov tests
    {Opcode(0b0101,0b1000,5,0,0), "mov r0, r5"},
    {Opcode(0b0101,0b1001,2,0,4), "mt r4, r2"},
    {Opcode(0b0101,0b1100,0,0,15), "mfrc sp"},

    // branch rel tests
    {Opcode(0b1000,0b0000,550000 >> 2),"bra 550000"}
};

void instr_test()
{
    Assembler assembler;

    for(uint32_t i = 0; i < INSTR_TEST_SIZE; i++)
    {
        const auto test = instr_test_table[i];
        const auto tokens = assembler.parse_tokens(test.line);
        if(!tokens.size() || tokens[0].type != token_type::instr)
        {
            printf("faulty test: %d:%s\n",i,test.line.c_str());
            exit(1);
        }
        Opcode op;
        const auto v = assembler.assemble_opcode(test.line,tokens);
        op.decode_op(v,assembler.get_instr_entry(tokens[0].literal).type);
    
        if(op == test.expected)
        {
            printf("instr test passed %d:%s(%x)\n",i,test.line.c_str(),v);
        }

        else
        {
            printf("instr test failed %d:%s(%x)\n",i,test.line.c_str(),v);

            puts("\nexpected: \n");
            test.expected.print();

            puts("\ngot: \n");
            op.print();

            return;
        }
    
    }
}