#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <cassert>
#include <stdexcept>

#include "Config.hpp"

namespace Show {
    template<class T>
    inline abyss::string show(const T &x) {
        return std::to_string(x);
    }
    /*
    template<class T>
    abyss::string show(const T &&x) {
        std::cout << "!!!" << std::endl;
        return std::to_string(x);
    }
    */

    template<class T>
    inline void print(const T &x) {
        std::cout << show(x);
    }
    template<class T>
    inline void println(const T &x) {
        std::cout << show(x) << std::endl;
    }

    inline abyss::string show(const char *c_str) {
        return std::string(c_str);
    }
    inline void print(const char *c_str) {
        std::cout << c_str;
    }
    inline void println(const char *c_str) {
        std::cout << c_str << std::endl;
    }
    template<>
    inline abyss::string show(const abyss::string &s) {
        return s;
    }
}

namespace Cast {

    //template<class To, class From>
    //To &to(From &a); //Just declaration!
}




namespace abyss {

#define abyss_assert(exp) assert(exp)


    inline void ensure(int exp) {
        //assert(exp);
        if(!exp) std::throw_with_nested
                     (std::runtime_error(""));
    }
    inline void ensure(int exp, const char *msg) {
        if(!exp) std::throw_with_nested
                     (std::runtime_error(msg));
    }
    inline void ensure(int exp, const abyss::string &msg) {
        if(!exp) std::throw_with_nested(std::runtime_error(msg));
    }

    inline void error(const char *msg) {
        ensure(false, msg);
    }
    inline void error(const abyss::string &msg) {
        ensure(false, msg);
    }

    inline void report(const char *msg) {
        Show::println(msg);
    }
    inline void report(const abyss::string &msg) {
        Show::println(msg);
    }

    inline abyss::string notMatch(const char *A, const char *B) {
        const abyss::string M = A;
        return
            "\trequired: " + M + "\n" +
            "\tactually: " + B;
    }
    inline abyss::string notMatch(const abyss::string &A, const abyss::string &B) {
        return notMatch(A.c_str(), B.c_str());
    }
    inline abyss::string notMatch(const char *A, const abyss::string &B) {
        return notMatch(A, B.c_str());
    }
    inline abyss::string notMatch(const abyss::string &A, const char *B) {
        return notMatch(A.c_str(), B);
    }

}


#endif
