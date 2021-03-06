#include <mina/mina.h>

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

            case instr_type::M:
            {
                op1 = 0;
                op2 = 0;
                op3 = (op >> 12) & 0xf;  
                // encode the split imm back in to one param just because its easy
                op4 = (op & 0x0fff) | ((op >> SRC2_OFFSET) & 0xf) << 12;
                op5 = 0;               
                break;
            }

            case instr_type::F:
            {
                op1 = (op >> 20) & 0xf;
                op2 = (op >> 16) & 0xf;
                op3 = (op >> 12) & 0xf;
                op4 = (op >> 8) & 0xf;
                op5 = 0;
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


InstrTest instr_test_table[] = 
{
    // arithmetic tests

    // register - imm
    {Opcode(instr_group::arith,0b0000,12,0,13,5*5),"addi r13, r12, 5*5"},
    {Opcode(instr_group::arith,0b0000,12,0,13,0b11),"addi r13, r12, 0b11"},
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
    {Opcode(instr_group::arith,0b1111,5,0,13),"pcadd r13, r5"},


    // logical tests

    // register - immediate

    {Opcode(instr_group::logic,0b0000,12,0,13,5),"andi r13, r12, 5"},
    {Opcode(instr_group::logic,0b0001,12,0,13,5),"ori r13, r12, 5"},
    {Opcode(instr_group::logic,0b0010,12,0,13,5),"xori r13, r12, 5"},
    {Opcode(instr_group::logic,0b0011,12,0,13,5),"nandi r13, r12, 5"},

    // register - register
    {Opcode(instr_group::logic,0b1000,5,15,0), "and r0, r5, sp"},
    {Opcode(instr_group::logic,0b1001,5,15,0), "or r0, r5, sp"},
    {Opcode(instr_group::logic,0b1010,5,15,0), "xor r0, r5, sp"},
    {Opcode(instr_group::logic,0b1011,5,15,0), "nand r0, r5, sp"},
    {Opcode(instr_group::logic,0b1100,1,0,1), "popcnt r1, r1"},
    {Opcode(instr_group::logic,0b1101,5,0,0), "clo r0, r5"},
    {Opcode(instr_group::logic,0b1110,5,0,0), "plo r0, r5"},


    // cmp tests

    // register - immedaite
    {Opcode(instr_group::cmp,0b0000,1,0,0,5), "cmpi/eq r1, 5"},
    {Opcode(instr_group::cmp,0b0001,1,0,0,5), "cmpi/lo r1, 5"},
    {Opcode(instr_group::cmp,0b0010,1,0,0,5), "cmpi/ls r1, 5"},
    {Opcode(instr_group::cmp,0b0011,1,0,0,5), "cmpi/lt r1, 5"},
    {Opcode(instr_group::cmp,0b0100,1,0,0,5), "cmpi/le r1, 5"},

    // register - register
    {Opcode(instr_group::cmp,0b1000,1,5,0), "cmp/eq r1, r5"},
    {Opcode(instr_group::cmp,0b1001,1,5,0), "cmp/lo r1, r5"},
    {Opcode(instr_group::cmp,0b1010,1,5,0), "cmp/ls r1, r5"},
    {Opcode(instr_group::cmp,0b1011,1,5,0), "cmp/lt r1, r5"},
    {Opcode(instr_group::cmp,0b1100,1,5,0), "cmp/le r1, r5"},

    // register branch tests

    // register - immediate

    // implicit shift of 2
    {Opcode(instr_group::reg_branch,0b0000,4,0,0,0x20 >> 2),"rbra r4, 0x20"},
    {Opcode(instr_group::reg_branch,0b0001,4,0,0,0x20 >> 2),"rcall r4, 0x20"},
    {Opcode(instr_group::reg_branch,0b0010,0,0,0,0),"ret"},

    // register - register
    {Opcode(instr_group::reg_branch,0b1000,1,5,0), "robra r1, r5"},
    {Opcode(instr_group::reg_branch,0b1001,1,5,0), "rocall r1, r5"},

    // mem tests

    // register - immediate

    {Opcode(instr_group::mem,0b0000,2,0,8,4 >> 2),"ld r8, [r2, 4]"},
    {Opcode(instr_group::mem,0b0001,2,0,8,4 >> 1),"ldh r8, [r2, 4]"},
    {Opcode(instr_group::mem,0b0010,2,0,8,4),"ldb r8, [r2, 4]"},
    {Opcode(instr_group::mem,0b0011,2,0,8,4 >> 2),"st r8, [r2, 4]"},
    {Opcode(instr_group::mem,0b0100,2,0,8,4 >> 1),"sth r8, [r2, 4]"},
    {Opcode(instr_group::mem,0b0101,2,0,8,4),"stb r8, [r2, 4]"},
    {Opcode(instr_group::mem,0b0110,2,0,0,4 >> 2),"ldc [r2, 4]"},
    {Opcode(instr_group::mem,0b0111,2,0,0,4 >> 2),"stc [r2, 4]"},

    // register - register
    {Opcode(instr_group::mem,0b1000,2,4,8,0),"rld r8, [r2, r4]"},
    {Opcode(instr_group::mem,0b1001,2,4,8,0),"rldh r8, [r2, r4]"},
    {Opcode(instr_group::mem,0b1010,2,4,8,0),"rldb r8, [r2, r4]"},
    {Opcode(instr_group::mem,0b1011,2,4,8,0),"rst r8, [r2, r4]"},
    {Opcode(instr_group::mem,0b1100,2,4,8,0),"rsth r8, [r2, r4]"},
    {Opcode(instr_group::mem,0b1101,2,4,8,0),"rstb r8, [r2, r4]"},
    {Opcode(instr_group::mem,0b1110,0,0,8,0),"pop r8"},
    {Opcode(instr_group::mem,0b1111,0,0,8,0),"push r8"},

    // mov tests

    // register - immedaite
    {Opcode(instr_group::mov,0b0000,0,0,1,5), "movi r1, 5"},
    {Opcode(instr_group::mov,0b0001,0,0,1,5), "mti r1, 5"},
    {Opcode(instr_group::mov,0b0010,0,0,1,5), "mfi r1, 5"},

    // special
    {Opcode(instr_group::mov,0b0011,0,0,1,0xffff), "movl r1, 0xffff"},
    {Opcode(instr_group::mov,0b0100,0,0,1,0xffff), "movu r1, 0xffff"},

    // register - register
    {Opcode(instr_group::mov,0b1000,5,0,0), "mov r0, r5"},
    {Opcode(instr_group::mov,0b1001,2,0,4), "mt r4, r2"},
    {Opcode(instr_group::mov,0b1100,0,0,15), "mfrc sp"},

    // shift tests
    
    // register - immedaite
    {Opcode(instr_group::shift,0b0000,3,4,8,0),"lsl r8, r3, 4"},
    {Opcode(instr_group::shift,0b0001,3,4,8,0),"lsr r8, r3, 4"},
    {Opcode(instr_group::shift,0b0010,3,4,8,0),"asr r8, r3, 4"},
    {Opcode(instr_group::shift,0b0011,3,4,8,0),"ror r8, r3, 4"},

    // register - register
    {Opcode(instr_group::shift,0b1000,3,4,8),"rlsl r8, r3, r4"},
    {Opcode(instr_group::shift,0b1001,3,4,8),"rlsr r8, r3, r4"},
    {Opcode(instr_group::shift,0b1010,3,4,8),"rasr r8, r3, r4"},
    {Opcode(instr_group::shift,0b1011,3,4,8),"rror r8, r3, r4"},

    // special
    {Opcode(instr_group::shift,0b1100,3,4,8,5),"flsl r8,r3,r4,5"},
    {Opcode(instr_group::shift,0b1101,3,4,8,5),"flsr r8,r3,r4,5"},


    // control instructions

    // register - immedaite
    {Opcode(instr_group::cnt,0b0000,0,0,0,0),"stop"},
    {Opcode(instr_group::cnt,0b0001,0,0,0,0),"wfi"},
    {Opcode(instr_group::cnt,0b0010,0,0,0,0),"sett"},
    {Opcode(instr_group::cnt,0b0011,0,0,0,0),"clrt"},
    {Opcode(instr_group::cnt,0b0100,0,0,0,0),"switch"},


    // register - register
    {Opcode(instr_group::cnt,0b1000,0,0,1,0),"svcall r1"},
    {Opcode(instr_group::cnt,0b1001,2,0,1,0),"fault r1, r2"},
    {Opcode(instr_group::cnt,0b1010,0,0,1,0),"mtof r1"},
    {Opcode(instr_group::cnt,0b1011,0,0,1,0),"mfrf r1"},



    // branch 
    
    // rel barnch
    {Opcode(instr_group::rel_branch,0b0000,0x8000 >> 2),"bra 0x8000"},
    {Opcode(instr_group::rel_branch,0b0000,-0x8000 >> 2),"bra -0x8000"},
    {Opcode(instr_group::rel_branch,0b0001,0x8000 >> 2),"bt 0x8000"},
    {Opcode(instr_group::rel_branch,0b0010,0x8000 >> 2),"bf 0x8000"},

    // rel call
    {Opcode(instr_group::rel_branch,0b1000,0x8000 >> 2),"call 0x8000"},
    {Opcode(instr_group::rel_branch,0b1001,0x8000 >> 2),"ct 0x8000"},
    {Opcode(instr_group::rel_branch,0b1010,0x8000 >> 2),"cf 0x8000"}
};

static constexpr uint32_t INSTR_TEST_SIZE = sizeof(instr_test_table) / sizeof(instr_test_table[0]);

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
        const auto v = assembler.assemble_opcode(tokens);
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