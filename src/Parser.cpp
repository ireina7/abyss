#include "Parser.hpp"


namespace abyss {

    bool SExpr::isDefinition() const {
        bool ans = false;
        match(this->val,
              [&](const SExprList &xs) {
                  if(xs.size() > 0 &&
                     xs[0]->getType() == SVar &&
                     Cast::to<VarDesc&>(*xs[0]).name == "define") {
                      ans = true;
                  }
              },
              [&](auto) {});
        return ans;
    }

    std::pair<SExpr, abyss::string::size_type>
    parseSExpr(abyss::string src, abyss::string::size_type start, I32 level) {
        SExpr exp;
        if(src.empty()) return { SExprNone, 0 };

        abyss::string::size_type i = start;
        while(i < src.size() && src[i] == ' ') ++i;
        if(src[i] != '(') {
            //Is a variable
            abyss::string var = "";
            if(isdigit(src[i]) || src[i] == '~') {

                bool isReal = false;
                while(i < src.size() && !isblank(src[i]) && src[i] != ')') {
                    if(src[i] == '.') {
                        isReal = true;
                    }
                    var += src[i];
                    ++i;
                }
                //exp = SExpr(isReal ? SReal : SInt);
                if(isReal) exp = std::stod(var);
                else exp = std::stoi(var);
            }
            else if(src[i] == '"') {
                for(i += 1; i < src.size() && src[i] != '"'; ++i) {
                    var += src[i];
                }
                i += 1;
                exp = var;
            }
            else {
                while(i < src.size() && !isblank(src[i]) && src[i] != ')') {
                    var += src[i];
                    ++i;
                }
                /* Unimplemented! SBool type */
                if(var == "True" || var == "False") {
                    //exp = SExpr(SBool);
                    exp = (var == "True") ? true : false;
                }
                else {
                    //exp = SExpr(SVar);
                    exp = VarDesc(var);
                }
            }
            return { exp, i - 1 };
        }

        /* src[i] is '(' */
        exp = SExpr(SList);
        exp.val = vector<shared_ptr<SExpr>>{};
        for(i = start + 1; i < src.size();) {

            while(i < src.size() && src[i] == ' ') ++i;
            if(src[i] == '(') {
                //Is again a List
                while(i < src.size() && isblank(src[i])) ++i;

                auto list_and_idx = parseSExpr(src, i, level + 1);
                auto list = list_and_idx.first;
                auto j = list_and_idx.second;
                if(j >= src.size()) {
                    return { SExprNone, start };
                }

                Cast::to<SExprList&>(exp).push_back(std::make_shared<SExpr>(list));
                i = j + 1;
            }
            else if(src[i] == ')') {
                break;
            }
            else {
                //Is a variable
                auto var_and_idx = parseSExpr(src, i, level);
                auto j = var_and_idx.second;
                Cast::to<SExprList&>(exp).push_back(std::make_shared<SExpr>(var_and_idx.first));
                i = j + 1;
            }
        }
        return { exp, i };
    }
}
