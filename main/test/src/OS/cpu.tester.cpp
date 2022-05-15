#include "OS/cpu.tester.hpp"

int main(int argc, char const *argv[])
{
    return TS::_CPU_::ALL_TESTS();
}

namespace TS { namespace _CPU_
{
/** ALLTESTS: ( OS::CPU ) */
BoolInt ALL_TESTS()
{
 /** ALLTESTS: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BoolInt results_ = TEST_PASSED;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if ( results_ += SYSTEM_CODES()      ) return results_;
    if ( results_ += COMPARISION_CODES() ) return results_;
    if ( results_ += ARITHMETIC_CODES()  ) return results_;
    if ( results_ += JUMP_CODES()        ) return results_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return results_;
}

namespace /* {anonymous} */
{
/** TEST: all system [code]... NOP, DAT, MOV, SPL                  */
BoolInt SYSTEM_CODES()
{
    TS__CPU__CONST_OPRS()
    
    Inst::Operand djn_zero  {Admo::IMMEDIATE, 1};

    int constexpr n_inst = 6;
    Inst const test_insts[n_inst] {
        Inst( {Opcode::MOV, Modifier::I }, jump_f, base_f ),
        Inst( {Opcode::MOV, Modifier::F }, jump_f, dest_f ),
        Inst( {Opcode::MOV, Modifier::AB}, src_f,  dest_f ),
        Inst( {Opcode::SPL, Modifier::B }, jump_f, base_f ),
        Inst( {Opcode::NOP, Modifier::B }, base_f, base_f ),
        Inst( /* DAT #0, #0 */                            ),
    };
    TS__CPU__SET_TEST_ENV(n_inst, test_insts)

    Report test_rpt;

 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]  | mov.i    $3, #4  |
                        [1]  | mov.f    $3, #4  |
                        [2]  | mov.ab   #8, #4  |
                        [3]  | spl.b    $3, #0  |
                        [4]  | nop      #0, #0  |
                        [5]  | dat      #0, #0  |
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"execute_system()", "SYSTEM_CODES()", ""} ));
    int E_,  A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[MOV].<I> (FULL)";

    E_ = (int) Opcode::SPL;     // move [3] -> [0]

    test_rpt = core_.run_fde_cycle();
    A_ = (int) memory_[ test_rpt.dest.address ].OP.code;

    TS__CPU__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[MOV].<F> (DOUBLE)";

    E_ = base_f.val;

    test_rpt = core_.run_fde_cycle();
    A_ = memory_[ test_rpt.dest.address ].A.val;  // [A]
    RUN_TEST(E_, A_, HDR_);

    A_ = memory_[ test_rpt.dest.address ].B.val;  // [B]
    TS__CPU__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[MOV].<AB> (SINGLE)";

    E_ = src_f.val;

    A_ = memory_[ core_.run_fde_cycle().dest.address ].B.val;
    TS__CPU__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[SPL]";

    E_ = sched_.processes() +1;

    core_.run_fde_cycle();   // add process
    A_ = sched_.processes();
    TS__CPU__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[NOP]";

    E_ = (int) Event::NOOP;

    A_ = (int) core_.run_fde_cycle().exe.event;
    TS__CPU__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[DAT]";

    E_ = (int) Status::TERMINATED;

    A_ = (int) core_.run_fde_cycle().status;
    TS__CPU__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 return HDR_.result;
} /* SYSTEM_CODES() */

/** TEST: all comparision [code]... SEQ, SNE, SLT                  */
BoolInt COMPARISION_CODES()
{
    TS__CPU__CONST_OPRS()

    int constexpr n_inst = 6;
    Inst const test_insts[] {
        Inst( {Opcode::SEQ, Modifier::AB}, src_f,  src_f  ),
        Inst( /* DAT #0, #0 */                            ),
        Inst( {Opcode::SNE, Modifier::AB}, src_f,  dest_f ),
        Inst( /* DAT #0, #0 */                            ),
        Inst( {Opcode::SLT, Modifier::AB}, base_f, dest_f ),
        Inst( /* DAT #0, #0 */                            ),
    };
    TS__CPU__SET_TEST_ENV(n_inst, test_insts)
    Report test_rpt;

 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]  | seq.ab   #8, #8  |
                        [1]  | ...      ..  ..  |
                        [2]  | sne.ab   #8, #4  |
                        [3]  | ...      ..  ..  |
                        [4]  | slt.ab   #0, #8  |
                        [5]  | ...      ..  ..  |
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"execute_compare()", "COMPARISION_CODES()", ""} ));
    int E_,  A_;

    #define COMPARISION_CODES__RUN_TEST()  \
        test_rpt = core_.run_fde_cycle();  \
        E_ = test_rpt.exe.address + 2;     \
        A_ = test_rpt.next_pc;             \
        TS__CPU__RUN_TEST()
    /* COMPARISION_CODES__RUN_TEST() */
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[SEQ]";
    COMPARISION_CODES__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[SNE]";
    COMPARISION_CODES__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[SLT]";
    COMPARISION_CODES__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 return HDR_.result;
} /* COMPARISION_CODES() */

