#ifndef CONFIG_HPP
#define CONFIG_HPP

//#include "Limit.hpp"
#include <string>
#include <variant>
#include <optional>
#include <functional>
#include <vector>
#include <list>
#include <unordered_map>


namespace abyss {

    using string = std::string;

    template<class ...T>
    using variant = std::variant<T...>; //C++ 17

    template<class T>
    using optional = std::optional<T>; //C++ 17
    inline constexpr std::nullopt_t nothing = std::nullopt;

    using std::function;

    /* chars used as small naturals (so that 'char' is reserved for characters) */
    using U8 = unsigned char;
    using I8 =   signed char;

    using U32 = unsigned int;
    using I32 =   signed int;

    using U16 = unsigned short;
    using I16 =   signed short;

    using U64 = unsigned long;
    using I64 =   signed long;

    using F32 = float;
    using F64 = double;

    using MemSize = size_t;

    template<class ...T>
    using shared_ptr = std::shared_ptr<T...>;

    template<class ...T>
    using vector = std::vector<T...>;

    template<class ...T>
    using list = std::list<T...>;

    template<class ...T>
    using unordered_map = std::unordered_map<T...>;

    using std::make_shared;
}



#endif
