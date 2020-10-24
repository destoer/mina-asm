#include <mina/mina.h>

// look for a more mathy way to do this (brute force with a loop is slow for obvious reasons)
// needs unit tests
bool encode_i_type_operand(int32_t &v, uint32_t &s)
{
    // ok so we have a 4 bit shift and 12 bit signed imm
    // to encode V into

    const auto sign = is_set(v,(sizeof(v)*8)-1);

    // what this effectively means is we need to check if the number can fit into a 12bit imm
    // and keep shifting it down up to 15 unti it does if it cant then it is invalid
    for(s = 0; s < 16; s++)
    {
        if(abs(v) <= set_bit(0,10)-1)
        {
            v = sign? set_bit(v,11) : deset_bit(v,11); 
            v &= set_bit(0,12)-1;
            return true;
        }

        // if we have a right side carry then its not possible to encode
        if(is_set(abs(v),0))
        {
            return false;
        }

        v >>= 1;
    }

    return false;
}


uint32_t Assembler::handle_i_implicit_shift(uint32_t v,uint32_t shift)
{
    if( (v & (set_bit(0,shift)-1)) > 0)
    {
        die("implicit shift cannot encode i type operand\n");
    }

    return v >> shift;
}


uint32_t Assembler::decode_i_instr(const Instr &instr_entry,const std::vector<Token> &tokens)
{
    // ok we are expecting eg addi r0, 0x1

    uint32_t opcode = 0;

    Ast ast(tokens);
    //ast.print(ast.root);

    if(ast.operands != instr_entry.operand_count)
    {
        die("i type instr: expected %d operands but got: %zd\n",instr_entry.operand_count,ast.operands);
    }

    switch(instr_entry.operand_count)
    {
        case 0: // eg nop
        {
            // no params so we have nothing to do
            break;
        }


        case 2:
        {
            auto root = ast.root->right;

            const auto reg = read_op(root,operand_type::reg);
            auto v = read_op(root,operand_type::val);
            uint32_t s = 0;


            if(instr_entry.group == instr_group::reg_branch)
            {
                v = handle_i_implicit_shift(v,reg_branch_shift[instr_entry.opcode]);
            }

            else if(instr_entry.group == instr_group::mem)
            {
                v = handle_i_implicit_shift(v,mem_shift[instr_entry.opcode]);
            }


            const auto success = encode_i_type_operand(v,s);

            // TODO add psuedo op that will encode ones too large
            // into several instrs
            if(!success)
            {
                die("cannot encode i type operand: %d\n",v);
            }
        
            static constexpr int32_t reg_field_table[9] = 
            {
                DST_OFFSET, // ARITH
                DST_OFFSET, // LOGIC
                SRC1_OFFSET, // CMP
                SRC1_OFFSET, // REG_BRANCH
                SRC1_OFFSET, // MEM
                DST_OFFSET, // MOV
                -1, // SHIFT
                -1, // CNT
                -1 // REL_BRANCH
            };

            const auto operand_shift = reg_field_table[static_cast<int>(instr_entry.group)];

            if(operand_shift == -1)
            {
                die("i type 2 operand group unhandled: %d\n",static_cast<int>(instr_entry.group));
            }

            opcode |= reg << operand_shift
                | v | (s << 16); // encode imm and shift
            
            break;
        }

        case 3:
        {
            auto root = ast.root->right;

            const auto reg1 = read_op(root,operand_type::reg);
            const auto reg2 = read_op(root,operand_type::reg);
            auto v = read_op(root,operand_type::val);
            uint32_t s = 0;


            if(instr_entry.group == instr_group::mem)
            {
                v = handle_i_implicit_shift(v,mem_shift[instr_entry.opcode]);
            }

            if(instr_entry.group != instr_group::shift)
            {
                const auto success = encode_i_type_operand(v,s);
                // TODO add psuedo op that will encode ones too large
                // into several instrs
                if(!success)
                {
                    die("cannot encode i type operand: %d\n",v);
                }

            }

            // for shifts we only want the shift field and to ignore the imm
            else
            {
                s = v;
                v = 0;

                if(s > 0xf)
                {
                    die("shift imm: out of range shift: %d\n",s);
                }
            }

            opcode |= reg1  << DST_OFFSET // dst
                | reg2 << SRC1_OFFSET // src
                | v | (s << 16); // encode imm and shift

            break;
        }

        default:
        {
            die("imm unhandled operand len %d\n",instr_entry.operand_count);
        }
    }

    return opcode;
}

// atm this is only used for mov in the isa spec
uint32_t Assembler::decode_m_instr(const Instr &instr_entry,const std::vector<Token> &tokens)
{
    uint32_t opcode = 0;


    Ast ast(tokens);

    if(ast.operands != instr_entry.operand_count)
    {
        die("s type instr: expected %d operands but got: %zd\n",instr_entry.operand_count,ast.operands);
    }

    switch(instr_entry.operand_count)
    {

        case 2:
        {
            auto root = ast.root->right;
            const auto reg = read_op(root,operand_type::reg);

            const uint32_t v = read_op(root,operand_type::val);

            // max 16 bit unsigned imm
            if(v >= set_bit(0,16))
            {
                die("cannot encode m type operand: %d\n",v);
            }

            // lower 12 bit at bottom of op
            // upper 4 where src2 field normally is
            opcode |= reg << DST_OFFSET |
                (v & 0x0fff) | (((v & 0xf000) >> 12) << SRC2_OFFSET);  

            break;
        }

         default:
        {
            die("m unhandled operand len %d\n",instr_entry.operand_count);
        }       
    }

    return opcode;
}

