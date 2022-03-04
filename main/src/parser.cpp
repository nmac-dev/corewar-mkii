
#include "parser.hpp"
#include "file_loader.hpp"

namespace parser
{
inline static bool isNumber(char& value)
{
    bool is_number = false;
    if (value >= '0' && value <= '9') is_number = true;

    return is_number;
}

inline static bool isInlineSpace(char& value)
{
    bool is_space = false;
    if (value == ' ' || value == '\t') is_space = true;

    return is_space;
}

/** Processing Example:
 *   Input: " start   mov.ab   #4,  bmb    "
 *  Output: "start mov.ab #4,bmb"
 */
static void cleanAsmStr(std::string &str)
{
    const int len = str.length() -1; // fetch once
    int l = 0, r = 0; // l : left index, r : right index
    int logic_p  = 0; // seperator for logical partition, used to erase excess string length
    
    while (l < len && r < len)
    {
        for (l = r;  isInlineSpace(str[l]) && l < len; l++) {} // start
        for (r = l; !isInlineSpace(str[r]) && r < len; r++)    // end
        {
            if(str[r] == ',') break;
        }
        
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
    while(isInlineSpace(str[r--])) str.pop_back();
}
} /// namespace parser