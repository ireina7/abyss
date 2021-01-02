#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include "OpCode.hpp"
#include "Utils.hpp"
#include <vector>
#include <array>
#include <initializer_list>
/*===========================================================================
  We assume that instructions are unsigned 32-bit integers.
  All instructions have an opcode in the first 7 bits.
  Instructions can have the following formats:

        3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
        1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
iABC          C(8)     |      B(8)     |k|     A(8)      |   Op(7)     |
iABx                Bx(17)               |     A(8)      |   Op(7)     |
iAsBx              sBx (signed)(17)      |     A(8)      |   Op(7)     |
iAx                           Ax(25)                     |   Op(7)     |
isJ                           sJ(25)                     |   Op(7)     |

  A signed argument is represented in excess K: the represented value is
  the written unsigned value minus K, where K is half the maximum for the
  corresponding unsigned argument.
===========================================================================*/

namespace abyss {

    using Micro = U32;
    using Reg = I32; //This should be enough and convenient
    using Slot = I32; //Slot index of constant pool
    /*
    ** size and position of opcode arguments.
    */
    constexpr U32
        SIZE_A  = 8,
        SIZE_B  = 8,
        SIZE_C  = 8,
        SIZE_Bx = SIZE_C + SIZE_B + 1,
        SIZE_Ax = SIZE_Bx + SIZE_A,
        SIZE_sJ = SIZE_Bx + SIZE_A,
        SIZE_OP = 7,

        POS_OP	= 0,
        POS_A   = POS_OP + SIZE_OP,
        POS_k   = POS_A + SIZE_A,
        POS_B   = POS_k + 1,
        POS_C   = POS_B + SIZE_B,
        POS_Bx	= POS_k,
        POS_Ax  = POS_A,
        POS_sJ  = POS_A,

        MAXARG_A   = (1 << SIZE_A) - 1,
        MAXARG_B   = (1 << SIZE_B) - 1,
        MAXARG_C   = (1 << SIZE_C) - 1,
        MAXARG_Bx  = INT_MAX,
        MAXARG_sJ  = INT_MAX,
        OFFSET_sBx = MAXARG_Bx >> 1,         /* 'sBx' is signed */
        OFFSET_sJ  = MAXARG_sJ >> 1,
        OFFSET_sC  = MAXARG_C  >> 1;


    /* creates a mask with 'n' 1 bits at position 'p' */
    constexpr Micro MASK1(U8 n, U8 p) {
        constexpr U32 zeros = 0;
        return (~(~zeros << n)) << p;
    }
    /* creates a mask with 'n' 0 bits at position 'p' */
    constexpr Micro MASK0(U8 n, U8 p)	{
        return ~MASK1(n, p);
    }
    constexpr Micro int2sC(Micro i) {
        return i + OFFSET_sC;
    }
    constexpr Micro sC2int(Micro i) {
        return i - OFFSET_sC;
    }

    /**
       The main atom instruction structure.

       Many useful tool functions for components of atom instruction
       are included.
     */
    struct Atom {

