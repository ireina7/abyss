#include "Instruction.hpp"



namespace abyss {

    namespace instructions {

        Atom MOVE(Reg a, Reg b) {
            return Atom::create_iABC(OP_MOVE, a, b, 0, 0);
        }
        Atom LOAD(Reg a, const I32 i) {
            return Atom::create_iAsBx(OP_LOADI, a, i);
        }
        Atom LOAD(Reg a, const F64 f) {
            return Atom::create_iAsBx(OP_LOADF, a, f);
        }
        Atom LOAD(Reg a, const bool b) {
            return b
                ? Atom::create_iABx(OP_LOADTRUE,  a, 0)
                : Atom::create_iABx(OP_LOADFALSE, a, 0);
        }
        Atom LOADK(Reg a, Slot k) {
            return Atom::create_iABx(OP_LOADK, a, k);
        }
        Atom LOADNIL(Reg a, I32 b) {
            return Atom::create_iABC(OP_LOADNIL, a, b, 0, 0);
        }
        Atom ADD(Reg a, Reg b, Reg c) {
            return Atom::create_iABC(OP_ADD, a, b, c, 0);
        }
        Atom SUB(Reg a, Reg b, Reg c) {
            return Atom::create_iABC(OP_SUB, a, b, c, 0);
        }
        Atom MUL(Reg a, Reg b, Reg c) {
            return Atom::create_iABC(OP_MUL, a, b, c, 0);
        }
        Atom DIV(Reg a, Reg b, Reg c) {
            return Atom::create_iABC(OP_DIV, a, b, c, 0);
        }
        Atom ADDK(Reg a, Reg b, Slot c) {
            return Atom::create_iABC(OP_ADD, a, b, c, 0);
        }
        Atom SUBK(Reg a, Reg b, Slot c) {
            return Atom::create_iABC(OP_SUB, a, b, c, 0);
        }
        Atom MULK(Reg a, Reg b, Slot c) {
            return Atom::create_iABC(OP_MUL, a, b, c, 0);
        }
        Atom DIVK(Reg a, Reg b, Slot c) {
            return Atom::create_iABC(OP_DIV, a, b, c, 0);
        }
        Atom EQ(Reg a, Reg b, Reg c) {
            return Atom::create_iABC(OP_EQ, a, b, c, 0);
        }
        Atom LT(Reg a, Reg b, Reg c) {
            return Atom::create_iABC(OP_LT, a, b, c, 0);
        }
        Atom LE(Reg a, Reg b, Reg c) {
            return Atom::create_iABC(OP_LE, a, b, c, 0);
        }

        Atom CALL(Reg a, Reg b, Reg c) {
            return Atom::create_iABC(OP_CALL, a, b, c, 0);
        }
        Atom RETURN(Reg a) {
            return Atom::create_iABC(OP_RETURN1, a, 0, 0, 0);
        }
        Atom RETURN() {
            return Atom::create_iABC(OP_RETURN, 0, 0, 0, 0);
        }

        Atom GETTABUP(Reg a, Reg b, Reg c) {
            return Atom::create_iABC(OP_GETTABUP, a, b, c, 0);
        }
        Atom GETUPVAL(Reg a, Reg b) {
            return Atom::create_iABC(OP_GETUPVAL, a, b, 0, 0);
        }
        Atom CLOSURE(Reg a, Reg bx) {
            return Atom::create_iABx(OP_CLOSURE, a, bx);
        }

        Atom TEST(Reg a, Reg c) {
            return Atom::create_iABC(OP_TEST, a, 0, c, 0);
        }
        Atom JUMP(Reg sJ) {
            return Atom::create_iAsBx(OP_JMP, 0, sJ);
        }
    }//end namespace instructions

}//end namespace abyss
