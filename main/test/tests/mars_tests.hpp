#include "template/test_suite.hpp"

#include "mars.hpp"

namespace TS { namespace _MARS_
{
namespace /* {anonymous} */
{
    using namespace OS;
    using namespace ASM;

Info suite_info(Info _info)
{
    _info.func_name = "MARS::" + _info.func_name;
    return _info;
}

BoolInt OUT_OF_BOUNDS(MARS &_mars); /** TEST: out of bounds (lower | upper)                        */
BoolInt ALL_ADMOS(MARS &_mars);     /** TEST: all <admo> '#', '$', '*', '@', '<', '{', '>', '}'    */
BoolInt ALL_MODIFIERS(MARS &_mars); /** TEST: all <mod> '.a', '.b', '.ab', '.ba', '.f', '.x', '.i' */
} /* ::{anonymous} */

/** ALLTESTS: MARS */
BoolInt ALL_TESTS()
{
    constexpr int max_warrior_len = 12,
                  min_seperation  = 12;

    ASM::WarriorList warriors;
    warriors.push_back(
        ASM::UniqWarrior ( new ASM::Warrior("example", 1, max_warrior_len) )
    );
    warriors[0].get()->push(Inst());

    MARS mars_(&warriors, min_seperation); // scheduler starts with 1 process

 /** ALLTESTS: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BoolInt results_ = TEST_PASSED;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if (results_ += OUT_OF_BOUNDS(mars_) ) return results_;
    if (results_ += ALL_ADMOS(mars_)     ) return results_;
    if (results_ += ALL_MODIFIERS(mars_) ) return results_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return results_;
} /* ::ALL_TESTS() */

namespace /* {anonymous} */
{
/** TEST: out of bounds (lower | upper) */
BoolInt OUT_OF_BOUNDS(MARS &_mars)
{
    Inst const _reset_ = Inst();
    int  const  lower_bounds = 0,
                upper_bounds = _mars.size() - 1;
    
    int test_index;
    Inst  &_lower = _mars[lower_bounds],
          &_upper = _mars[upper_bounds];

    _lower.OP.code = _upper.OP.code = OPCODE::NOP;

 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]:         nop  0, 0  | [_lower]
                        [..]:        dat  0, 0  |
                        [size() -1]: nop  0, 0  | [_upper]
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"operator[]", "OUT_OF_BOUNDS()", ""} ));
    int E_, A_;

    #define OUT_OF_BOUNDS__TEST(E_REF, A_INDEX) \
        E_ = (int) E_REF;                       \
        A_ = (int) _mars[A_INDEX].OP.code;      \
                                                \
        RUN_TEST(E_, A_, HDR_);
    /* ::OUT_OF_BOUNDS__TEST() */

    #define OUT_OF_BOUNDS__RESTORE()    \
        _lower = _upper = _reset_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Lower Bounds ( RAM[-1] )";

    OUT_OF_BOUNDS__TEST(_lower.OP.code, lower_bounds -1)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Upper Bounds ( RAM[size()] )";
    
    OUT_OF_BOUNDS__TEST(_upper.OP.code, upper_bounds +1)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OUT_OF_BOUNDS__RESTORE()
    return HDR_.result;
} /* ::OUT_OF_BOUNDS() */

