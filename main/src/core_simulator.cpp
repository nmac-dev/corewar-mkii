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