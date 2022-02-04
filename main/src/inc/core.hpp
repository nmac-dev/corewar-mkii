/// core.cpp

/** The Core is a simulated memory space for memory
 *  Each position within the core acts as a memory address for a single instruction
 *  Addresses are absolute, however warriors use relative addressing (individual instructions)
 *  Mod math is used for the core, therefore an address that is (< 0) or (> core max). loops round like a circle
 *      For example: 
 *          (1 .. x) = core 
 *               [x] = current position
 *          (1 2 3 4 5 [6] 7 8), [6]+3 = ([1] 2 3 4 5 6 7 8)
 */

/// AsmInst: a struct for storing an assembly instruction
struct AsmInst {
    
    char *label,        // Alias assigned to a specific memory address (alphanumeric only, starts with 'a-z' or '_')
         *opcode,       // Specifies operation to perform (i.e. ADD, MOV, ..)
         *modifier,     // Modifies opcode behaviour
         *operand_a,    // Operand A of the opcode argument
         *operand_b;    // Operand B of the opcode argument

    AsmInst(char *lbl, char *opcode, char *mod, char *op_a, char *op_b);
    ~AsmInst();
};

AsmInst::AsmInst(char *lbl, char *opcode, char *mod, char *op_a, char *op_b) {

    label     = lbl;
    opcode    = opcode;
    modifier  = mod;
    operand_a = op_a;
    operand_b = op_b;
}

/// Core: contains and handles a memory array of assembly instructions
class Core {

 private:
    static short int core_size; // stores core_size from match_settings.txt

 public:
    AsmInst *memory_array;

    Core(short int size);
    ~Core();
};