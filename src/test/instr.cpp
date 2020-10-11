#include <mina/assembler.h>

struct Opcode
{
    Opcode() {};

    Opcode(const Opcode &op)
    {
        this->opcode = op.opcode;
        this->group = op.group;
        this->op1 = op.op1;
        this->op2 = op.op2;
        this->op3 = op.op3;
        this->op4 = op.op4;
        this->op5 = op.op5;        
    }

    Opcode(instr_group group, int32_t opcode, int32_t op1=0, int32_t op2=0, int32_t op3=0, int32_t op4=0, int32_t op5=0)
    {
        this->group = static_cast<uint32_t>(group);
        this->opcode = opcode;
        this->op1 = op1;
        this->op2 = op2;
        this->op3 = op3;
        this->op4 = op4;
        this->op5 = op5;
    }

    void print() const
    {
        printf("opcode: %x\ngroup:%x\nop1:%x\nop2:%x\nop3:%x\nop4:%x\nop5:%x\n",
            opcode,group,op1,op2,op3,op4,op5);
    }

    void decode_op(uint32_t op, instr_type type)
    {
        group = (op >> 28) & 0xf;
        opcode = (op >> 24) & 0xf;


        switch(type)
        {
            case instr_type::S:
            {
                op1 = (op >> 20) & 0xf;
                op2 = (op >> 16) & 0xf;
                op3 = (op >> 12) & 0xf;
                op4 = 0;
                op5 = 0;
                break;
            }

            case instr_type::I:
            {
                op1 = (op >> 20) & 0xf;
                op2 = (op >> 16) & 0xf;
                op3 = (op >> 12) & 0xf;
                op4 = (op & (set_bit(0,12)-1));
                op4 = sign_extend(op4,12);
                // we are just putting the imm in op4 only as its easier
                op5 = 0;
                break;                
            }

            case instr_type::B:
            {
                op1 = op & (set_bit(0,24) - 1);
                op1 = sign_extend(op1,24);
                break;
            }

            default:
            {
                printf("test unhandled instr decode: %d\n",static_cast<int>(type));
                exit(1);
            }
        }
    }

    bool operator ==(const Opcode &op) const
    {
        return opcode == op.opcode &&
            group == op.group &&
            op1 == op.op1 &&
            op2 == op.op2 && 
            op3 == op.op3 &&
            op4 == op.op4 &&
            op5 == op.op5;
    }

    int32_t opcode = 0;
    uint32_t group = 0;
    int32_t op1 = 0; // src1 / sbz
    int32_t op2 = 0; // src2 / shift / imm
    int32_t op3 = 0; // dest
    int32_t op4 = 0;  // sbz / imm / rshift
    int32_t op5 = 0; // sbz / imm / offset
};

struct InstrTest
{
    InstrTest(Opcode op, std::string l) : expected(op), line(l)
    {

    }

    Opcode expected;
    std::string line;
};

void instr_test();

static constexpr uint32_t INSTR_TEST_SIZE = 22;
InstrTest instr_test_table[INSTR_TEST_SIZE] = 
{
    // arithmetic tests

    // register - imm
    {Opcode(instr_group::arith,0b0000,12,0,13,5),"addi r13, r12, 5"},
    {Opcode(instr_group::arith,0b0000,12,0,13,-5),"addi r13, r12, -5"},
    {Opcode(instr_group::arith,0b0000,12,2,13,0xff0 >> 2),"addi r13, r12, 0xff0"},
    {Opcode(instr_group::arith,0b0000,12,2,13,-0xff0 >> 2),"addi r13, r12, -0xff0"},
    {Opcode(instr_group::arith,0b0001,12,0,13,5),"multi r13, r12, 5"},
    {Opcode(instr_group::arith,0b0010,12,0,13,5),"divi r13, r12, 5"},
    {Opcode(instr_group::arith,0b0011,12,0,13,5),"remi r13, r12, 5"},
    {Opcode(instr_group::arith,0b0100,12,0,13,5),"slti r13, r12, 5"},
    {Opcode(instr_group::arith,0b0101,12,0,13,5),"sltiu r13, r12, 5"},
    {Opcode(instr_group::arith,0b0110,0,0,0,0),"nop"},
    {Opcode(instr_group::arith,0b0111,0,0,13,5),"pcaddi r13, 5"},


    // register - register
    {Opcode(instr_group::arith,0b1001,12,5,13),"mult r13, r12, r5"},
    {Opcode(instr_group::arith,0b1010,12,5,13),"div r13, r12, r5"},
    {Opcode(instr_group::arith,0b1011,12,5,13),"rem r13, r12, r5"},
    {Opcode(instr_group::arith,0b1100,12,5,13),"slt r13, r12, r5"},
    {Opcode(instr_group::arith,0b1101,12,5,13),"sltu r13, r12, r5"},
    {Opcode(instr_group::arith,0b1111,0,5,13),"pcadd r13, r5"},

    // mov tests

    // register - register
    {Opcode(instr_group::mov,0b1000,5,0,0), "mov r0, r5"},
    {Opcode(instr_group::mov,0b1001,2,0,4), "mt r4, r2"},
    {Opcode(instr_group::mov,0b1100,0,0,15), "mfrc sp"},

    // branch rel tests
    {Opcode(instr_group::rel_branch,0b0000,0x8000 >> 2),"bra 0x8000"},
    {Opcode(instr_group::rel_branch,0b0000,-0x8000 >> 2),"bra -0x8000"}
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
            printf("instr test passed %d:%s(%08x)\n",i,test.line.c_str(),v);
        }

        else
        {
            printf("instr test failed %d:%s(%08x)\n",i,test.line.c_str(),v);

            puts("\nexpected: \n");
            test.expected.print();

            puts("\ngot: \n");
            op.print();

            return;
        }
    
    }
}