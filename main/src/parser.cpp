
#include "parser.hpp"

namespace parser
{

namespace /**** Anonymous Namespace ****/
{

    /**** Parser Utility Functions ****/

inline void invalidAssemblyError(std::string full_inst, std::string asm_arg)
{
    std::cerr
        << "Error: " << asm_arg << " is not valid argument\n"
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
    const int len = str.length();
    int l, r;

    // start of arg
    for (l = pos; isAsmSeperator(str[l]) && l < len; l++) {}
    // end of arg
    for (r = l; !isAsmSeperator(str[r]) && r < len; r++) {}

    pos = r; // retain index value for next argument
    return str.substr(l, r - l);
}


//****      Fix: Lower/Upper case       ****/
inline std::string cleanAsmStr(std::string str)
{
    /* Processing Example:
    *   Input: " start   mov.ab   #4,  bmb    "
    *  Output: "start mov.ab #4,bmb"
    */
    std::string clean_asm = "";
    int pos = 0;
    
    while (pos < str.length())
    {
        clean_asm.append( findAsmArgument(str, pos) );
        
        // retain dot for modifier argument
        if (str[pos] == '.')
        {
            clean_asm.push_back('.');
            continue;
        }
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
}

} /// Anonymous Namespace


    /**** Parser Functions ****/

LabelLinker findAsmLabels(std::vector<std::string> &asm_data)
{
    LabelLinker labels;
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
            labels[first_arg] = i;
        }
    }
    return labels;
}

Inst asmStrToInst(std::string &str, std::unordered_map<std::string, int> &linker)
{    
    _OP  opcode;               // Specifies operation
    _MOD modifier;             // Modifies opcode behaviour
    _AM  admo_a, admo_b;       // Addressing mode for operands (A|B)
    int  operand_a, operand_b; // Operand (A|B) of the opcode argument

    std::string asm_arg;      // stores assembly code arguments
    bool no_modifier = false; // flag to notify function to create modifier
    int pos = 0;              // tracks asm string processing position

    // set mode & value to operand A until B is found
    _AM *admo_ptr    = &admo_a;
    int *operand_ptr = &operand_a;

    // get assembly argument
    asm_arg = findAsmArgument(str, pos);

    /* identify argument */

    // skip <label>
    if (linker.count(asm_arg))
    {
        // get [opcode]
        asm_arg = findAsmArgument(str, pos);
    }
    
    // [opcode]
    if (opcode_tbl.count(asm_arg))
    {
        // add [opcode]
        opcode = opcode_tbl.at(asm_arg);

        // check [opcode] has <modifier>
        if (str[pos] == '.')
        {
            // get <modifier> argument 
            asm_arg = findAsmArgument(str, pos);

            // add <modifier>
            if (modifier_tbl.count(asm_arg))
            {
                modifier = modifier_tbl.at(asm_arg);
            }
            else 
            {
                invalidAssemblyError(str, asm_arg);
            }
        }
        // set flag to create default <modifier> at the end (depends on all other arguments)
        else
        {
            no_modifier = true;
        }

        // loop over both <mode>[operand]
        while (pos < str.length())
        {
            // get <mode>[operand]
            asm_arg = findAsmArgument(str, pos);
            
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
                    *operand_ptr = linker.at(asm_arg);
                }
                else 
                {
                    invalidAssemblyError(str, asm_arg);
                }
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
    else
    {
        invalidAssemblyError(str, asm_arg);
    }

    return Inst(opcode, modifier, admo_a, operand_a, admo_b, operand_b);
} /// strToInstr()

Warrior asmFileToWarrior(std::string warrior_name)
{
    std::vector<std::string> asm_code = file_loader::getFileData(warrior_name, ';');
    const int max_warrior_len = Settings::get().max_warrior_len(); // .ini warrior max length
    int n_inst = asm_code.size();  // number of warrior instruction
    LabelLinker linker;            // stores label positions

    // validate number of instructions is within configuration bounds
    if (n_inst > max_warrior_len)
    {
        std::cerr 
        << "Warning: '" << warrior_name << "' has a length greater than the max ("
        << max_warrior_len << ") and will be truncated.\n"
        << "Edit |" << Settings::get().file_name() << "| to increase 'max_warrior_len'";
        // truncate length
        n_inst = max_warrior_len;
    }

    // construct warrior w/ default arguments
    Warrior warrior = Warrior(warrior_name.c_str(), new Inst[n_inst],n_inst);

    // clean asm code to correct format
    for (int i = 0; i < n_inst; i++)
    {
        asm_code[i] = cleanAsmStr(asm_code[i]);

        #ifdef ASM_PARSER_DEBUG
            std::cout << "parser::cleanAsmStr: |" << asm_code[i] << '|' << std::endl;
        #endif
    }

    // find all labels within the asm code
    linker = findAsmLabels(asm_code);

    #ifdef ASM_PARSER_DEBUG
        for (auto itr = linker.begin(); itr != linker.end(); itr++)
        {
            std::cout
                << "parser::findAsmLabels: \t" 
                << "Key= \""    << itr->first << "\""
                << " \tValue= " << itr->second
                << std::endl;
        }
    #endif

    // parse all asm code string instructions to a instruction object (Inst)
    for (size_t i = 0; i < n_inst; i++)
    {
        // replace default warrior instructions
        warrior[i] = asmStrToInst(asm_code[i], linker); 

        #ifdef ASM_PARSER_DEBUG
            std::cout
                << "parser::asmStrToInst: \nIn: |" << asm_code[i] 
                << "|\nOut: _OP::" << (int)warrior[i].opcode 
                << "._MOD::"       << (int)warrior[i].modifier 
                << " _AM::"  << (int)warrior[i].admo_a << " op_A::" << warrior[i].operand_a
                << ", _AM::" << (int)warrior[i].admo_b << " op_B::" << warrior[i].operand_b
                << std::endl;
        #endif
    }

    return warrior;
}

} /// namespace parser