/** TEST: all <admo> '#', '$', '*', '@', '<', '{', '>', '}' */
BoolInt ALL_ADMOS(MARS &_mars)
{
    Inst const _reset_ = Inst();
    int _index = 0;
    Inst &_exe = _mars[_index];             // [EXE]
    _exe.A.val = 1;
    _exe.B.val = 2;

    Inst &_src = _mars[_exe.A.val];         // [SRC]
    _src.A.val = 3;

    Inst &_dest = _mars[_exe.B.val];        // [DEST]
    _dest.B.val = 4;

    ADMO test_admo;
    ControlUnit CTRL ( {_index, &_exe} );

 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]: dat (?)1, (?)2    |           [EXE]
                        [1]: dat   #5,   #0    | (--|++)   [SRC]
                        [2]: dat   #0,   #9    | (--|++)   [DEST]
                        [3]: dat   #0,   #0    |               
                        [4]: dat   #0,   #0    | indirect  (SRC)
                        [5]: dat   #0,   #0    |       
                        [6]: dat   #0,   #0    | indirect  (DEST)
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"decode_admo()", "ALL_ADMOS()", ""} ));
    int E__src,  A__src;
    int E__dest, A__dest;

    #define ALL_ADMOS__RESET_TEST()  \
        _src = _dest = _reset_;      \
        _src.A.val   = 3;            \
        _dest.B.val  = 4;

    #define ALL_ADMOS__SET_TEST(E_1, E_2)  \
        E__src   = E_1;                    \
        E__dest  = E_2;                    \
                                           \
        _exe.A.admo = _exe.B.admo = test_admo;
    /* # ::..__SET_TEST() */

    #define ALL_ADMOS__RUN_TEST(A_1, A_2)             \
        CTRL.SRC  = _mars.decodeAdmo(&CTRL, OPR::A);  \
        CTRL.DEST = _mars.decodeAdmo(&CTRL, OPR::B);  \
                                                      \
        if (test_admo >= ADMO::POST_INC_A)            \
            _mars.applyPostInc(CTRL);                 \
                                                      \
        A__src  = A_1;                                \
        A__dest = A_2;                                \
                                                      \
        RUN_TEST(E__src,  A__src, HDR_);              \
        RUN_TEST(E__dest, A__dest, HDR_);             \
                                                      \
        ALL_ADMOS__RESET_TEST()
    /* # ::..__RUN_TEST() */

    #define ALL_ADMOS__RESTORE()        \
        _exe = _src = _dest = _reset_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "IMMEDIATE ( '#' )";

    test_admo = ADMO::IMMEDIATE;                    // [EXE]

    ALL_ADMOS__SET_TEST(_index,         _index)
    ALL_ADMOS__RUN_TEST(CTRL.SRC.index, CTRL.DEST.index)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "DIRECT ( '$' )";

    test_admo = ADMO::DIRECT;                       // [SRC, DEST]

    ALL_ADMOS__SET_TEST(_exe.A.val,     _exe.B.val)
    ALL_ADMOS__RUN_TEST(CTRL.SRC.index, CTRL.DEST.index)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "INDIRECT_A ( '*' )";

    test_admo = ADMO::INDIRECT_A;   // [SRC  + A]
                                    // [DEST + A]
    ALL_ADMOS__SET_TEST(_exe.A.val +  _src.A.val,
                            _exe.B.val + _dest.A.val)
    ALL_ADMOS__RUN_TEST(CTRL.SRC.index, 
                            CTRL.DEST.index)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "INDIRECT_B ( '@' )";

    test_admo = ADMO::INDIRECT_B;   // [SRC  + B]
                                    // [DEST + B]
    ALL_ADMOS__SET_TEST(_exe.A.val +  _src.B.val,
                            _exe.B.val + _dest.B.val)
    ALL_ADMOS__RUN_TEST(CTRL.SRC.index, 
                            CTRL.DEST.index)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "PRE_DEC_A ( '{' )";

    test_admo = ADMO::PRE_DEC_A;    // (*) + (--A)

    ALL_ADMOS__SET_TEST(_src.A.val - 1, _dest.A.val - 1)
    ALL_ADMOS__RUN_TEST(_src.A.val,     _dest.A.val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "PRE_DEC_B ( '<' )";
 
    test_admo = ADMO::PRE_DEC_B;    // (@) + (--B)

    ALL_ADMOS__SET_TEST(_src.B.val - 1, _dest.B.val - 1)
    ALL_ADMOS__RUN_TEST(_src.B.val,     _dest.B.val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "POST_INC_A ( '}' )";

    test_admo = ADMO::POST_INC_A;    // (*) + (A++)

    ALL_ADMOS__SET_TEST(_src.A.val + 1, _dest.A.val + 1)
    ALL_ADMOS__RUN_TEST(_src.A.val,     _dest.A.val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "POST_INC_B ( '>' )";

    test_admo = ADMO::POST_INC_B;   // (@) + (B++)

    ALL_ADMOS__SET_TEST(_src.B.val + 1, _dest.B.val + 1)
    ALL_ADMOS__RUN_TEST(_src.B.val,     _dest.B.val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ALL_ADMOS__RESTORE()
    return HDR_.result;
} /* ::ALL_ADMOS() */

/** TEST: all <mod> ('.a'|'.b'|'.ab'|'.ba'|'.f'|'.x'|'.i') */
BoolInt ALL_MODIFIERS(MARS &_mars)
{
    Inst const _reset_ = Inst();
    int _index = 0;

    Register const _exe (_index, &_mars[_index]);               //  [EXE]
                   _exe.OP->code = OPCODE::MOV;
                   _exe.A->val   = 1;
                   _exe.B->val   = 2;

    Register const _src( _exe.A->val, &_mars[_exe.A->val]);     //  [SRC]
                   _src.A->val = 3;
                   _src.B->val = 6;

    Register const _dest(_exe.B->val, &_mars[_exe.B->val]);     //  [DEST]
                   _dest.A->val = 9;
                   _dest.B->val = 12;

    MOD test_modifier;
    ControlUnit CTRL (_exe, _src, _dest);

 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]: mov   $1,   $2     |   [EXE]
                        [1]: dat   #3,   #6     |   [SRC]
                        [2]: dat   #9,   #12    |   [DEST]
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"decode_modifier()", "ALL_MODIFIERS()", ""} ));
    int E__src,  A__src;
    int E__dest, A__dest;

    #define ALL_MODIFIERS__RESET_TEST()        \
        CTRL = ControlUnit(_exe, _src, _dest);

    #define ALL_MODIFIERS__SET_TEST(E_1, E_2)  \
        E__src   = E_1;                        \
        E__dest  = E_2;                        \
        _exe.OP->mod = _exe.OP->mod = test_modifier;
    /* # ::..__SET_TEST() */

    #define ALL_MODIFIERS__RUN_TEST(A_1, A_2)  \
        _mars.decodeModifier(&CTRL);           \
        A__src  = A_1;                         \
        A__dest = A_2;                         \
                                               \
        RUN_TEST(E__src,   A__src,  HDR_);     \
        RUN_TEST(E__dest,  A__dest, HDR_);     \
                                               \
        ALL_MODIFIERS__RESET_TEST()
    /* # ::..__RUN_TEST() */

    #define ALL_MODIFIERS__RESTORE()           \
        _mars[_index] = _mars[_exe.A->val] = _mars[_exe.B->val] = _reset_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.A' )";

    test_modifier = MOD::A;     // [SRC] A -> A [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.A->val, CTRL.DEST.A->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.B' )";

    test_modifier = MOD::B;     // [SRC] B -> B [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.A->val, CTRL.DEST.A->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.AB' )";

    test_modifier = MOD::AB;    // [SRC] A -> B [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.A->val, CTRL.DEST.A->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.BA' )";

    test_modifier = MOD::BA;    // [SRC] B -> A [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.A->val, CTRL.DEST.A->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.F' )";

    test_modifier = MOD::F;    // [SRC] A,B -> A,B [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.A->val, CTRL.DEST.A->val)

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.B->val, CTRL.DEST.B->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.X' )";

    test_modifier = MOD::X;    // [SRC] A,B -> B,A [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.A->val, CTRL.DEST.A->val)

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.B->val, CTRL.DEST.B->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.I' )";

    test_modifier = MOD::I;    // [SRC] A,B -> B,A [DEST]

    ALL_MODIFIERS__SET_TEST((int) MOD_TYPE::FULL, 0)
    ALL_MODIFIERS__RUN_TEST((int)  CTRL.TYPE.mod, 0)

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.A->val, CTRL.DEST.A->val)

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(CTRL.SRC.B->val, CTRL.DEST.B->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ALL_MODIFIERS__RESTORE()
    return HDR_.result;
} /* ::ALL_MODIFIERS() */

} /* ::{anonymous}  */
}} /* ::TS::_MARS_ */
