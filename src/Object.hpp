#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <bitset>
#include "Abyss.hpp"
#include "OpCode.hpp"
#include "Instruction.hpp"
#include "Config.hpp"
#include "Utils.hpp"


namespace abyss {

    namespace object {
        enum Type {
              TNONE           = -1,
              TNIL            = 0,
              TBOOLEAN	      = 1,
              TLIGHTUSERDATA  = 2,
              TINTEGER        = 3,
              TNUMBER	      = 4,
              TSTRING	      = 5,
              TTABLE	      = 6,
              TFUNCTION	      = 7,
              TUSERDATA	      = 8,
              TTHREAD	      = 9,
              NUMTYPES	      = 10,
        };
        /*
        ** tags for Tagged Values have the following use of bits:
        ** bits 0-3: actual tag (a ABYSS_T* constant)
        ** bits 4-5: variant bits
        ** bit 6: whether value is collectable
        */

        struct Tag {
            U8 tag;
            Tag(): tag(TNONE) {}
            Tag(const Type &t): tag(t) {}
            Tag(Type t, U8 var, bool canCollect)
                : tag(static_cast<U8>(t) | (var << 4) | (canCollect << 6)) {}
            bool canCollect() const {
                return (tag >> 6) % 2 == 1;
            }
            Tag &setVar(I8 var) {
                tag = static_cast<Type>(tag | (var << 4));
                return *this;
            }
            Type getType() const {
                /**
                   This is evil trick! I think there's sth wrong with static_cast of enums.
                   So currently use magic number 15 here.
                   @author: Ireina
                 */
                if(static_cast<I32>(tag & 0x0F) == 15) return TNONE;
                return static_cast<Type>(tag & 0x0F);
            }
            bool operator==(const Tag &that) const {
                return that.tag == this->tag;
            }
        };
    }


    /* Common type for all collectable objects */
    struct GCObject {
        object::Tag tag;
        GCObject *next;
        U8 marked = 0;
        GCObject() {}
        explicit GCObject(const object::Tag &tag): tag(tag) {}

        object::Type getType() const {
            return tag.getType();
        }
        virtual ~GCObject() {}
    };

    /**
       Defaults, change if u do not like!
     */
    using Bool = bool;
    using Integer = I32;
    using Real = F64;

    struct String : public GCObject {
        abyss::string val;
        String() {}
        String(const abyss::string& s): GCObject(object::TSTRING), val(s) {}
        String(const char *c_str): GCObject(object::TSTRING), val(abyss::string(c_str)) {}
    };

    using ValueUnion = abyss::variant <
        //shared_ptr<GCObject>,
        GCObject*,
        Bool,
        Integer,
        Real
    >;





    /*
    ** Description of a local variable for function prototypes
    ** (used for debug information)
    */
    struct LocVar {
        std::string name;
        I32 startpc;  /* first point where variable is active */
        I32 endpc;    /* first point where variable is dead */
    };

    /*
    ** Associates the absolute line source for a given instruction ('pc').
    ** The array 'lineinfo' gives, for each instruction, the difference in
    ** lines from the previous instruction. When that difference does not
    ** fit into a byte, Abyss saves the absolute line for that instruction.
    ** (Abyss also saves the absolute line periodically, to speed up the
    ** computation of a line number: we can use binary search in the
    ** absolute-line array, but we must traverse the 'lineinfo' array
    ** linearly to compute a line.)
    */
    struct AbsLineInfo {
        int pc;
        int line;
    };

    struct Value;
    struct Upvaldesc;
    struct Lambda : public GCObject {
        std::string name;
        U8 n_params;  /* number of fixed (named) parameters */
        U8 is_vararg;
        U8 max_stacksize;  /* number of registers needed by this function */
        I32 size_lineinfo;
        I32 size_abslineinfo;  /* size of 'abslineinfo' */
        I32 line_defined;  /* debug information  */
        I32 last_linedefined;  /* debug information  */
        unordered_map<string, I32> sym_table;
        vector<Value> k;  /* constants used by the function */
        vector<Atom> code;  /* opcodes */
        vector<shared_ptr<Lambda>> p;  /* functions defined inside the function */
        vector<Upvaldesc> upvalues;  /* upvalue information */
        U8 *lineinfo;  /* information about source lines (debug information) */
        AbsLineInfo *abslineinfo;  /* idem */
        vector<LocVar> locvars;  /* information about local variables (debug information) */
        GCObject *gclist;
        Lambda(): GCObject(object::TFUNCTION) {}
        Lambda(const Lambda &that) = default;
    };


