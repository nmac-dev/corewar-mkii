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

#define TS__MARS__SET_TEST_ENV()                                                \
    constexpr int max_warrior_len = 12,                                         \
                  min_seperation  = 12;                                         \
                                                                                \
    ASM::WarriorVec warriors;                                                   \
    warriors.push_back(                                                         \
        ASM::UniqWarrior ( new ASM::Warrior("example", 1, max_warrior_len) )    \
    );                                                                          \
    warriors[0].get()->push(Inst());                                            \
                                                                                \
    MARS mars_(&warriors, min_seperation);
    /* TS__MARS__SET_TEST_ENV() */

BoolInt OUT_OF_BOUNDS(); /** TEST: out of bounds (lower | upper)                        */
BoolInt ALL_ADMOS();     /** TEST: all <admo> '#', '$', '*', '@', '<', '{', '>', '}'    */
BoolInt ALL_MODIFIERS(); /** TEST: all <mod> '.a', '.b', '.ab', '.ba', '.f', '.x', '.i' */
} /* ::{anonymous} */

/** ALLTESTS: ( OS::MARS ) */
BoolInt ALL_TESTS()
{
 /** ALLTESTS: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BoolInt results_ = TEST_PASSED;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if (results_ += OUT_OF_BOUNDS() ) return results_;
    if (results_ += ALL_ADMOS()     ) return results_;
    if (results_ += ALL_MODIFIERS() ) return results_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return results_;
}

namespace /* {anonymous} */
{
/** TEST: out of bounds (lower | upper) */
BoolInt OUT_OF_BOUNDS()
{
    TS__MARS__SET_TEST_ENV()
    
    int  const  lower_bounds = 0,
                upper_bounds = mars_.size() - 1;
    
    int test_index;
    Inst  &lower_ = mars_[lower_bounds],
          &upper_ = mars_[upper_bounds];

    lower_.OP.code = upper_.OP.code = Opcode::NOP;

 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]:         nop  0, 0  | [lower_]
                        [..]:        dat  0, 0  |
                        [size() -1]: nop  0, 0  | [upper_]
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"operator[]", "OUT_OF_BOUNDS()", ""} ));
    int E_, A_;

    #define OUT_OF_BOUNDS__TEST(E_REF, A_INDEX) \
        E_ = (int) E_REF;                       \
        A_ = (int) mars_[A_INDEX].OP.code;      \
                                                \
        RUN_TEST(E_, A_, HDR_);
    /* ::OUT_OF_BOUNDS__TEST() */
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Lower Bounds ( RAM[-1] )";

    OUT_OF_BOUNDS__TEST(lower_.OP.code, lower_bounds -1)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Upper Bounds ( RAM[size()] )";
    
    OUT_OF_BOUNDS__TEST(upper_.OP.code, upper_bounds +1)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ::OUT_OF_BOUNDS() */