        Micro atom;
        OpMode mode;
        Atom() {}
        Atom(OpMode m, Micro a): atom(a), mode(m) {}
        static Atom create_iABC(OpCode op, I32 a, I32 b, I32 c, bool k) {
            Micro i = (static_cast<Micro>(op) << POS_OP)
                | (static_cast<Micro>(a) << POS_A)
                | (static_cast<Micro>(b) << POS_B)
                | (static_cast<Micro>(c) << POS_C)
                | (static_cast<Micro>(k) << POS_k);
            return Atom(iABC, i);
        }
        static Atom create_iABx(OpCode op, I32 a, I32 bx) {
            Micro i = (static_cast<Micro>(op) << POS_OP)
                | (static_cast<Micro>(a)  << POS_A)
                | (static_cast<Micro>(bx) << POS_Bx);
            return Atom(iABx, i);
        }
        static Atom create_iAsBx(OpCode op, I32 a, I32 sbx) {
            Micro i = (static_cast<Micro>(op) << POS_OP)
                | (static_cast<Micro>(a)  << POS_A)
                | (static_cast<Micro>(sbx) << POS_Bx);
            return Atom(iAsBx, i);
        }
        static Atom create_iAx(OpCode op, I32 ax) {
            Micro i = (static_cast<Micro>(op) << POS_OP)
                | (static_cast<Micro>(ax) << POS_Ax);
            return Atom(iAx, i);
        }
        static Atom create_isJ(OpCode op, I32 j, I32 k) {
            Micro i = (static_cast<Micro>(op) << POS_OP)
                | (static_cast<Micro>(j) << POS_sJ)
                | (static_cast<Micro>(k) << POS_k);
            return Atom(isJ, i);
        }
        OpCode getOpCode() const {
            return static_cast<OpCode>((atom >> POS_OP) & MASK1(SIZE_OP, 0));
        }
        Atom &setOpCode() {
            /*Unimplemented!*/
            return *this;
        }
        I32 getArg_A() const {
            /* No need to check OpMode since part A is included always */
            return getArg(POS_A, SIZE_A);
        }
        Atom &setArg_A(I32 v) {
            return setArg(v, POS_A, SIZE_A);
        }

        I32 getArg_B() const {
            ensureMode(iABC);
            return getArg(POS_B, SIZE_B);
        }
        I32 getArg_sB() const {
            return sC2int(getArg_B());
        }
        Atom &setArg_B(I32 v) {
            return setArg(v, POS_B, SIZE_B);
        }

        I32 getArg_C() const {
            ensureMode(iABC);
            return getArg(POS_C, SIZE_C);
        }
        I32 getArg_sC() const {
            return sC2int(getArg_C());
        }
        Atom &setArg_C(I32 v) {
            return setArg(v, POS_C, SIZE_C);
        }

        bool testArg_k() const {
            ensureMode(iABC);
            return static_cast<bool>(atom & (1u << POS_k));
        }
        U32 getArg_k() const {
            ensureMode(iABC);
            return getArg(POS_k, 1);
        }
        Atom &setArg_k(I32 v) {
            return setArg(v, POS_k, 1);
        }

        U32 getArg_Bx() const {
            ensureMode(iABx);
            return getArg(POS_Bx, SIZE_Bx);
        }
        Atom &setArg_Bx(I32 v) {
            return setArg(v, POS_Bx, SIZE_Bx);
        }


        U32 getArg_Ax() const {
            ensureMode(iAx);
            return getArg(POS_Ax, SIZE_Ax);
        }
        Atom &setArg_Ax(I32 v) {
            return setArg(v, POS_Ax, SIZE_Ax);
        }

        I32 getArg_sBx() const {
            ensureMode(iAsBx);
            return getArg(POS_Bx, SIZE_Bx);// - OFFSET_sBx;
        }
        Atom &setArg_sBx(I32 b) {
            return setArg_Bx(static_cast<U32>(b));// + OFFSET_sBx));
        }

        I32 getArg_sJ() const {
            ensureMode(isJ);
            return getArg(POS_sJ, SIZE_sJ) - OFFSET_sJ;
        }
        Atom &setArg_sJ(I32 j) {
            return setArg(static_cast<U32>(j + OFFSET_sJ), POS_sJ, SIZE_sJ);
        }

    private:
        void ensureMode(OpMode mode) const {
            ensure(this->mode == mode,
                   std::string("error> Mode of Atom instruction not match: \n") +
                   "\trequired: " + Show::show(mode) + "\n" +
                   "\tactually: " + Show::show(this->mode));
        }
        Atom &setArg(I32 v, U8 pos, U8 size) {
            atom = (atom & MASK0(size, pos)) |
                ((static_cast<Micro>(v) << pos) & MASK1(size, pos));
            return *this;
        }
        I32 getArg(U8 pos, U8 size) const {
            //I32 mask = this->atom >> (pos + size);
            //auto x = static_cast<I32>((atom >> pos) & MASK1(size, 0));
            //Show::println(mask);
            return static_cast<I32>((atom >> pos) & MASK1(size, 0));
        }
    };


