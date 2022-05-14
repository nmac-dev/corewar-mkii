
#include "parser.hpp"

namespace Parser
{
std::string clean_assembly(std::string _data)
{
    std::string clean_code_ = "";
    int _pos   = 0,
        code_i = 0;

    // filter
    int comma_i   = _data.size() +1; // unreachable incase of single operand argument
    while (_pos++ < _data.size())
    {
        char &val = _data[_pos];

        // find colon
        if (val == ',')
            comma_i = _pos;
        // to lowercase
        if (is_between(val, 'A', 'Z'))
            val = val - ('Z' - 'z');
        // remove comment
        if (val == ASSEMBLY_COMMENT) 
            _data.erase(_pos);
    }

    // build clean code
    _pos = 0;
    while (_pos < _data.size())
    {
        clean_code_.append( find_argument(_data, _pos) );
        
        // retain dot for modifier argument
        if (_data[_pos] == '.')
        {
            clean_code_.push_back('.');
            continue;
        }

        // add comma [A]',' [B]
        if(_pos > comma_i) {
            clean_code_.insert(code_i, 1, ',');
            comma_i = _data.size() +1;
        }
        clean_code_.push_back(' ');
        code_i = clean_code_.size() -1;
    }
    // erase end partition
    while(is_seperator(clean_code_[code_i--]))
        clean_code_.pop_back();
    
    #ifdef ASM_PARSER_DEBUG
    printf("\nparser::clean_assembly(): |%s| \n", clean_code_.c_str());
    #endif

    return clean_code_;
} /* ::clean_assembly() */


LabelLinker generate_label_linker(AssemblyCode &assembly)
{
    LabelLinker linker_;
    std::string first_arg; // first argument in asm code

    // search each line of asm code
    for (int i = 0; i < assembly.size(); i++)
    {
        int pos = 0;
        first_arg = find_argument(assembly[i], pos);

        // label found, as first_arg is not opcode
        if (!opcode_tbl.count(first_arg))
        {
            char select_c = first_arg[0];
            // true if label starts with ('a-z', 'A-Z' or '_')
            if (is_between(select_c, 'a', 'z') || is_between(select_c, 'A', 'Z') || select_c == '_')
            {
                bool is_alphanumeric;
                // validate full label is alphanumeric or '_', and contains ':' at the end (optional)
                for (int i = 1; i < first_arg.size(); i++)
                {
                    select_c = first_arg[i];

                    is_alphanumeric = 
                           is_between(select_c, 'a', 'z') || is_between(select_c, 'A', 'Z')
                        || is_between(select_c, '0', '9') 
                        || select_c == '_';
                    
                    // end of label contains colon
                    if (select_c == ':' && i == first_arg.size() -1)
                    {
                        is_alphanumeric = true;
                        first_arg.pop_back();   // remove colon for hash table lookup
                    }
                    
                    // not alphanumeric, skip label so parser will report error line
                    if (!is_alphanumeric)
                        break;
                }

                // all tests passed, add label
                if (is_alphanumeric)
                {
                    linker_[first_arg] = i;

                    #ifdef ASM_PARSER_DEBUG
                    printf("\nparser::generate_label_linker: found [%d]|%s| \n", 
                            i, first_arg.c_str());
                    #endif
                }
            }
        }
    }
    return linker_;

} /* ::generate_label_linker() */

Inst assembly_to_inst(std::string &_line, LabelLinker &_linker, int _index)
{    
    /* Inst members */
    Inst const inst;
    Inst::Operation OP_ {inst.OP};
    Inst::Operand A_ {inst.A}, 
                  B_ {inst.B};
    /* Asm line */
    std::string asm_arg;      // stores assembly code arguments
    bool no_modifier = false; // flag to notify function to create modifier
    int pos = 0;              // tracks asm line processing position

    // set mode & value to operand A until B is found
    Admo *admo_ptr = &A_.admo;
    int  *val_ptr  = &A_.val;

    // get assembly argument
    asm_arg = find_argument(_line, pos);

    /* <label> */
    if (asm_arg[asm_arg.size() -1] == ':')
    {
        asm_arg.pop_back(); // remove colon (not contained by hash table)
    }

    // skip <label>
    if (_linker.count(asm_arg))
    {
        // get [code]
        asm_arg = find_argument(_line, pos);
    }
    
    /* [code] */
    if (opcode_tbl.count(asm_arg))
    {
        // add [code]
        OP_.code = opcode_tbl.at(asm_arg);

        // check [code] has <mod>
        if (_line[pos] == '.')
        {
            // get <mod> argument 
            asm_arg = find_argument(_line, pos);

            /* <mod> */
            if (mod_tbl.count(asm_arg))
            {
                OP_.mod = mod_tbl.at(asm_arg);

                // opcode modifier override
                switch (OP_.code)
                {
                /* Ignored */
                case Opcode::NOP:
                case Opcode::DAT: OP_.mod = Modifier::F; break;
                case Opcode::JMP:
                case Opcode::SPL: OP_.mod = Modifier::B; break;
                /* Filtered */
                case Opcode::JMZ:
                case Opcode::JMN:
                case Opcode::DJN:
                    switch (OP_.mod) // correct: compare jumps modifier
                    {
                    case Modifier::AB: OP_.mod = Modifier::B; break;
                    case Modifier::BA: OP_.mod = Modifier::A; break;
                    case Modifier::X:
                    case Modifier::I:  OP_.mod = Modifier::F; break;
                    }
                }
            }
            else invalid_assembly(_index, _line, asm_arg);// invalid <mod>
        }
        // set flag to create default <mod> at the end (depends on all other arguments)
        else no_modifier = true;

        // loop over both <mode>[operand]
        while (pos < _line.size())
        {
            // get <mode>[operand]
            asm_arg = find_argument(_line, pos);
            
            /* <mode> */
            if (admo_tbl.count(asm_arg[0]))
            {
                *admo_ptr = admo_tbl.at(asm_arg[0]);

                // remove <mode> from argument
                asm_arg = asm_arg.substr(1);
            }
            // no <mode>, add default
            else 
            {
                if (OP_.code == Opcode::DAT) *admo_ptr = admo_tbl.at('#');
                else                         *admo_ptr = admo_tbl.at('$');
            }

            /* [operand] */
            char first_c = asm_arg[0];
            if (is_between(first_c, '0', '9') || first_c == '+' || first_c == '-')
            {
                *val_ptr = stoi(asm_arg);
            }
            // options exhausted, assume [operand] is <label> reference
            else
            {
                // get label position from linker
                if (_linker.count(asm_arg))
                {
                    int lbl_i    = _linker.at(asm_arg); // label address 

                    // calculate labels relative reference for operand address
                    if      (lbl_i < _index) lbl_i -= _index;
                    else if (lbl_i > _index) lbl_i  = std::abs(_index - lbl_i);
                    
                    *val_ptr = lbl_i;
                }
                else invalid_assembly(_index, _line, asm_arg); // invalid [operand]
            }
            // move from <mode>[operand] A -> B
            admo_ptr = &B_.admo;
            val_ptr  = &B_.val;
        }
        // get default <modifer>
        if (no_modifier)
        {
            OP_.mod = Inst::find_default_mod(OP_.code, A_.admo, B_.admo);
        }
    }
    else invalid_assembly(_index, _line,  _line.substr(0, pos)); // invalid [code]

    return Inst(OP_, A_, B_);

} /* ::assembly_to_inst() */

Warrior *create_warrior(std::string _warrior_name, AssemblyCode &_assembly, int _max_warrior_len)
{
    LabelLinker linker_;             // stores label positions
    int _length = _assembly.size();  // number of warrior instruction

    // validate number of instructions is within configuration bounds
    if (_length > _max_warrior_len)
    {
        printf("Warning: '%s' has a length greater than the max (%d) and will be truncated."
                "\n\tEdit corewar config file to increase '_max_warrior_len'\n",
                _warrior_name.c_str(), _max_warrior_len
        );
        // truncate length
        _length = _max_warrior_len;
    }

    // construct warrior w/ default arguments
    Warrior *warrior_ = new Warrior(_warrior_name.c_str(), _length, _max_warrior_len);

    // clean asm code to correct format
    for (int i = 0; i < _length; i++)
    {
        _assembly[i] = clean_assembly(_assembly[i]);
    }

    // find all labels within the asm code
    linker_ = generate_label_linker(_assembly);

    // parse all asm code lineing instructions to a instruction object (Inst)
    for (int i = 0; i < _length; i++)
    {
        // replace default warrior instructions
        (*warrior_).push(assembly_to_inst(_assembly[i], linker_, i));

        #ifdef ASM_PARSER_DEBUG
        if (i == 0) printf("\nparser::create_warrior: '%s'\n" , _warrior_name.c_str());
        printf(" line:[%d] \n\t I::|%s| \n\t O::|%s|\n",
                i + 1,
                _assembly[i].c_str(),
                (*warrior_)[i].to_assembly().c_str());
        #endif
    }
    return warrior_;

} /* ::create_warrior() */

} /* ::Parser */
