
#pragma once
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

    Opcode(int group, int opcode, int op1=0, int op2=0, int op3=0, int op4=0, int op5=0)
    {
        this->group = group;
        this->opcode = opcode;
        this->op1 = op1;
        this->op2 = op2;
        this->op3 = op3;
        this->op4 = op4;
        this->op5 = op5;
    }

    void print() const
    {
        printf("opcode: %d\ngroup:%d\nop1:%d\nop2:%d\nop3:%d\nop4:%d\nop5:%d\n",
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

    int opcode = 0;
    int group = 0;
    int op1 = 0; // src1 / sbz
    int op2 = 0; // src2 / shift / imm
    int op3 = 0; // dest
    int op4 = 0;  // sbz / imm / rshift
    int op5 = 0; // imm / offset / sbz
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