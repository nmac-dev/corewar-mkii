
#include "parser.hpp"

namespace parser
{
inline static bool isNumber(char& val)
{
    bool is_number = false;
    if (val >= '0' && val<= '9') is_number = true;

    return is_number;
}

inline static bool isAsmSeperator(char& val)
{
    bool is_space = false;
    if (val == ' ' || val == '\t' || val == '.' || val == ',') is_space = true;

    return is_space;
}

inline static void findAsmArgument(std::string &str, int &l, int &r)
{
    /// find start & end of argument
    for (l = r;  isAsmSeperator(str[l]) && l < str.length() -1; l++) {} // start
    for (r = l; !isAsmSeperator(str[r]) && r < str.length() -1; r++) {}  // end
} 

static void cleanAsmStr(std::string &str)
{
    /* Processing Example:
     *   Input: " start   mov.ab   #4,  bmb    "
     *  Output: "start mov.ab #4,bmb"
     */

    const int len = str.length() -1; // fetch once
    int l = 0, r = 0; // l : left index, r : right index
    int logic_p  = 0; // seperator for logical partition, used to erase excess string length
    
    while (l < len && r < len)
    {
        findAsmArgument(str, l, r);
        
        /// swaps the indexes from the left index to the end of the logical partition
        while (l <= r)
        {
            // only swap index if necessary
            if (logic_p != l)
            {
                str[logic_p] = str[l];
                str[l]       = ' ';
            }
            l++; logic_p++; // inc both
        }
        // remove extra space after comma of [op_a] & [op_b]
        if(str[r] == ',') r++;
    }
    // erase end partition
    while(isAsmSeperator(str[r--])) str.pop_back();
}

static Instr *strToInstr(std::string &str, std::unordered_map<std::string, int> &label_linker)
{
    Instr *asm_instr = new Instr();
    std::string asm_arg; asm_arg.reserve(32);

    const int len = str.length() -1; // fetch once
    bool no_modifier = false;        // flag to notify function to create modifier
    int l = 0, r = 0; // l : left index, r : right index

    // clean instruction string
    cleanAsmStr(str);

    while (l < len && r < len)
    {
        // get assembly argument
        findAsmArgument(str, l, r);
        asm_arg = str.substr(l, r - l);

        /* identify argument */

        // <label>
        if (l = 0 && !opcode_tbl.count(asm_arg))
        {
            /// TODO: process label
        }

        // [opcode]
        if (opcode_tbl.count(asm_arg))
        {
            // add [opcode]
            asm_instr->opcode = (_OP *)opcode_tbl.at(asm_arg);

            // check [opcode] has <modifier>
            if (str[r] == '.')
            {
                // get <modifier> argument 
                findAsmArgument(str, l, r);
                asm_arg = str.substr(l, r - l);

                // add <modifier>
                if (modifier_tbl.count(asm_arg))
                {
                    asm_instr->opcode = (_OP *)opcode_tbl.at(asm_arg);
                }
                // not valid
                else 
                {
                    std::cerr
                        << "Error: " << asm_arg << " is not a valid <modifier> . . .\n"
                        << "Full Instruction: |" << str << "|"
                        << std::endl;
                    throw std::exception(); // begin stack unwind to main()
                }
            }
            // set flag to create default <modifier> at the end (depends on all other arguments)
            else
            {
                no_modifier = true;
            }
        }
        // <mode_a|b> & [op_a|b]
        if (str[r] == ',' || r == len)
        {
            // process <mode_a> on first run, then <mode_b> on second run
            if (adr_mode_tbl.count(str[l]))
            {
                if (str[r] == ',')
                {
                    asm_instr->mode_a = (_AM *)adr_mode_tbl.at(str[l]);
                }
                else asm_instr->mode_b = (_AM *)adr_mode_tbl.at(str[l]);
                
                // l++; // inc left index to [op_a|b] argument
            }
            // no <mode_a|b>, add default
            else 
            {
                if (str[r] == ',')
                {
                    asm_instr->mode_a = (_AM *)adr_mode_tbl.at('$');
                }
                else asm_instr->mode_b = (_AM *)adr_mode_tbl.at('$');
            }

            // process [op_a] on first run, then [op_b] on second run
            if (isNumber(str[l]))
            {
                if (str[r] == ',')
                {
                    asm_instr->op_a  = stoi(str.substr(l, r - l));
                }
                else asm_instr->op_b = stoi(str.substr(l, r - l)); 
            }
            // options exhausted, assume [op_a|b] is <label> reference
            else
            {
                /// FIX: process label_linker before 
            }
        }
    } /// while (..)
} /// strToInstr()
} /// namespace parser
