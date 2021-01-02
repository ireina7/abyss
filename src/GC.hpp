#ifndef GC_HPP
#define GC_HPP

#include "Object.hpp"
#include "Config.hpp"
//#include "State.hpp"


namespace abyss {

    enum Color { White = 0, Grey, Black };

    struct State;
    I32 traverseStack(State &S, I32 bound = -1);

    I32 traverseAll(State &S);

    I32 collect(State &S);
}

#endif