// currently only used for shifts
uint32_t Assembler::decode_f_instr(const Instr &instr_entry,const std::vector<Token> &tokens)
{
    Ast ast(tokens);

    if(ast.operands != instr_entry.operand_count)
    {
        die("s type instr: expected %d operands but got: %zd\n",instr_entry.operand_count,ast.operands);
    }

    uint32_t opcode = 0;

    switch(instr_entry.operand_count)
    {

        case 4:
        {

            auto root = ast.root->right;
            const auto reg1 = read_op(root,operand_type::reg);
            const auto reg2 = read_op(root,operand_type::reg);
            const auto reg3 = read_op(root,operand_type::reg);
            const int32_t v = read_op(root,operand_type::val);

        
            // dest = op1, src1 = op2, src2 = op3
            opcode |= (reg1 << DST_OFFSET) | 
                (reg2 << SRC1_OFFSET) |
                (reg3 << SRC2_OFFSET) |
                v << 8; // rshift 8 bits in                       
        
            break;
        }

        default:
        {
            die("m unhandled operand len %d\n",instr_entry.operand_count);           
            break;
        }
    }



    return opcode;
}


uint32_t Assembler::decode_s_instr(const Instr &instr_entry,const std::vector<Token> &tokens)
{
    uint32_t opcode = 0;

    Ast ast(tokens);

    if(ast.operands != instr_entry.operand_count)
    {
        die("s type instr: expected %d operands but got: %zd\n",instr_entry.operand_count,ast.operands);
    }

    switch(instr_entry.operand_count)
    {
        // dont know 1 operand is consistant
        case 1: // dest = op1, everything else zero
        {
            auto root = ast.root->right;
            const auto reg = read_op(root,operand_type::reg);

            opcode |= reg << DST_OFFSET;
            break;
        }

        // register - register encoding is not consistant across groups for
        // 2 operands...
        // for now we will do this with a switch but it would be nicer to encode this
        // with a lut
        case 2: 
        {

            static constexpr int32_t reg_field_table[9][2] = 
            {
                {DST_OFFSET,SRC2_OFFSET}, // ARITH
                {DST_OFFSET, SRC1_OFFSET}, // LOGIC
                {SRC1_OFFSET,SRC2_OFFSET}, // CMP
                {SRC1_OFFSET,SRC2_OFFSET}, // REG_BRANCH
                {-1,-1}, // MEM
                {DST_OFFSET,SRC1_OFFSET}, // MOV
                {-1,-1}, // SHIFT
                {DST_OFFSET,SRC1_OFFSET}, // CNT
                {-1,-1} // REL_BRANCH
            };

            const auto operand1 = reg_field_table[static_cast<int>(instr_entry.group)][0];
            const auto operand2 = reg_field_table[static_cast<int>(instr_entry.group)][1];

            if(operand1 == -1)
            {
                die("s type 2 operand group unhandled: %d\n",static_cast<int>(instr_entry.group));               
            }

            auto root = ast.root->right;
            const auto reg1 = read_op(root,operand_type::reg);
            const auto reg2 = read_op(root,operand_type::reg);

            opcode |= (reg1 << operand1) | (reg2 << operand2);
            break;
        }

        case 3: // dest = op1, src1 = op2, src2 = op3
        {

            auto root = ast.root->right;
            const auto reg1 = read_op(root,operand_type::reg);
            const auto reg2 = read_op(root,operand_type::reg);
            const auto reg3 = read_op(root,operand_type::reg);

            opcode |= (reg1 << DST_OFFSET) |  (reg2 << SRC1_OFFSET) | (reg3 << SRC2_OFFSET);                        
            break;
        }

        default: 
        {
            printf("S type unhandled operand count %d\n",instr_entry.operand_count);
            break;
        }
    }    

    return opcode;
}

uint32_t Assembler::decode_b_instr(const Instr &instr_entry,const std::vector<Token> &tokens)
{   
    UNUSED(instr_entry);

    uint32_t opcode = 0;

    Ast ast(tokens);

    if(ast.operands != instr_entry.operand_count)
    {
        die("s type instr: expected %d operands but got: %zd\n",instr_entry.operand_count,ast.operands);
    }

    
    auto root = ast.root->right;
    const auto v = read_op(root,operand_type::val);

    const auto sign = is_set(v,(sizeof(v)*8)-1);
    const int32_t branch =  (v >> 2) & (set_bit(0,24) - 1);        
    const int32_t final_branch = sign? set_bit(branch,23) : deset_bit(branch,23);

    //24 bit imm added to pc to reach target
    // left shifed by two then sign extended
    if(abs(v) > set_bit(0,22))
    {
        die("cannot represent relative branch in 26 bit signed op: %x\n",v);
    }

    opcode |= final_branch;

    return opcode;
}