#ifndef STACK_HPP
#define STACK_HPP

#include "Object.hpp"
//#include "VM.hpp"
//#include "GC.hpp"


namespace abyss {

    /*
    ** Entries in the Abyss stack
    */
    struct StackValue {
        Value val;
        StackValue() {}
        StackValue(const Value &v): val(v) {}
        object::Type getType() {
            return val.tag.getType();
        }
        object::Tag getTag() {
            return val.tag;
        }
    };
    /* index to stack elements */
    //using StkId = StackValue*;
    using StkId  = vector<StackValue>::iterator;
    using StkIdx = vector<StackValue>::size_type;
    constexpr I32 STACK_SIZE = 100;
}

namespace Cast {

    template<class T>
    T to(const abyss::StackValue &sv) {}
    template<class T>
    T to(abyss::StackValue &sv) {}

    template<>
    inline abyss::Value& to(abyss::StackValue &sv) {
        return sv.val;
    }
    template<>
    inline const abyss::Value& to(abyss::StackValue &sv) {
        return sv.val;
    }
    template<>
    inline const abyss::Value& to(const abyss::StackValue &sv) {
        return sv.val;
    }
}

namespace Show {

    template<>
    inline abyss::string show(const abyss::StackValue &sv) {
        return show(sv.val);
    }
}




#endif