    struct Closure : public GCObject {
        U8 n_upvals;
        GCObject *gclist;
        Closure(): GCObject(object::TFUNCTION) {}
        Closure(const object::Tag &tag): GCObject(tag) {}
        virtual ~Closure() {}
    };

    const object::Tag TFUNCTION_CCLOSURE =
        object::Tag(object::TFUNCTION, 0, true);

    const object::Tag TFUNCTION_LCLOSURE =
        object::Tag(object::TFUNCTION, 1, true);

    struct CClosure : public Closure {
        CFunction f;
        Value *upvals;
        CClosure(): Closure(TFUNCTION_CCLOSURE) {}
    };

    struct UpVal;
    struct LClosure : public Closure {
        Lambda lam;
        vector<UpVal> upvals;
        LClosure(): Closure(TFUNCTION_LCLOSURE) {}
        LClosure(const Lambda &lam): Closure(TFUNCTION_LCLOSURE), lam(lam) {}
    };




    struct StackValue;
    struct Value {
        object::Tag tag;
        ValueUnion val;
        Value(): tag(object::TNONE) {}
        explicit Value(object::Tag t): tag(t) {}
        Value(const Value &) = default;
        Value(const Bool &b): tag(object::TBOOLEAN) {
            this->val = b;
        }
        Value(const Integer &i): tag(object::TINTEGER) {
            this->val = i;
        }
        Value(const Real &r): tag(object::TNUMBER) {
            this->val = r;
        }
        Value(String *const s): tag(object::TSTRING) {
            this->val = s;
            setCollect(true);
        }
        Value(Lambda *const lam): tag(object::TFUNCTION) {
            this->val = lam;
            setCollect(true);
        }
        Value(Closure *const cl): tag(object::TFUNCTION) {
            this->val = cl;
            setCollect(true);
        }
        Value(CClosure *const ccl): tag(TFUNCTION_CCLOSURE) {
            this->val = ccl;
            setCollect(true);
        }
        Value(LClosure *const lcl): tag(TFUNCTION_LCLOSURE) {
            this->val = lcl;
            setCollect(true);
        }

        object::Type getType() const {
            return tag.getType();
        }
        Value &setCollect(bool can) {
            tag.tag |= (can << 6);
            return *this;
        }
        ~Value() {}
    };



    /*
    ** Upvalues for Abyss closures
    */
    struct UpVal : public Value {
        U8 tbc;  /* true if it represents a to-be-closed variable */
        Value *v;
        struct {  // (when open)
            UpVal *next;  // linked list
            UpVal *prev;
        } open;
        Value value;  /* the value (when closed) */
        UpVal() {}
        UpVal(const Value &val): value(val) {}
        UpVal(Value *vp): v(vp) {}
        bool isOpen() const {
            return v != &value; //What a trick!
        }
    };

    /*
    ** Description of an upvalue for function prototypes
    */
    struct Upvaldesc {
        abyss::string name;  /* upvalue name (for debug information) */
        UpVal upval;
        bool instack;        /* whether it is in stack (register) */
        U8 idx;            /* index of upvalue (in stack or in outer function's list) */
        U8 kind;           /* kind of corresponding variable */
    };

    const Value Nil = Value(object::Tag(object::TNIL));
    const Value None = Value();


    namespace closure {

        shared_ptr<LClosure>
        inline mainClosure() {
            auto main_cl = make_shared<LClosure>();

            return main_cl;
        }
    }

}



namespace Show {

    template<>
    inline abyss::string show(const abyss::object::Type &t) {
        abyss::string ans = "";
        switch(t) {
        case abyss::object::TNONE: {
            ans = "None";
            break;
        }
        case abyss::object::TNIL: {
            ans = "Nil";
            break;
        }
        case abyss::object::TBOOLEAN: {
            ans = "Boolean";
            break;
        }
        case abyss::object::TINTEGER: {
            ans = "Integer";
            break;
        }
        case abyss::object::TNUMBER: {
            ans = "Real";
            break;
        }
        case abyss::object::TSTRING: {
            ans = "String";
            break;
        }
        case abyss::object::TFUNCTION: {
            ans = "Function";
            break;
        }
        default: {
            ans = "<Unknow type: " + show(static_cast<abyss::I32>(t)) + ">";
        }
        }
        return ans;
    }