    /**
       The main Instruction structure!
       Instructions can be composed now.
     */
    struct Instruction {

        string name;
        vector<Atom> instrs;
        /**
           We need to make sure that instrs has at least 1 element.
         */
        Instruction(): instrs{Atom::create_iAx(OP_PASS, 0)} {}
        Instruction(const Atom &ins): instrs{ins} {}
        Instruction(std::initializer_list<Atom> ins)
            : instrs(ins.size() == 0
                     ? std::vector<Atom>(1, Atom::create_iAx(OP_PASS, 0))
                     : ins) {}
        Instruction operator+(const Instruction &that) {
            Instruction ans;
            instrs.insert(instrs.end(), that.instrs.begin(), that.instrs.end());
            return ans;
        }
    };

    namespace instructions {

        Atom MOVE(Reg a, Reg b);
        Atom LOAD(Reg a, const I32 i);
        Atom LOAD(Reg a, const F64 f);
        Atom LOAD(Reg a, const bool b);
        Atom LOADK(Reg a, Slot k);
        Atom LOADNIL(Reg a, I32 b);
        Atom ADD(Reg a, Reg b, Reg c);
        Atom SUB(Reg a, Reg b, Reg c);
        Atom MUL(Reg a, Reg b, Reg c);
        Atom DIV(Reg a, Reg b, Reg c);
        Atom ADDK(Reg a, Reg b, Slot c);
        Atom SUBK(Reg a, Reg b, Slot c);
        Atom MULK(Reg a, Reg b, Slot c);
        Atom DIVK(Reg a, Reg b, Slot c);
        Atom EQ(Reg a, Reg b, Reg c);
        Atom LT(Reg a, Reg b, Reg c);
        Atom LE(Reg a, Reg b, Reg c);

        Atom CALL(Reg a, Reg b, Reg c);
        Atom RETURN(Reg a);
        Atom RETURN();

        Atom GETTABUP(Reg a, Reg b, Reg c);
        Atom GETUPVAL(Reg a, Reg b);
        Atom CLOSURE(Reg a, Reg bx);

        Atom TEST(Reg a, Reg c);
        Atom JUMP(Reg sJ);
    }//end namespace instructions

}//end namespace abyss



namespace Show {

    template<>
    inline abyss::string show(const abyss::Atom &atom) {
        abyss::string ans = show(atom.getOpCode()) + "\t";
        switch(atom.mode) {
        case abyss::iABC: {
            ans +=
                show(atom.getArg_A()) + " " +
                show(atom.getArg_B()) + " " +
                show(atom.getArg_C());
            break;
        }
        case abyss::iABx: {
            ans +=
                show(atom.getArg_A()) + " " +
                show(atom.getArg_Bx());
            break;
        }
        case abyss::iAsBx: {
            ans +=
                show(atom.getArg_A()) + " " +
                show(atom.getArg_sBx());
            break;
        }
        case abyss::iAx: {
            ans += show(atom.getArg_Ax());
            break;
        }
        case abyss::isJ: {
            ans += show(atom.getArg_sJ());
            break;
        }
        }//end switch
        return ans;
    }

    template<>
    inline abyss::string show(const abyss::Instruction &instr) {
        abyss::string ans = "";
        /*unimplemented!*/
        return "<Instruction>";
    }
}



namespace abyss {
    namespace test {
        namespace instruction {
            inline void test_iABC() {
                Atom i = Atom::create_iABC(OP_ADD, 0, 1, 2, 0);
                ensure(i.mode == iABC, "error> Instruction test failed");
                i.getOpCode();
                i.getArg_A();
                i.getArg_B();
                i.getArg_C();
                Show::show(i);
            }
            inline void test() {
                test_iABC();
            }
        }
    }
}

#endif
