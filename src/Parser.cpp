#include "Parser.hpp"


namespace abyss {

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
                exp = SExpr(isReal ? SReal : SInt);
                Show::println("???? " + Show::show(var));
                if(isReal) exp.var = std::stod(var);
                else exp.var = std::stoi(var);
                Show::println("???? " + Show::show(exp));
            }
            else if(true) {
                while(i < src.size() && !isblank(src[i]) && src[i] != ')') {
                    var += src[i];
                    ++i;
                }
                /* Unimplemented! SBool type */
                if(var == "True" || var == "False") {
                    exp = SExpr(SBool);
                    exp.var = var == "True" ? true : false;
                }
                else {
                    exp = SExpr(SVar);
                    exp.var = var;
                }
            }
            else {
                error("error> How to parse?");
            }
            return { exp, i - 1 };
        }
        /* src[i] is '(' */
        exp = SExpr(SList);
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
                exp.list.push_back(std::make_shared<SExpr>(list));
                i = j + 1;
            }
            else if(src[i] == ')') {
                break;
            }
            else {
                //Is a variable
                auto var_and_idx = parseSExpr(src, i, level);
                auto j = var_and_idx.second;
                exp.list.push_back(std::make_shared<SExpr>(var_and_idx.first));
                i = j + 1;
            }
        }
        return { exp, i };
    }
}