/** TEST: all arithmetic [code]... ADD, SUB, MUL, DIV, MOD         */
BoolInt ARITHMETIC_CODES()
{
    TS__CPU__CONST_OPRS()

    int constexpr n_inst = 7;
    Inst const test_insts[] {
        Inst( {Opcode::ADD, Modifier::AB}, src_f, dest_f  ),
        Inst( {Opcode::SUB, Modifier::AB}, src_f, dest_f  ),
        Inst( {Opcode::MUL, Modifier::AB}, src_f, dest_f  ),
        Inst( {Opcode::DIV, Modifier::AB}, src_f, dest_f  ),
        Inst( {Opcode::MOD, Modifier::AB}, src_f, dest_f  ),
        /* Division by 0 */
        Inst( {Opcode::DIV, Modifier::AB}, src_f, base_f  ),
    };
    TS__CPU__SET_TEST_ENV(n_inst, test_insts)
 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]  | add.ab   #8, #4  |
                        [1]  | sub.ab   #8, #4  |
                        [2]  | mul.ab   #8, #4  |
                        [3]  | div.ab   #8, #4  |
                        [4]  | mod.ab   #8, #4  |
                        [3]  | div.ab   #8, #0  | ( X / 0 )
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"execute_arithmetic()", "ARITHMETIC_CODES()", ""} ));
    int E_,  A_;

    #define ARITHMETIC_CODES__RUN_TEST(E_arithmetic)                \
        E_ = E_arithmetic;                                          \
        A_ = memory_[ core_.run_fde_cycle().dest.address ].B.val;   \
        TS__CPU__RUN_TEST()
    /* ARITHMETIC_CODES__RUN_TEST() */
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[ADD]";
    ARITHMETIC_CODES__RUN_TEST(dest_f.val + src_f.val);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[SUB]";
    ARITHMETIC_CODES__RUN_TEST(dest_f.val - src_f.val);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[MUL]";
    ARITHMETIC_CODES__RUN_TEST(dest_f.val * src_f.val);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[DIV]";
    ARITHMETIC_CODES__RUN_TEST(dest_f.val / src_f.val);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[MOD]";
    ARITHMETIC_CODES__RUN_TEST(dest_f.val % src_f.val);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[DIV] (Division by 0)";

    E_ = (int) Status::TERMINATED;
    A_ = (int) core_.run_fde_cycle().status;

    TS__CPU__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 return HDR_.result;
} /* ARITHMETIC_CODES() */

/** TEST: all jump [code]... JMP, JMZ, JMN, DJN                    */
BoolInt JUMP_CODES()
{
    TS__CPU__CONST_OPRS()

    Inst::Operand djn_zero  {Admo::IMMEDIATE, 1};

    int constexpr n_inst = 15;
    Inst const test_insts[] {
        Inst( {Opcode::JMP, Modifier::B }, jump_f, base_f   ),
        Inst( /* DAT #0, #0 */                              ),
        Inst( /* DAT #0, #0 */                              ),
        Inst( {Opcode::JMZ, Modifier::B }, jump_f, base_f   ),
        Inst( /* DAT #0, #0 */                              ),
        Inst( /* DAT #0, #0 */                              ),
        Inst( {Opcode::JMN, Modifier::B }, jump_f, dest_f   ),
        Inst( /* DAT #0, #0 */                              ),
        Inst( /* DAT #0, #0 */                              ),
        Inst( {Opcode::DJN, Modifier::B }, jump_f, base_f   ),
        Inst( /* DAT #0, #0 */                              ),
        Inst( /* DAT #0, #0 */                              ),
        Inst( {Opcode::DJN, Modifier::B }, jump_f, djn_zero ),
    };
    TS__CPU__SET_TEST_ENV(n_inst, test_insts)
    Report test_rpt;

 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]  | jmp.b    #3, #0  |
                        [1]  | ...      ..  ..  |
                        [2]  | ...      ..  ..  |
                        [3]  | jmz.b    #3, #4  |
                        [4]  | ...      ..  ..  |
                        [5]  | ...      ..  ..  |
                        [6]  | jmn.b    #3, #4  |
                        [7]  | ...      ..  ..  |
                        [8]  | ...      ..  ..  |
                        [9]  | djn.b    #3, #4  | (DEST = NOT ZERO)
                       [10]  | ...      ..  ..  |
                       [11]  | ...      ..  ..  |
                       [12]  | djn.b    #3, #1  | (DEST = ZERO)
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"execute_jump()", "JUMP_CODES()", ""} ));
    int E_,  A_;
    
    #define JUMP_CODES__RUN_TEST()               \
        test_rpt = core_.run_fde_cycle();        \
        E_ = test_rpt.exe.address + jump_f.val;  \
        A_ = test_rpt.next_pc;                   \
        TS__CPU__RUN_TEST()
    /* JUMP_CODES__RUN_TEST() */
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[JMP]";
    JUMP_CODES__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[JMZ]";
    JUMP_CODES__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[JMN]";
    JUMP_CODES__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[DJN] (DEST = NOT ZERO)";
    JUMP_CODES__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "[DJN] (DEST = ZERO)";
 HDR_.cmp_op = CMP_OP::GT;
    JUMP_CODES__RUN_TEST()
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 return HDR_.result;
} /* JUMP_CODES() */

} /* ::{anonymous}  */
}} /* ::TS::_CPU_ */
