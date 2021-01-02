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

    enum SExprType { SBool, SInt, SReal, SVar, SList, SNone };
    using VarDesc = variant<Integer, Real, Bool, string>;
    struct SExpr {
        SExprType type;
        VarDesc var;
        vector<shared_ptr<SExpr>> list;
        string error_msg;
        Bool isK = false;
        SExprType getType() const {
            return this->type;
        }
        explicit SExpr(SExprType t = SNone): type(t) {}
        SExpr(const VarDesc &var): type(SVar), var(var) {}
    };
    const SExpr SExprNone = SExpr(SNone);

}

namespace Cast {

    template<class To>
    To to(const abyss::VarDesc &vd) {}

    template<>
    inline const abyss::string &to(const abyss::VarDesc &vd) {
        return std::get<abyss::string>(vd);
    }
    template<>
    inline const abyss::Integer &to(const abyss::VarDesc &vd) {
        return std::get<abyss::Integer>(vd);
    }
    template<>
    inline const abyss::Real &to(const abyss::VarDesc &vd) {
        return std::get<abyss::Real>(vd);
    }
    template<>
    inline const abyss::Bool &to(const abyss::VarDesc &vd) {
        return std::get<abyss::Bool>(vd);
    }
    template<>
    inline abyss::Integer to(const abyss::VarDesc &vd) {
        return std::get<abyss::Integer>(vd);
    }
    template<>
    inline abyss::Real to(const abyss::VarDesc &vd) {
        return std::get<abyss::Real>(vd);
    }
    template<>
    inline abyss::Bool to(const abyss::VarDesc &vd) {
        return std::get<abyss::Bool>(vd);
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
            ans = (exp.isK ? "$" : "") + show(Cast::to<const abyss::Bool&>(exp.var));
            break;
        }
        case abyss::SInt: {
            ans = (exp.isK ? "$" : "") + show(Cast::to<const abyss::Integer&>(exp.var));
            break;
        }
        case abyss::SReal: {
            ans = (exp.isK ? "$" : "") + show(Cast::to<const abyss::Real&>(exp.var));
            break;
        }
        case abyss::SVar: {
            ans = show(Cast::to<const abyss::string&>(exp.var));
            break;
        }
        case abyss::SList: {
            ans = "(";
            for(auto &x : exp.list) {
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
