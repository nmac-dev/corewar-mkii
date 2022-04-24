
#include "parser.hpp"

namespace Parser
{

std::string cleanAsmStr(std::string line)
{
    std::string clean_asm = "";
    int pos = 0;

    // filter
    while (pos++ < line.size())
    {
        // to lowercase
        if (line[pos] >= 'A' && line[pos] <= 'Z') line[pos] = line[pos] - ('Z' - 'z');
        // remove comment
        if (line[pos] == ASM_CDOE_COMMENT) line.erase(pos);
    }

    // build clean asm lineing
    pos = 0;
    while (pos < line.length())
    {    
        clean_asm.append( findAsmArgument(line, pos) );
        
        // retain dot for modifier argument
        if (line[pos] == '.')
        {
            clean_asm.push_back('.');
            continue;
        }
        // skip space after comma of [op_a] & [op_b]
        if(line[pos] == ',') {
            clean_asm.push_back(',');
            continue;
        } 
        clean_asm.push_back(' ');
    }
    // erase end partition
    pos = clean_asm.length() -1;
    while(isAsmSeperator(clean_asm[pos--])) clean_asm.pop_back();
    
    return clean_asm;
}

MOD getDefaultModifier(OPCODE opcode, ADMO admo_a, ADMO admo_b)
{
    switch (opcode)
    {
        // [dat]
        case OPCODE::DAT: return MOD::F;
            
        // [mov, seq, sne]
        case OPCODE::MOV:
        case OPCODE::SEQ:
        case OPCODE::SNE:
        // [add, sub, mul, div, mod]
        case OPCODE::ADD:
        case OPCODE::SUB:
        case OPCODE::MUL:
        case OPCODE::DIV:
        case OPCODE::MOD:
            // A == '#'
            if (admo_a == ADMO::IMMEDIATE)
            {
                return MOD::AB;
            }
            // A != ['#','*'] && B == '#' 
            else if (admo_a != ADMO::IMMEDIATE && admo_b == ADMO::IMMEDIATE)
            {
                return MOD::B;
            }
            else
            {   // default: [mov, seq, sne]
                switch (opcode)
                {
                    case OPCODE::MOV:
                    case OPCODE::SEQ:
                    case OPCODE::SNE: 
                        return MOD::I;
                }
                // default: [add, sub, mul, div, mod]
                return MOD::F;
            }
        // [slt]
        case OPCODE::SLT:
            // A == '#'
            if (admo_a == ADMO::IMMEDIATE)
            {
                return MOD::AB;
            } // else B

        // [jmp, jmz, jmn, djn, spl]
        case OPCODE::JMP:
        case OPCODE::JMZ:
        case OPCODE::JMN:
        case OPCODE::DJN:
        case OPCODE::SPL:
            default: return MOD::B;
    }
} // getDefaultModifier()

LabelLinker findAsmLabels(AssemblyCode &asm_data)
{
    LabelLinker labels;
    std::string first_arg; // first argument in asm code

    // search each line of asm code
    for (int i = 0; i < asm_data.size(); i++)
    {
        int pos = 0;
        first_arg = findAsmArgument(asm_data[i], pos);

        // label found, as first_arg is not opcode
        if (!opcode_tbl.count(first_arg))
        {
            // true if label starts with ('a-z', '0-9' or '_')
            bool is_alphanumeric = first_arg[0] >= 'a' && first_arg[0] <= 'z' ||
                                   first_arg[0] >= '0' && first_arg[0] <= '9' ||
                                   first_arg[0] == '_';

            // add label to linker with line position
            if (is_alphanumeric)
            {
                labels[first_arg] = i;
            }
        }
    }
    return labels;
}

Inst asmStrToInst(std::string &line, LabelLinker &linker, int index)
{    
        /* Inst members */
    Inst::Operation OP_;
    Inst::Operand A_, B_;

        /* Asm lineing */
    std::string asm_arg;      // stores assembly code arguments
    bool no_modifier = false; // flag to notify function to create modifier
    int pos = 0;              // tracks asm lineing processing position

    // set mode & value to operand A until B is found
    ADMO *admo_ptr = &A_.admo;
    int  *val_ptr  = &A_.val;

    // get assembly argument
    asm_arg = findAsmArgument(line, pos);

        /* identify argument */

    // skip <label>
    if (linker.count(asm_arg))
    {
        // get [code]
        asm_arg = findAsmArgument(line, pos);
    }
    
    // [code]
    if (opcode_tbl.count(asm_arg))
    {
        // add [code]
        OP_.code = opcode_tbl.at(asm_arg);

        // check [code] has <mod>
        if (line[pos] == '.')
        {
            // get <mod> argument 
            asm_arg = findAsmArgument(line, pos);

            // add <mod>
            if (mod_tbl.count(asm_arg))
            {
                OP_.mod = mod_tbl.at(asm_arg);

                // opcode modifier override
                switch (OP_.code)
                {
                /* Ignored */
                case OPCODE::DAT: OP_.mod = MOD::F; break;
                case OPCODE::JMP:
                case OPCODE::SPL: OP_.mod = MOD::B; break;
                /* Filtered */
                case OPCODE::JMZ:
                case OPCODE::JMN:
                case OPCODE::DJN:
                    switch (OP_.mod) // correct: compare jumps modifier
                    {
                    case MOD::AB: OP_.mod = MOD::B; break;
                    case MOD::BA: OP_.mod = MOD::A; break;
                    case MOD::X:
                    case MOD::I:  OP_.mod = MOD::F; break;
                    }
                }
            }
            else invalidAssemblyError(line, asm_arg, index); // invalid <mod>
        }
        // set flag to create default <mod> at the end (depends on all other arguments)
        else no_modifier = true;

        // loop over both <mode>[operand]
        while (pos < line.length())
        {
            // get <mode>[operand]
            asm_arg = findAsmArgument(line, pos);
            
            // process <mode>
            if (admo_tbl.count(asm_arg[0]))
            {
                *admo_ptr = admo_tbl.at(asm_arg[0]);

                // remove <mode> from argument
                asm_arg = asm_arg.substr(1);
            }
            // no <mode>, add default
            else 
            {
                if (OP_.code == OPCODE::DAT) *admo_ptr = admo_tbl.at('#');
                else                         *admo_ptr = admo_tbl.at('$');
            }

            // process [operand]
            if (asm_arg[0] >= '0' && asm_arg[0]<= '9' || asm_arg[0] == '-')
            {
                *val_ptr = stoi(asm_arg);
            }
            // options exhausted, assume [operand] is <label> reference
            else
            {
                // get label position from linker
                if (linker.count(asm_arg))
                {
                    int lbl_i    = linker.at(asm_arg); // label index 

                    // calculate labels relative reference for operand index
                    if      (lbl_i < index) lbl_i -= index;
                    else if (lbl_i > index) lbl_i  = std::abs(index - lbl_i);
                    
                    *val_ptr = lbl_i;
                }
                else invalidAssemblyError(line, asm_arg, index); // invalid [operand]
            }
            // move from <mode>[operand] A -> B
            admo_ptr = &B_.admo;
            val_ptr  = &B_.val;
        }
        // get default <modifer>
        if (no_modifier)
        {
            OP_.mod = getDefaultModifier(OP_.code, A_.admo, B_.admo);
        }
    }
    else invalidAssemblyError(line, line.substr(0, pos), index); // invalid [code]

    return Inst(OP_, A_, B_);
} // asmStrToInst()

Warrior *asmCodeToWarrior(std::string warrior_name, AssemblyCode &asm_code, int _max_warrior_len)
{
    LabelLinker linker;            // stores label positions
    int _length = asm_code.size();  // number of warrior instruction

    // validate number of instructions is within configuration bounds
    if (_length > _max_warrior_len)
    {
        printf("Warning: '%s' has a length greater than the max (%d) and will be truncated."
                "\n\tEdit corewar config file to increase '_max_warrior_len'\n",
                warrior_name.c_str(), _max_warrior_len
        );
        // truncate length
        _length = _max_warrior_len;
    }

    // construct warrior w/ default arguments
    Warrior *warrior = new Warrior(warrior_name.c_str(), _length, _max_warrior_len);

    // clean asm code to correct format
    for (int i = 0; i < _length; i++)
    {
        asm_code[i] = cleanAsmStr(asm_code[i]);

        #ifdef ASM_PARSER_DEBUG
        if (i == 0) printf("\nparser::cleanAsmStr: \n");
        printf("\t |%s| \n", asm_code[i].c_str());
        #endif
    }

    // find all labels within the asm code
    linker = findAsmLabels(asm_code);

    #ifdef ASM_PARSER_DEBUG
    printf("\nparser::findAsmLabels: found %d\n", linker.size());
    for (auto itr = linker.begin(); itr != linker.end(); itr++)
    {
        printf("\t [%d]|%s| \n", itr->second, itr->first.c_str());
    }
    #endif

    // parse all asm code lineing instructions to a instruction object (Inst)
    for (int i = 0; i < _length; i++)
    {
        // replace default warrior instructions
        (*warrior).push(asmStrToInst(asm_code[i], linker, i)); 

        #ifdef ASM_PARSER_DEBUG
        if (i == 0) printf("\nparser::asmStrToInst: '%s'\n" , warrior_name.c_str());
        printf(" line:%d \n\t I::|%s| \n\t O::|[%d].<%d> <%d>[%d] <%d>[%d]|\n",
                i + 1,
                asm_code[i].c_str(),
                (*warrior)[i].OP.code,(*warrior)[i].OP.mod,
                (*warrior)[i].A.admo, (*warrior)[i].A.val,
                (*warrior)[i].B.admo, (*warrior)[i].B.val);
        #endif
    }
    return warrior;
} // asmCodeToWarrior()

} // namespace parser
