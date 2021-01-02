#ifndef VM_HPP
#define VM_HPP

#include "Abyss.hpp"
#include "Object.hpp"
#include "State.hpp"
#include "Instruction.hpp"
#include "GC.hpp"
#include <algorithm>

/*
#define RA(i)	(base+GETARG_A(i))
#define RB(i)	(base+GETARG_B(i))
#define vRB(i)	s2v(RB(i))
#define KB(i)	(k+GETARG_B(i))
#define RC(i)	(base+GETARG_C(i))
#define vRC(i)	s2v(RC(i))
#define KC(i)	(k+GETARG_C(i))
#define RKC(i)	((TESTARG_k(i)) ? k + GETARG_C(i) : s2v(base + GETARG_C(i)))
*/

namespace abyss {
    namespace vm {
        void execute(State &S, CallInfo *ci);
    }
}



#endif
