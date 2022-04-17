
#include "parser.hpp"

namespace Parser
{

namespace //// Anonymous Namespace
{

    /* Parser Utility Functions */

inline void invalidAssemblyError(std::string line, std::string asm_arg, int index)
{
    printf("\nError: '%s' is not valid argument" "\n[Line Number]|Full Instruction|: [%d]|%s|",
            asm_arg.c_str(), index, line.c_str());
    throw std::exception(); // begin stack unwind
}

inline bool isAsmSeperator(char val)
{
    return val == ' ' || val == '\t' || val == '.' || val == ',';
}

inline std::string findAsmArgument(std::string line, int &pos)
{
    int len = line.length();
    int begin, end;

    for (begin = pos;  isAsmSeperator(line[begin]) && begin < len; begin++) {} // start of arg
    for (end = begin; !isAsmSeperator(line[end])   &&   end < len; end++  ) {} // end of arg

    pos = end; // retain index value for next argument
    return line.substr(begin, end - begin);
}

inline std::string cleanAsmStr(std::string line)
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

inline _MOD getDefaultModifier(_OP opcode, _AM admo_a, _AM admo_b)
{
    switch (opcode)
    {
        // [dat, nop]
        case _OP::DAT:
            return _MOD::F;
            
        // [mov, cmp, sne]
        case _OP::MOV:
        case _OP::CMP:
        // [add, sub, mul, div, mod]
        case _OP::ADD:
        case _OP::SUB:
        case _OP::MUL:
        case _OP::DIV:
        case _OP::MOD:
            // A == '#'
            if (admo_a == _AM::IMMEDIATE)
            {
                return _MOD::AB;
            }
            // A != ['#','*'] && B == '#' 
            else if (admo_a != _AM::IMMEDIATE && admo_b == _AM::IMMEDIATE)
            {
                return _MOD::B;
            }
            // default: [mov, cmp]
            else if (opcode == _OP::MOD, opcode == _OP::CMP)
            {
                return _MOD::I;
            }
            // default: [add, sub, mul, div, mod]
            else
            {
                return _MOD::F;
            }

        // [slt]
        case _OP::SLT:
            // A == '#'
            if (admo_a == _AM::IMMEDIATE)
            {
                return _MOD::AB;
            }
            // A != '#'
            else
            {
                return _MOD::B;
            }
            
        // [jmp, jmz, jmn, djn, spl]
        case _OP::JMP:
        case _OP::JMZ:
        case _OP::JMN:
        case _OP::DJN:
        case _OP::SPL:
            default: return _MOD::B;
    }
} //// getDefaultModifier()

} //// Anonymous Namespace

    /* Parser Functions */

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
    _OP  opcode;                                     // specifies operation
    _MOD modifier;                                   // modifies opcode behaviour
    _AM  admo_a = _AM::DIRECT, admo_b = _AM::DIRECT; // addressing mode for operands (A|B)
    int  operand_a = 0,        operand_b = 0;        // operand (A|B) of the opcode argument

        /* Asm lineing */
    std::string asm_arg;      // stores assembly code arguments
    bool no_modifier = false; // flag to notify function to create modifier
    int pos = 0;              // tracks asm lineing processing position

    // set mode & value to operand A until B is found
    _AM *admo_ptr    = &admo_a;
    int *operand_ptr = &operand_a;

    // get assembly argument
    asm_arg = findAsmArgument(line, pos);

        /* identify argument */

    // skip <label>
    if (linker.count(asm_arg))
    {
        // get [opcode]
        asm_arg = findAsmArgument(line, pos);
    }
    
    // [opcode]
    if (opcode_tbl.count(asm_arg))
    {
        // add [opcode]
        opcode = opcode_tbl.at(asm_arg);

        // check [opcode] has <modifier>
        if (line[pos] == '.')
        {
            // get <modifier> argument 
            asm_arg = findAsmArgument(line, pos);

            // add <modifier>
            if (modifier_tbl.count(asm_arg))
            {
                modifier = modifier_tbl.at(asm_arg);
            }
            else invalidAssemblyError(line, asm_arg, index); // invalid <modifier>
        }
        // set flag to create default <modifier> at the end (depends on all other arguments)
        else
        {
            no_modifier = true;
        }

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
                *admo_ptr = admo_tbl.at('$');
            }

            // process [operand]
            if (asm_arg[0] >= '0' && asm_arg[0]<= '9' || asm_arg[0] == '-')
            {
                *operand_ptr = stoi(asm_arg);
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
                    
                    *operand_ptr = lbl_i;
                }
                else invalidAssemblyError(line, asm_arg, index); // invalid [operand]
            }
            // move from <mode>[operand] A -> B
            admo_ptr    = &admo_b;
            operand_ptr = &operand_b;
        }
        // get default <modifer>
        if (no_modifier)
        {
            modifier = getDefaultModifier(opcode, admo_a, admo_b);
        }
    }
    else invalidAssemblyError(line, line.substr(0, pos), index); // invalid [opcode]

    return Inst(opcode, modifier, admo_a, operand_a, admo_b, operand_b);
} //// asmStrToInst()

Warrior asmCodeToWarrior(std::string warrior_name, AssemblyCode &asm_code, int max_warrior_len)
{
    LabelLinker linker;            // stores label positions
    int n_inst = asm_code.size();  // number of warrior instruction

    // validate number of instructions is within configuration bounds
    if (n_inst > max_warrior_len)
    {
        printf("Warning: '%s' has a length greater than the max (%d) and will be truncated."
                "\n\tEdit corewar config file to increase 'max_warrior_len'\n",
                warrior_name.c_str(), max_warrior_len
        );
        // truncate length
        n_inst = max_warrior_len;
    }

    // construct warrior w/ default arguments
    Warrior warrior = Warrior(warrior_name.c_str(), new Inst[n_inst], n_inst);

    // clean asm code to correct format
    for (int i = 0; i < n_inst; i++)
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
    for (int i = 0; i < n_inst; i++)
    {
        // replace default warrior instructions
        warrior[i] = asmStrToInst(asm_code[i], linker, i); 

        #ifdef ASM_PARSER_DEBUG
            if (i == 0) printf("\nparser::asmStrToInst: '%s'\n" , warrior_name.c_str());
            printf(" line:%d \n\t I::|%s| \n\t O::|[%d].<%d> <%d>[%d] <%d>[%d]|\n",
                    i + 1,
                    asm_code[i].c_str(),
                    warrior[i].opcode, warrior[i].modifier,
                    warrior[i].admo_a, warrior[i].operand_a,
                    warrior[i].admo_b, warrior[i].operand_b
            );
        #endif
    }
    return warrior;
} //// asmCodeToWarrior()

} //// namespace parser
