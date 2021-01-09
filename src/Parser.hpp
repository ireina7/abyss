#ifndef PARSER_HPP
#define PARSER_HPP

#include "Object.hpp"
#include <memory>
#include <utility>
#include <cctype>


/**
   I think the parser should not be relevant with State(thread),
   Therefore I am wondering whether we should reuse Value for SExpr structure...

   @author:: Ireina
 */
namespace abyss {

    enum SExprType { SBool, SInt, SReal, SString, SVar, SList, SNone };
    struct VarDesc {
        string name;
        VarDesc(string s = "<unknown>"): name(s) {}
    };
    struct SExpr;
    using SExprList = vector<shared_ptr<SExpr>>;
    struct SExpr {
        SExprType type; //may be unnecessary...
        variant <
            Integer,
            Real,
            Bool,
            string,
            VarDesc,
            SExprList
            > val;
        /*
          VarDesc var;
          vector<shared_ptr<SExpr>> list;
        */
        string error_msg;
        Bool isK = false;
        SExprType getType() const {
            return this->type;
        }
        explicit SExpr(SExprType t = SNone): type(t) {}
        SExpr(const Bool    &b): type(SBool),   val(b) {}
        SExpr(const string  &s): type(SString), val(s) {}
        SExpr(const Integer &i): type(SInt),    val(i) {}
        SExpr(const Real    &r): type(SReal),   val(r) {}
        SExpr(const VarDesc &v): type(SVar),    val(v) {}

        bool isDefinition() const;
    };
    const SExpr SExprNone = SExpr(SNone);
}

namespace Cast {

    template<class To>
    To to(const abyss::SExpr &sexpr) {}
    template<class To>
    To to(abyss::SExpr &sexpr) {}

    template<>
    inline abyss::string &to(abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SString);
        return std::get<abyss::string>(exp.val);
    }
    template<>
    inline abyss::Integer &to(abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SInt);
        return std::get<abyss::Integer>(exp.val);
    }
    template<>
    inline abyss::Real &to(abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SReal);
        return std::get<abyss::Real>(exp.val);
    }
    template<>
    inline abyss::Bool &to(abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SBool);
        return std::get<abyss::Bool>(exp.val);
    }
    template<>
    inline abyss::VarDesc &to(abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SVar);
        return std::get<abyss::VarDesc>(exp.val);
    }
    template<>
    inline abyss::SExprList &to(abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SList);
        return std::get<abyss::SExprList>(exp.val);
    }

    template<>
    inline const abyss::Integer &to(const abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SInt);
        return std::get<abyss::Integer>(exp.val);
    }
    template<>
    inline const abyss::Real &to(const abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SReal);
        return std::get<abyss::Real>(exp.val);
    }
    template<>
    inline const abyss::Bool &to(const abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SBool);
        return std::get<abyss::Bool>(exp.val);
    }
    template<>
    inline const abyss::string &to(const abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SString);
        return std::get<abyss::string>(exp.val);
    }
    template<>
    inline const abyss::VarDesc &to(const abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SVar);
        return std::get<abyss::VarDesc>(exp.val);
    }
    template<>
    inline const abyss::SExprList &to(const abyss::SExpr &exp) {
        abyss::ensure(exp.getType() == abyss::SList);
        return std::get<abyss::SExprList>(exp.val);
    }

    template<>
    inline abyss::Integer to(abyss::SExpr &exp) {
        return to<abyss::Integer&>(exp);
    }
    template<>
    inline abyss::Real to(abyss::SExpr &exp) {
        return to<abyss::Real&>(exp);
    }
    template<>
    inline abyss::Bool to(abyss::SExpr &exp) {
        return to<abyss::Bool&>(exp);
    }
}



namespace Show {

    template<>
    inline abyss::string show(const abyss::SExpr &exp) {

        abyss::string ans = "";
        switch(exp.getType()) {
        case abyss::SNone: {
            ans = "None";
            break;
        }
        case abyss::SBool: {
            ans = (exp.isK ? "$" : "") + show(Cast::to<const abyss::Bool&>(exp));
            break;
        }
        case abyss::SInt: {
            ans = (exp.isK ? "$" : "") + show(Cast::to<const abyss::Integer&>(exp));
            break;
        }
        case abyss::SReal: {
            ans = (exp.isK ? "$" : "") + show(Cast::to<const abyss::Real&>(exp));
            break;
        }
        case abyss::SString: {
            ans = (exp.isK ? "$" : "") + show(Cast::to<const abyss::string&>(exp));
            break;
        }
        case abyss::SVar: {
            ans = show(Cast::to<const abyss::VarDesc&>(exp).name);
            break;
        }
        case abyss::SList: {
            ans = "(";
            for(auto &x : Cast::to<const abyss::SExprList&>(exp)) {
                ans += show(*x);
                ans += " ";
            }
            if(ans.size() > 1) ans[ans.size() - 1] = ')';
            else ans += ")";
            break;
        }
        default: {
            ans = "<Unknow Abyss SExpr>";
        }
        }
        return ans;
    }
}




namespace abyss {

    std::pair<SExpr, abyss::string::size_type>
    parseSExpr(abyss::string src, abyss::string::size_type start, I32 level);
}





#endif