    template<>
    inline abyss::string show(const abyss::Upvaldesc &uvd) {
        abyss::string ans = "";
        return show(uvd.name) + ": " + show(uvd.idx) +
            (uvd.instack ? " in" : " out") + " stack";
    }
}

namespace Cast {

    /**
       We can provide converting functions with better error msgs here.
     */

    /**
       From abyss::Values
     */
#define ensure_or_report(t, tt, v)\
    abyss::ensure(v.getType() == abyss::object::t,\
        "error> Converting failed:\n" +\
                abyss::notMatch(#tt, Show::show(v.getType())))

#define ensure_ptr_or_report(t, tt, v)\
    abyss::ensure(v->getType() == abyss::object::t,\
        "error> Converting failed:\n" +\
                abyss::notMatch(#tt, Show::show(v->getType())))

#define ensure_gc_or_report(v)\
    abyss::ensure(v.tag.canCollect(),\
                    "error> Converting failed:\n" +\
                    abyss::notMatch("GCObject", Show::show(v.getType())))

    template<class To>
    To to(abyss::Value &v) {}
    template<class To>
    To to(const abyss::Value &v) {}

    template<>
    inline abyss::Bool to(abyss::Value &v) {
        ensure_or_report(TBOOLEAN, Boolean, v);
        return std::get<abyss::Bool>(v.val);
    }
    template<>
    inline abyss::Integer to(abyss::Value &v) {
        ensure_or_report(TINTEGER, Integer, v);
        return std::get<abyss::Integer>(v.val);
    }
    template<>
    inline abyss::Real to(abyss::Value &v) {
        ensure_or_report(TNUMBER, Real, v);
        return std::get<abyss::Real>(v.val);
    }
    template<>
    inline abyss::GCObject *to(abyss::Value &v) {
        ensure_gc_or_report(v);
        return std::get<abyss::GCObject*>(v.val);
    }

    template<>
    inline abyss::Bool to(const abyss::Value &v) {
        ensure_or_report(TBOOLEAN, Boolean, v);
        return std::get<abyss::Bool>(v.val);
    }
    template<>
    inline abyss::Integer to(const abyss::Value &v) {
        ensure_or_report(TINTEGER, Integer, v);
        return std::get<abyss::Integer>(v.val);
    }
    template<>
    inline abyss::Real to(const abyss::Value &v) {
        ensure_or_report(TNUMBER, Real, v);
        return std::get<abyss::Real>(v.val);
    }
    template<>
    inline abyss::GCObject *to(const abyss::Value &v) {
        ensure_gc_or_report(v);
        return std::get<abyss::GCObject*>(v.val);
    }


    /**
       From abyss::Value Views (marked as const)
     */
    template<>
    inline const abyss::Bool &to(abyss::Value &v) {
        ensure_or_report(TBOOLEAN, Boolean, v);
        return std::get<abyss::Bool>(v.val);
    }
    template<>
    inline const abyss::Integer &to(abyss::Value &v) {
        ensure_or_report(TINTEGER, Integer, v);
        return std::get<abyss::Integer>(v.val);
    }
    template<>
    inline const abyss::Real &to(abyss::Value &v) {
        ensure_or_report(TNUMBER, Real, v);
        return std::get<abyss::Real>(v.val);
    }

    template<>
    inline const abyss::GCObject &to(abyss::Value &v) {
        ensure_gc_or_report(v);
        return *to<abyss::GCObject*>(v);
    }
    template<>
    inline const abyss::GCObject &to(const abyss::Value &v) {
        ensure_gc_or_report(v);
        return *to<abyss::GCObject*>(v);
    }
    template<>
    inline abyss::GCObject &to(abyss::Value &v) {
        ensure_gc_or_report(v);
        return *to<abyss::GCObject*>(v);
    }


    /**
       From GCObjects
     */
    template<class To>
    To to(abyss::GCObject &gc) {}
    template<class To>
    To to(const abyss::GCObject &gc) {}

    template<class To>
    To to(abyss::GCObject *gc) {}
    template<class To>
    To to(const abyss::GCObject *gc) {}

    template<>
    inline const abyss::Lambda &to(abyss::GCObject &gc) {
        ensure_or_report(TFUNCTION, Function, gc);
        return dynamic_cast<const abyss::Lambda&>(gc);
    }
    template<>
    inline const abyss::Lambda &to(const abyss::GCObject &gc) {
        ensure_or_report(TFUNCTION, Function, gc);
        return dynamic_cast<const abyss::Lambda&>(gc);
    }
    template<>
    inline const abyss::Lambda &to(abyss::Value &v) {
        ensure_or_report(TFUNCTION, Function, v);
        return to<const abyss::Lambda&>(to<abyss::GCObject&>(v));
    }
    template<>
    inline const abyss::Lambda &to(const abyss::Value &v) {
        ensure_or_report(TFUNCTION, Function, v);
        return to<const abyss::Lambda&>(to<const abyss::GCObject&>(v));
    }
    template<>
    inline const abyss::Closure &to(abyss::GCObject &gc) {
        abyss::ensure(gc.tag == abyss::TFUNCTION_CCLOSURE ||
                    gc.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("Closure", Show::show(gc.getType())));
        return dynamic_cast<const abyss::Closure&>(gc);
    }
    template<>
    inline const abyss::Closure &to(const abyss::GCObject &gc) {
        abyss::ensure(gc.tag == abyss::TFUNCTION_CCLOSURE ||
                    gc.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("Closure", Show::show(gc.getType())));
        return dynamic_cast<const abyss::Closure&>(gc);
    }
    template<>
    inline const abyss::LClosure &to(const abyss::GCObject &gc) {
        abyss::ensure(gc.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("LClosure", Show::show(gc.getType())));
        return dynamic_cast<const abyss::LClosure&>(to<const abyss::Closure&>(gc));
    }
    template<>
    inline abyss::Closure &to(abyss::GCObject &gc) {
        abyss::ensure(gc.tag == abyss::TFUNCTION_CCLOSURE ||
                    gc.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("Closure", Show::show(gc.getType())));
        return dynamic_cast<abyss::Closure&>(gc);
    }
    template<>
    inline const abyss::Closure &to(abyss::Value &v) {
        abyss::ensure(v.tag == abyss::TFUNCTION_CCLOSURE ||
                    v.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("Closure", Show::show(v.getType())));
        return to<const abyss::Closure&>(to<abyss::GCObject&>(v));
    }
    template<>
    inline const abyss::Closure &to(const abyss::Value &v) {
        abyss::ensure(v.tag == abyss::TFUNCTION_CCLOSURE ||
                    v.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("Closure", Show::show(v.getType())));
        return to<const abyss::Closure&>(to<const abyss::GCObject&>(v));
    }
    template<>
    inline abyss::Closure &to(abyss::Value &v) {
        abyss::ensure(v.tag == abyss::TFUNCTION_CCLOSURE ||
                    v.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("Closure", Show::show(v.getType())));
        return to<abyss::Closure&>(to<abyss::GCObject&>(v));
    }



    template<>
    inline const abyss::String *to(abyss::GCObject *gc) {
        ensure_ptr_or_report(TSTRING, String, gc);
        return dynamic_cast<abyss::String*>(gc);
    }
    template<>
    inline const abyss::String *to(const abyss::GCObject *gc) {
        ensure_ptr_or_report(TSTRING, String, gc);
        return dynamic_cast<const abyss::String*>(gc);
    }
    template<>
    inline abyss::String *to(abyss::GCObject *gc) {
        ensure_ptr_or_report(TSTRING, String, gc);
        return dynamic_cast<abyss::String*>(gc);
    }
    template<>
    inline const abyss::String &to(abyss::GCObject &gc) {
        ensure_or_report(TSTRING, String, gc);
        return dynamic_cast<const abyss::String&>(gc);
    }
    template<>
    inline const abyss::String &to(const abyss::GCObject &gc) {
        ensure_or_report(TSTRING, String, gc);
        return dynamic_cast<const abyss::String&>(gc);
    }

    template<>
    inline const abyss::String &to(abyss::Value &v) {
        ensure_or_report(TSTRING, String, v);
        return to<const abyss::String&>(to<abyss::GCObject&>(v));
    }
    template<>
    inline const abyss::String &to(const abyss::Value &v) {
        ensure_or_report(TSTRING, String, v);
        return to<const abyss::String&>(to<const abyss::GCObject&>(v));
    }


    /**
       to Closures
     */
    template<class To>
    To to(abyss::Closure &cl) {}
    template<class To>
    To to(const abyss::Closure &cl) {}

    /* Views */
    template<>
    inline const abyss::CClosure &to(abyss::Closure &cl) {
        abyss::ensure(cl.tag == abyss::TFUNCTION_CCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("CClosure", Show::show(cl.getType())));
        return dynamic_cast<const abyss::CClosure&>(cl);
    }
    template<>
    inline const abyss::LClosure &to(abyss::Closure &cl) {
        abyss::ensure(cl.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("LClosure", Show::show(cl.getType())));
        return dynamic_cast<const abyss::LClosure&>(cl);
    }
    template<>
    inline abyss::LClosure &to(abyss::Closure &cl) {
        abyss::ensure(cl.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("LClosure", Show::show(cl.getType())));
        return dynamic_cast<abyss::LClosure&>(cl);
    }
    template<>
    inline const abyss::CClosure &to(const abyss::Closure &cl) {
        abyss::ensure(cl.tag == abyss::TFUNCTION_CCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("CClosure", Show::show(cl.getType())));
        return dynamic_cast<const abyss::CClosure&>(cl);
    }
    template<>
    inline const abyss::LClosure &to(const abyss::Closure &cl) {
        abyss::ensure(cl.tag == abyss::TFUNCTION_LCLOSURE,
                    "error> Converting failed:\n" +
                    abyss::notMatch("LClosure", Show::show(cl.getType())));
        return dynamic_cast<const abyss::LClosure&>(cl);
    }

    template<>
    inline const abyss::CClosure &to(abyss::Value &v) {
        return to<const abyss::CClosure&>(to<abyss::Closure&>(v));
    }
    template<>
    inline const abyss::LClosure &to(abyss::Value &v) {
        return to<const abyss::LClosure&>(to<abyss::Closure&>(v));
    }
    template<>
    inline const abyss::CClosure &to(const abyss::Value &v) {
        return to<const abyss::CClosure&>(to<const abyss::Closure&>(v));
    }
    template<>
    inline const abyss::LClosure &to(const abyss::Value &v) {
        return to<const abyss::LClosure&>(to<const abyss::Closure&>(v));
    }
    template<>
    inline abyss::LClosure &to(abyss::Value &v) {
        return to<abyss::LClosure&>(to<abyss::Closure&>(v));
    }

}

/**
   Debug output of abyss values
 */
namespace Show {


    template<>
    inline abyss::string show(const abyss::Bool &b) {
        return b ? "True" : "False";
    }
    template<>
    inline abyss::string show(const abyss::String &s) {
        return s.val;
    }
    template<>
    inline abyss::string show(const abyss::Lambda &f) {
        return "<function>";
    }
    template<>
    inline abyss::string show(const abyss::Closure &cl) {

        return "<closure>";
    }
    template<>
    inline abyss::string show(const abyss::CClosure &cl) {
        return "<C closure>";
    }
    template<>
    inline abyss::string show(const abyss::LClosure &cl) {
        return "<closure: " + cl.lam.name + ">";
    }
    template<>
    inline abyss::string show(const abyss::GCObject &gc) {
        using Cast::to;

        abyss::string ans = "";
        switch(gc.getType()) {
        case abyss::object::TFUNCTION: {
            if(gc.tag == abyss::TFUNCTION_LCLOSURE) {
                ans = show(Cast::to<const abyss::LClosure&>(gc));
            }
            else {
                ans = "<function>";
            }
            break;
        }
        case abyss::object::TSTRING: {
            ans = to<const abyss::String&>(gc).val;
            break;
        }
        default: {
            ans = "<unknown GC object>";
            break;
        }
        }
        return ans;
    }
    template<>
    inline abyss::string show(const abyss::Value &v) {
        using Cast::to;
        abyss::string ans;
        switch(v.getType()) {

        case abyss::object::TNONE: {
            ans = "None";
            break;
        }
        case abyss::object::TNIL: {
            ans = "Nil";
            break;
        }
        case abyss::object::TBOOLEAN: {
            ans = show(to<abyss::Bool>(v));
            break;
        }
        case abyss::object::TINTEGER: {
            ans = show(to<abyss::Integer>(v));
            break;
        }
        case abyss::object::TNUMBER: {
            ans = show(to<abyss::Real>(v));
            break;
        }
        case abyss::object::TSTRING: {
            ans = show(to<const abyss::String&>(v));
            break;
        }
        case abyss::object::TFUNCTION: {
            if(v.tag == abyss::TFUNCTION_CCLOSURE) {
                ans = show(to<const abyss::CClosure&>(v));
                break;
            }
            if(v.tag == abyss::TFUNCTION_LCLOSURE) {
                ans = show(to<const abyss::LClosure&>(v));
                break;
            }
            ans = show(to<const abyss::Lambda&>(v));
            break;
        }
        default: {
            ans = "<abyss unknown value: " + show(v.getType()) + ">";
        }
        }
        return ans;
    }
}


#endif
