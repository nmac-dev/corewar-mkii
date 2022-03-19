
#include "parser.hpp"

namespace parser
{


namespace /**** Anonymous Namespace ****/
{

    /**** Parser Utility Functions ****/

inline void invalidAssemblyError(std::string full_inst, std::string asm_arg)
{
    std::cerr
        << "Error: " << asm_arg << " is not a valid [operand] . . .\n"
        << "Full Instruction: |" << full_inst << "|"
        << std::endl;
    throw std::exception(); // begin stack unwind to main()
}

inline bool isAsmSeperator(char val)
{
    return val == ' ' || val == '\t' || val == '.' || val == ',';
}

inline std::string findAsmArgument(std::string str, int &pos)
{
    const int len = str.length() -1;
    int l, r;

    // start of arg
    for (l = pos; isAsmSeperator(str[l]) && l < len; l++) {}
    // end of arg
    for (r = l; !isAsmSeperator(str[r]) && r < len; r++) {}

    pos = r; // retain index value for next argument
    return str.substr(l, r - l);
}

inline std::string cleanAsmStr(std::string str)
{
    /* Processing Example:
    *   Input: " start   mov.ab   #4,  bmb    "
    *  Output: "start mov.ab #4,bmb"
    */
    std::string clean_asm = "";

    const int len = str.length() -1; // fetch once
    int pos = 0;
    
    while (pos < len)
    {
        clean_asm.append( findAsmArgument(str, pos) );
        std::cout << clean_asm << "|\n";
        
        // skip space after comma of [op_a] & [op_b]
        if(str[pos] == ',') {
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

inline _MOD &getDefaultModifier(_OP opcode, _AM mode_a, _AM mode_b)
{
    switch (opcode)
    {
    // [dat, nop]
    case _OP::DAT:
        return modifier_tbl["f"];
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
        if (mode_a == _AM::IMMEDIATE)
        {
            return modifier_tbl["ab"];
        }
        // A != ['#','*'] && B == '#' 
        else if (mode_a != _AM::IMMEDIATE && mode_b == _AM::IMMEDIATE)
        {
            return modifier_tbl["b"];
        }
        // default: [mov, cmp]
        else if (opcode == _OP::MOD, opcode == _OP::CMP)
        {
            return modifier_tbl["i"];
        }
        // default: [add, sub, mul, div, mod]
        else
        {
            return modifier_tbl["f"];
        }
    // [slt]
    case _OP::SLT:
        // A == '#'
        if (mode_a == _AM::IMMEDIATE)
        {
            return modifier_tbl["ab"];
        }
        // A != '#'
        else
        {
            return modifier_tbl["b"];
        }
    // [jmp, jmz, jmn, djn, spl]
    case _OP::JMP:
    case _OP::JMZ:
    case _OP::JMN:
    case _OP::DJN:
    case _OP::SPL:
        return modifier_tbl["b"];
    }
}

} /// Anonymous Namespace


    /**** Parser Functions ****/

LabelLinker *getAsmLabels(std::vector<std::string> &asm_data)
{
    LabelLinker *labels = new LabelLinker;
    std::string first_arg; // first argument in asm code

    // Search each line of asm code
    for (int i = 0; i < asm_data.size(); i++)
    {
        int pos = 0;
        first_arg = findAsmArgument(asm_data[i], pos);

        // label found, as first_arg is not opcode
        if (!opcode_tbl.count(first_arg))
        {
            // add label to linker w/ line position
            (*labels)[first_arg] = i;
        }
    }
    return labels;
}

Inst *asmStrToInst(std::string &str, std::unordered_map<std::string, int> &label_linker)
{
    Inst *asm_inst = new Inst();
    std::string asm_arg;

    const int len = str.length() -1; // fetch once
    bool no_modifier = false;        // flag to notify function to create modifier
    int pos = 0;

    // set mode & value to operand a until b is found
    _AM **addr_mode = &asm_inst->mode_a;
    int *operand    = &asm_inst->op_a;

    // get assembly argument
    asm_arg = findAsmArgument(str, pos);

    /* identify argument */

    // skip <label>
    if (label_linker.count(asm_arg))
    {
        // get [opcode]
        asm_arg = findAsmArgument(str, pos);
    }
    

    // [opcode]
    if (opcode_tbl.count(asm_arg))
    {
        // add [opcode]
        asm_inst->opcode = (_OP *)opcode_tbl.at(asm_arg);

        // check [opcode] has <modifier>
        if (str[pos] == '.')
        {
            // get <modifier> argument 
            asm_arg = findAsmArgument(str, pos);

            // add <modifier>
            if (modifier_tbl.count(asm_arg))
            {
                asm_inst->opcode = (_OP *)opcode_tbl.at(asm_arg);
            }
            else 
            {
                invalidAssemblyError(str, "<modifier>");
            }
        }
        // set flag to create default <modifier> at the end (depends on all other arguments)
        else
        {
            no_modifier = true;
        }

        // loop over both [operand]
        while (pos < len)
        {
            // get <mode>[operand]
            asm_arg = findAsmArgument(str, pos);
            
            // process <mode>
            if (adr_mode_tbl.count(asm_arg[0]))
            {
                *addr_mode = &adr_mode_tbl.at(str[0]);

                // remove <mode> from argument
                asm_arg = asm_arg.substr(1);
            }
            // no <mode>, add default
            else 
            {
                *addr_mode= (_AM *)adr_mode_tbl.at('$');
            }

            // process [operand]
            if (asm_arg[0] >= '0' && asm_arg[0]<= '9')
            {
                *operand = stoi(asm_arg);
            }
            // options exhausted, assume [operand] is <label> reference
            else
            {
                // get label position from linker
                if (label_linker.count(asm_arg))
                {
                    *operand = label_linker.at(asm_arg);
                }
                else 
                {
                    invalidAssemblyError(str, "[operand]");
                }
            }
            // move from <mode>[operand] a -> b
            addr_mode = &asm_inst->mode_b;
            operand   = &asm_inst->op_b;
        }

        // get default <modifer>
        asm_inst->modifier = &getDefaultModifier(*asm_inst->opcode, *asm_inst->mode_a, *asm_inst->mode_b);
    }
    else
    {
        invalidAssemblyError(str, "[opcode]");
    }
} /// strToInstr()

} /// namespace parser
