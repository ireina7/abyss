#ifndef ABYSS_HPP
#define ABYSS_HPP

#include "Limit.hpp"
#include "Utils.hpp"


#define ABYSS_VERSION_MAJOR	"0"
#define ABYSS_VERSION_MINOR	"1"
#define ABYSS_VERSION_RELEASE	"2"

#define ABYSS_VERSION_NUM			504
#define ABYSS_VERSION_RELEASE_NUM		(ABYSS_VERSION_NUM * 100 + 0)

#define ABYSS_VERSION	"Abyss " ABYSS_VERSION_MAJOR "." ABYSS_VERSION_MINOR
#define ABYSS_RELEASE	ABYSS_VERSION "." ABYSS_VERSION_RELEASE
#define ABYSS_COPYRIGHT	ABYSS_RELEASE "  Copyright (C) 2020 Abyss, Ireina"
#define ABYSS_AUTHORS	"Ireina"

#define ABYSS_KCONTEXT	ptrdiff_t


namespace abyss {

    struct State;
    using KContext = ABYSS_KCONTEXT;
    using CFunction = int (*)(State *S);
    using KFunction = int (*)(State *S, int status, KContext ctx);

    inline void printAbyssInfo() {
        string prompt = "abyss> ";
        Show::println("Welcome to abyss!");
        Show::println(ABYSS_COPYRIGHT);
        Show::println("Made in Abyss.");
    }
}



#endif
