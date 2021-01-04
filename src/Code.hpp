#ifndef CODE_HPP
#define CODE_HPP

#include "Object.hpp"
#include "Parser.hpp"
#include "Utils.hpp"
#include <unordered_map>
#include <utility>
#include <functional>


namespace abyss {

    enum EnvIndexType
        { ErrorIndex = -1, RegIndex = 0, KIndex = 1, LamIndex = 2, UpValIndex = 3 };
    struct EnvIndex {
        EnvIndexType type;
        I32 val;
        EnvIndex(): type(RegIndex), val(0) {}
        EnvIndex(EnvIndexType t, I32 v): type(t), val(v) {}
    };
    const EnvIndex errorIdx = EnvIndex(ErrorIndex, -1);

}

namespace Show {
    template<>
    inline abyss::string show(const abyss::EnvIndexType &et) {
        abyss::string ans = "";
        switch(et) {
        case abyss::ErrorIndex: {
            ans = "ErrorIndex";
            break;
        }
        case abyss::RegIndex: {
            ans = "RegIndex";
            break;
        }
        case abyss::KIndex: {
            ans = "KIndex";
            break;
        }
        case abyss::LamIndex: {
            ans = "LamIndex";
            break;
        }
        case abyss::UpValIndex: {
            ans = "UpValIndex";
            break;
        }
        }
        return ans;
    }

    template<>
    inline abyss::string show(const abyss::EnvIndex &ei) {
        abyss::string ans = "";
        switch(ei.type) {
        case abyss::ErrorIndex: {
            ans = "?";
            break;
        }
        case abyss::RegIndex: {
            ans = "%" + show(ei.val);
            break;
        }
        case abyss::KIndex: {
            ans = "$" + show(ei.val);
            break;
        }
        case abyss::LamIndex: {
            ans = "\\" + show(ei.val);
            break;
        }
        case abyss::UpValIndex: {
            ans = "^" + show(ei.val);
            break;
        }
        }
        return ans;
    }
}

namespace abyss {

    /**
       Note: sym_table may be moved to lam later!

       @author: Ireina
     */
    struct FuncState {
        Lambda lam;
        SExpr tree;
        FuncState *outer = nullptr;
        I32 nk; //Number of constants
        I32 nf; //Number of functions defined inside
        I32 nv; //Number of local variables
        I32 np; //Number of parameters
        I32 nx; //Number of next fresh slot(register index)( > nv && > np)
        I32 top; //Current top of stack
        std::unordered_map<abyss::string, EnvIndex> sym_table; //Symbol table of local variable
    };


    template<class B>
    SExpr &traverseSExpr(SExpr &exp, FuncState &fs,
                         const std::function<B(SExpr &A, FuncState &fss)> &f);

    EnvIndex
    generateFuncCodeFromSExpr(const SExpr &exp, FuncState &fs);

    abyss::EnvIndex genArith(abyss::string op, const SExpr &exp, FuncState &fs);


    FuncState generateFuncState(const SExpr &tree, FuncState *outer_fs);

    EnvIndex
    generateFuncCodeFromSExpr(const SExpr &exp, FuncState &fs);
    vector<Atom>
    generateFuncCodeFromBody(const vector<std::shared_ptr<SExpr>> &body, FuncState &fs);

    FuncState
    generateFuncState(const SExpr &tree, FuncState *outer_fs = nullptr);

    SExpr &markK(SExpr &exp, FuncState &fs);
}//end namespace abyss

#endif