/** TEST: all <admo> '#', '$', '*', '@', '<', '{', '>', '}' */
BoolInt ALL_ADMOS()
{
    TS__MARS__SET_TEST_ENV()
    Inst const _reset_ = Inst();

    int _index = 0;
    Inst &_exe = mars_[_index];             // [EXE]
    _exe.A.val = 1;
    _exe.B.val = 2;

    Inst &_src = mars_[_exe.A.val];         // [SRC]
    _src.A.val = 3;

    Inst &_dest = mars_[_exe.B.val];        // [DEST]
    _dest.B.val = 4;

    Admo test_admo;
    ControlUnit ctrl ( {_index, &_exe} );

 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]: dat (?)1, (?)2    |           [EXE]
                        [1]: dat   #5,   #0    | (--|++)   [SRC]
                        [2]: dat   #0,   #9    | (--|++)   [DEST]
                        [3]: dat   #0,   #0    |               
                        [4]: dat   #0,   #0    | indirect  (SRC)
                        [5]: dat   #0,   #0    |       
                        [6]: dat   #0,   #0    | indirect  (DEST)
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"decode_admo()", "ALL_ADMOS()", ""} ));
    int E__src,  A__src;
    int E__dest, A__dest;

    #define ALL_ADMOS__SET_TEST(E_1, E_2)       \
        E__src   = E_1;                         \
        E__dest  = E_2;                         \
                                                \
        _exe.A.admo = _exe.B.admo = test_admo;
    /* # ALL_ADMOS__SET_TEST() */

    #define ALL_ADMOS__RUN_TEST(A_1, A_2)                     \
        ctrl.SRC  = mars_.decode_admo(&ctrl, INST_FIELD::A);  \
        ctrl.DEST = mars_.decode_admo(&ctrl, INST_FIELD::B);  \
                                                              \
        if (test_admo >= Admo::POST_INC_A)                    \
            mars_.apply_post_inc(ctrl);                       \
                                                              \
        A__src  = A_1;                                        \
        A__dest = A_2;                                        \
                                                              \
        RUN_TEST(E__src,  A__src, HDR_);                      \
        RUN_TEST(E__dest, A__dest, HDR_);                     \
                                                              \
        _src = _dest = _reset_;                               \
        _src.A.val   = 3;                                     \
        _dest.B.val  = 4;
    /* # ALL_ADMOS__RUN_TEST() */
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "IMMEDIATE ( '#' )";

    test_admo = Admo::IMMEDIATE;                    // [EXE]

    ALL_ADMOS__SET_TEST(_index,         _index)
    ALL_ADMOS__RUN_TEST(ctrl.SRC.address, ctrl.DEST.address)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "DIRECT ( '$' )";

    test_admo = Admo::DIRECT;                       // [SRC, DEST]

    ALL_ADMOS__SET_TEST(_exe.A.val,     _exe.B.val)
    ALL_ADMOS__RUN_TEST(ctrl.SRC.address, ctrl.DEST.address)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "INDIRECT_A ( '*' )";

    test_admo = Admo::INDIRECT_A;   // [SRC  + A]
                                    // [DEST + A]
    ALL_ADMOS__SET_TEST(_exe.A.val +  _src.A.val,
                            _exe.B.val + _dest.A.val)
    ALL_ADMOS__RUN_TEST(ctrl.SRC.address, 
                            ctrl.DEST.address)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "INDIRECT_B ( '@' )";

    test_admo = Admo::INDIRECT_B;   // [SRC  + B]
                                    // [DEST + B]
    ALL_ADMOS__SET_TEST(_exe.A.val +  _src.B.val,
                            _exe.B.val + _dest.B.val)
    ALL_ADMOS__RUN_TEST(ctrl.SRC.address, 
                            ctrl.DEST.address)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "PRE_DEC_A ( '{' )";

    test_admo = Admo::PRE_DEC_A;    // (*) + (--A)

    ALL_ADMOS__SET_TEST(_src.A.val - 1, _dest.A.val - 1)
    ALL_ADMOS__RUN_TEST(_src.A.val,     _dest.A.val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "PRE_DEC_B ( '<' )";
 
    test_admo = Admo::PRE_DEC_B;    // (@) + (--B)

    ALL_ADMOS__SET_TEST(_src.B.val - 1, _dest.B.val - 1)
    ALL_ADMOS__RUN_TEST(_src.B.val,     _dest.B.val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "POST_INC_A ( '}' )";

    test_admo = Admo::POST_INC_A;    // (*) + (A++)

    ALL_ADMOS__SET_TEST(_src.A.val + 1, _dest.A.val + 1)
    ALL_ADMOS__RUN_TEST(_src.A.val,     _dest.A.val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "POST_INC_B ( '>' )";

    test_admo = Admo::POST_INC_B;   // (@) + (B++)

    ALL_ADMOS__SET_TEST(_src.B.val + 1, _dest.B.val + 1)
    ALL_ADMOS__RUN_TEST(_src.B.val,     _dest.B.val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ALL_ADMOS() */

/** TEST: all <mod> ('.a'|'.b'|'.ab'|'.ba'|'.f'|'.x'|'.i') */
BoolInt ALL_MODIFIERS()
{
    TS__MARS__SET_TEST_ENV()

    int _index = 0;
    Register const _exe (_index, &mars_[_index]);               //  [EXE]
                   _exe.OP->code = Opcode::MOV;
                   _exe.A->val   = 1;
                   _exe.B->val   = 2;

    Register const _src( _exe.A->val, &mars_[_exe.A->val]);     //  [SRC]
                   _src.A->val = 3;
                   _src.B->val = 6;

    Register const _dest(_exe.B->val, &mars_[_exe.B->val]);     //  [DEST]
                   _dest.A->val = 9;
                   _dest.B->val = 12;

    Modifier test_modifier;
    ControlUnit ctrl (_exe, _src, _dest);

 /** ENVIROMENT: *//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/
                        [0]: mov   $1,   $2     |   [EXE]
                        [1]: dat   #3,   #6     |   [SRC]
                        [2]: dat   #9,   #12    |   [DEST]
 *//** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"decode_modifier()", "ALL_MODIFIERS()", ""} ));
    int E__src,  A__src;
    int E__dest, A__dest;

    #define ALL_MODIFIERS__SET_TEST(E_1, E_2)        \
        E__src   = E_1;                              \
        E__dest  = E_2;                              \
        _exe.OP->mod = _exe.OP->mod = test_modifier;
    /* # ::..__SET_TEST() */

    #define ALL_MODIFIERS__RUN_TEST(A_1, A_2)  \
        mars_.decode_modifier(&ctrl);          \
        A__src  = A_1;                         \
        A__dest = A_2;                         \
                                               \
        RUN_TEST(E__src,   A__src,  HDR_);     \
        RUN_TEST(E__dest,  A__dest, HDR_);     \
                                               \
        ctrl = ControlUnit(_exe, _src, _dest);
    /* # ::..__RUN_TEST() */
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.A' )";

    test_modifier = Modifier::A;     // [SRC] A -> A [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.A->val, ctrl.DEST.A->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.B' )";

    test_modifier = Modifier::B;     // [SRC] B -> B [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.A->val, ctrl.DEST.A->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.AB' )";

    test_modifier = Modifier::AB;    // [SRC] A -> B [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.A->val, ctrl.DEST.A->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.BA' )";

    test_modifier = Modifier::BA;    // [SRC] B -> A [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.A->val, ctrl.DEST.A->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.F' )";

    test_modifier = Modifier::F;    // [SRC] A,B -> A,B [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.A->val, ctrl.DEST.A->val)

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.B->val, ctrl.DEST.B->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.X' )";

    test_modifier = Modifier::X;    // [SRC] A,B -> B,A [DEST]

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.A->val, ctrl.DEST.A->val)

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.B->val, ctrl.DEST.B->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "( '.I' )";

    test_modifier = Modifier::I;    // [SRC] A,B -> B,A [DEST]

    ALL_MODIFIERS__SET_TEST((int) ModifierType::FULL, 0)
    ALL_MODIFIERS__RUN_TEST((int)  ctrl.TYPE.mod, 0)

    ALL_MODIFIERS__SET_TEST(    _src.A->val,     _dest.A->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.A->val, ctrl.DEST.A->val)

    ALL_MODIFIERS__SET_TEST(    _src.B->val,     _dest.B->val)
    ALL_MODIFIERS__RUN_TEST(ctrl.SRC.B->val, ctrl.DEST.B->val)
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ALL_MODIFIERS() */

} /* ::{anonymous}  */
}} /* ::TS::_MARS_ */
