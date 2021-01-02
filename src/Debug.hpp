#ifndef DEBUG_HPP
#define DEBUG_HPP

#define DEBUG_IS_NECESSARY

#include "Utils.hpp"
#include "Instruction.hpp"

using Show::print;
using Show::println;

namespace abyss {
    inline int tests() {
#ifdef DEBUG_IS_NECESSARY
        report("abyss> Running necessary tests...");
        abyss::test::instruction::test();
        report("abyss> ...Ok");
#endif
        return 0;
    }
}

void print_exception(const std::exception& e, int level =  0)
{
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch(const std::exception& e) {
        print_exception(e, level+1);
    }
}

#endif
