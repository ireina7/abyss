#include "Code.hpp"


namespace abyss {

    template<class B>
    SExpr &traverseSExpr(SExpr &exp, FuncState &fs,
                         const std::function<B(SExpr &A, FuncState &fss)> &f) {
        switch(exp.getType()) {
        case SNone:
        case SInt:
        case SBool:
        case SReal:
        case SVar: {
            f(exp, fs);
            break;
        }
        case SList: {
            if(exp.list[0]->getType() == SVar &&
               Cast::to<const abyss::string&>(exp.list[0]->var) == "define") {
                break;
            }
            for(const auto &x : exp.list) {
                traverseSExpr(*x, fs, f);
            }
            break;
        }
        }
        return exp;
    }

    EnvIndex
    generateFuncCodeFromSExpr(const SExpr &exp, FuncState &fs);

    std::unordered_map<abyss::string, std::function<abyss::Atom(Reg, Reg, Reg)>> ff
        {
         {"+", [](Reg a, Reg b, Reg c) { return instructions::ADD(a, b, c); }},
         {"-", [](Reg a, Reg b, Reg c) { return instructions::SUB(a, b, c); }},
         {"*", [](Reg a, Reg b, Reg c) { return instructions::MUL(a, b, c); }},
         {"/", [](Reg a, Reg b, Reg c) { return instructions::DIV(a, b, c); }},
        };


    EnvIndex
    genABCFunc(const SExpr &exp, FuncState &fs,
               function<void(FuncState&, Reg, Reg, Reg)> gen) {
        if(exp.list.size() != 3) {
            error("error> Arithmetic error");
        }

        Reg rb = generateFuncCodeFromSExpr(*exp.list[1], fs).val;
        if(rb >= fs.top) fs.top++;
        Reg rc = generateFuncCodeFromSExpr(*exp.list[2], fs).val;
        if(rb >= fs.top) fs.top++;
        Reg ra;
        /**
           This is used to save registers... i.e. pick the slot with least number
           If any returned rx > fs.nv, then it's a temporary register
           which can be reused.
           @author: Ireina
        */
        if(rb > fs.nv) {
            ra = rb;
        }
        else if(rc > fs.nv) {
            ra = rc;
        }
        else {
            ra = fs.top++;
        }
        fs.top = ra;
        gen(fs, ra, rb, rc);
        return EnvIndex(RegIndex, ra);
    }

    EnvIndex
    genArith(string op, const SExpr &exp, FuncState &fs) {

        return genABCFunc
            (exp, fs, [&op](FuncState &fs, Reg a, Reg b, Reg c) {
                          fs.lam.code.push_back(ff[op](a, b, c));
                      });
    }

    EnvIndex
    genEqual(const SExpr &exp, FuncState &fs) {

        return genABCFunc
            (exp, fs, [](FuncState &fs, Reg a, Reg b, Reg c) {
                          fs.lam.code.push_back(instructions::EQ(a, b, c));
                      });
    }

    EnvIndex
    genLT(const SExpr &exp, FuncState &fs) {

        return genABCFunc
            (exp, fs, [](FuncState &fs, Reg a, Reg b, Reg c) {
                          fs.lam.code.push_back(instructions::LT(a, b, c));
                      });
    }

    EnvIndex
    genLE(const SExpr &exp, FuncState &fs) {

        return genABCFunc
            (exp, fs, [](FuncState &fs, Reg a, Reg b, Reg c) {
                          fs.lam.code.push_back(instructions::LE(a, b, c));
                      });
    }


    FuncState generateFuncState(const SExpr &tree, FuncState *outer_fs);

    EnvIndex
    generateFuncCodeFromSExpr(const SExpr &exp, FuncState &fs) {
        std::vector<Atom> &codes = fs.lam.code;
        /*
        Show::println("---------- Codes ----------");
        for(const auto &code : fs.lam.code) {
            Show::println(code);
        }
        Show::println("---------- end Codes ----------");
        */
        switch(exp.getType()) {
        case SNone: {
            return errorIdx;
        }
        case SInt: {
            if(!exp.isK) {
                error("error> ...");
            }
            codes.push_back
                (instructions::LOADK(fs.top, Cast::to<Integer>(exp.var)));
            return EnvIndex(RegIndex, fs.top);
            break;
        }
        case SBool: {
            break;
        }
        case SReal: {
            if(!exp.isK) {
                error("error> ...");
            }
            codes.push_back
                (instructions::LOADK(fs.top, Cast::to<Real>(exp.var)));
            return EnvIndex(RegIndex, fs.top);
            break;
        }
        case SVar: {
            auto key = Cast::to<const string&>(exp.var);
            if(fs.sym_table.count(key) > 0) {
                /**
                   C++ sucks again!
                   We have to check count here!
                   If we do not carefully check whether key has existed,
                   the subscription operator will automatically create one for us
                   which is really a bug!

                   @author: Ireina
                 */
                return fs.sym_table[key];
            }
            else {
                /**
                   This may come from outer scope, we have to make an upvalue.

                   @author: Ireina
                 */
                Upvaldesc uvdesc;
                bool found = false;
                for(FuncState *cur = fs.outer; cur != nullptr; cur = cur->outer) {
                    if(cur->sym_table.count(key) > 0) {
                        uvdesc.name = key;
                        uvdesc.instack = 1;
                        uvdesc.idx = cur->sym_table[key].val;
                        found = true;
                    }
                }
                if(!found) {
                    error("error> Coder: Upvalue " +
                          Show::show(key) + " not found");
                }
                fs.lam.upvalues.push_back(uvdesc);
                codes.push_back
                    (instructions::GETTABUP
                     (fs.top, 0, fs.lam.upvalues.size() - 1));
                return EnvIndex(RegIndex, fs.top);
                //error("error> Code.hpp: Lexical scoping is not yet implemented!");
            }
            break;
        }
        case SList: {
            if(exp.list.empty()) {
                return errorIdx;
            }
            /**
               If is an atom call
             */
            if(exp.list[0]->getType() == SVar) {
                auto op = Cast::to<const string&>(exp.list[0]->var);

                if(op == "+") {
                    return genArith("+", exp, fs);
                }
                if(op == "-") {
                    return genArith("-", exp, fs);
                }
                if(op == "*") {
                    return genArith("*", exp, fs);
                }
                if(op == "/") {
                    return genArith("/", exp, fs);
                }
                if(op == "=") {
                    return genEqual(exp, fs);
                }
                if(op == "<") {
                    return genLT(exp, fs);
                }
                if(op == "<=") {
                    return genLE(exp, fs);
                }

                if(op == "define" && exp.list[1]->getType() == SList) {
                    /**
                       (define (f args...) <body>)
                     */
                    FuncState newFs = generateFuncState(exp, &fs);
                    fs.lam.p.push_back(std::make_shared<abyss::Lambda>(newFs.lam));
                    auto idx = EnvIndex(LamIndex, fs.lam.p.size() - 1);
                    auto list = exp.list;
                    /* Ok now this may be valid function declaration */
                    auto name_and_params = list[1];
                    std::shared_ptr<SExpr> name;
                    if(name_and_params->getType() == SList &&
                       name_and_params->list[0]->getType() == SVar) {
                        name = name_and_params->list[0];
                    }
                    else if(name_and_params->getType() == SVar) {
                        name = name_and_params;
                    }
                    else {
                        error("error> GenCodeFromSExpr: Wrong format of definition!");
                    }

                    codes.push_back(instructions::CLOSURE(fs.top++, idx.val));

                    fs.sym_table.insert
                    ({ Cast::to<const string&>(name->var),
                       EnvIndex(RegIndex, fs.top - 1) });
                    fs.nv++;
                    return EnvIndex(RegIndex, fs.top - 1);
                }
                if(op == "define" && exp.list[1]->getType() == SVar) {
                    /**
                       (define (f args...) <body>)
                     */
                    FuncState newFs = generateFuncState(exp, &fs);
                    fs.lam.p.push_back(std::make_shared<abyss::Lambda>(newFs.lam));
                    auto idx = EnvIndex(LamIndex, fs.lam.p.size() - 1);
                    auto list = exp.list;
                    /* Ok now this may be valid function declaration */
                    auto name_and_params = list[1];
                    std::shared_ptr<SExpr> name;
                    name = name_and_params;

                    codes.push_back(instructions::CLOSURE(fs.top, idx.val));
                    codes.push_back(instructions::CALL(fs.top++, 0, 0)); //call it in place!

                    fs.sym_table.insert
                    ({ Cast::to<const string&>(name->var),
                       EnvIndex(RegIndex, fs.top - 1) });
                    fs.nv++;
                    return EnvIndex(RegIndex, fs.top - 1);
                }
                if(op == "let") {
                    /**
                       (let (<binding>...) expr)
                     */
                    /* Unimplemented! */
                    return errorIdx;
                }
                if(op == "set!") {
                    /**
                       (set! id exp)
                     */
                    /* Unimplemented! */
                    return errorIdx;
                }
                if(op == "if") {
                    /**
                       (if <condition> <true-expression> <false-expression>)
                     */
                    ensure(exp.list.size() == 4,
                           "error> Code: wrong format of if expression");
                    auto cond = exp.list[1];
                    auto val0 = exp.list[2];
                    auto val1 = exp.list[3];
                    Reg ra = fs.top;
                    EnvIndex idx_cond = generateFuncCodeFromSExpr(*cond, fs);
                    if(idx_cond.val < ra) {
                        codes.push_back(instructions::MOVE(fs.top++, idx_cond.val));
                    }
                    codes.push_back(instructions::TEST(ra, 0));
                    codes.push_back(instructions::JUMP(0)); //need to reset value of sJ!
                    I32 idx_jmp0 = codes.size() - 1;

                    //The true part expression
                    EnvIndex idx_val1 = generateFuncCodeFromSExpr(*val1, fs);
                    if(idx_val1.val < ra) {
                        codes.push_back(instructions::MOVE(fs.top, idx_val1.val));
                    }
                    //fs.top--;
                    codes.push_back(instructions::JUMP(0));
                    I32 idx_jmp1 = codes.size() - 1;

                    //The false part expression
                    codes[idx_jmp0] = instructions::JUMP(codes.size() - idx_jmp0);
                    EnvIndex idx_val0 = generateFuncCodeFromSExpr(*val0, fs);
                    if(idx_val0.val < ra) {
                        codes.push_back(instructions::MOVE(fs.top, idx_val0.val));
                    }
                    codes[idx_jmp1] = instructions::JUMP(codes.size() - idx_jmp1);
                    codes.push_back(instructions::MOVE(ra, fs.top));
                    fs.top = ra;
                    return EnvIndex(RegIndex, ra);
                }
            }

            /**
               else this is a normal function call
             */
            auto f = exp.list[0];
            Reg ra = fs.top;
            for(auto it = exp.list.begin(); it != exp.list.end(); ++it) {
                Reg rp = generateFuncCodeFromSExpr(**it, fs).val;
                if(rp < ra) {//Can be equal?
                    codes.push_back(instructions::MOVE(fs.top, rp));
                }
                fs.top++;
            }
            Reg rb = fs.top - ra - 1;
            codes.push_back(instructions::CALL(ra, rb, 0));
            fs.top = ra;
            return EnvIndex(RegIndex, ra);
            break;
        }
        }
        return errorIdx;
    }
    vector<Atom>
    generateFuncCodeFromBody(const vector<std::shared_ptr<SExpr>> &body,
                      FuncState &fs) {
        std::vector<Atom> codes;
        /* Unimplemented! for assignment statements */
        for(auto it = body.begin(); it < body.end(); ++it) {
            generateFuncCodeFromSExpr(**it, fs);
        }
        return codes;
    }

    SExpr &markK(SExpr &exp, FuncState &fs) {
        traverseSExpr<void>
            (exp, fs,
             [](SExpr &exp, FuncState &fs) {
                 switch(exp.getType()) {
                 case SNone: {
                     return;
                 }
                 case SInt: {
                     fs.lam.k.push_back(std::get<Integer>(exp.var));
                     fs.nk += 1;
                     exp.isK = true;
                     exp.var = fs.nk;
                     break;
                 }
                 case SBool: {
                     fs.lam.k.push_back(std::get<Bool>(exp.var));
                     fs.nk += 1;
                     exp = SExpr(SInt); //we have to convert to int to indicate constant index
                     exp.isK = true;
                     exp.var = fs.nk;
                     break;
                 }
                 case SReal: {
                     fs.lam.k.push_back(std::get<Real>(exp.var));
                     fs.nk += 1;
                     exp = SExpr(SInt); //we have to convert to int to indicate constant index
                     exp.isK = true;
                     exp.var = fs.nk;
                     break;
                 }
                 case SVar: {
                     break;
                 }
                 default: {
                     error("error> Unknow SExpr type: " +
                           Show::show(exp.getType()));
                 }
                 }
             });
        return exp;
    }

    FuncState
    generateFuncState(const SExpr &tree,
                      FuncState *outer_fs) {
        /**
           Format:
           (define (f a b ...) <body>)
         */
        FuncState fs;
        fs.lam = Lambda();
        fs.outer = outer_fs;
        Lambda &lam = fs.lam;
        lam.k.push_back(abyss::None);//Index 0 is None constant
        ensure(tree.getType() == SList,
               "error> generateFunccode should receive function declaration!");
        /* else is a list */
        auto list = tree.list;
        ensure(list[0]->getType() == SVar &&
               //list[0]->var.getType() == object::TSTRING &&
               Cast::to<const abyss::string&>(list[0]->var) == "define",
               "error> generateFunccode should receive function declaration!");

        /* Ok now this may be valid function declaration */
        auto name_and_params = list[1];
        std::shared_ptr<SExpr> name;
        std::vector<std::shared_ptr<SExpr>>::iterator params;
        bool hasParams = true;
        if(name_and_params->getType() == SList &&
           name_and_params->list[0]->getType() == SVar) {

            name = name_and_params->list[0];
            params = name_and_params->list.begin() + 1;
        }
        else if(name_and_params->getType() == SVar) {
            name = name_and_params;
            hasParams = false;
        }
        else {
            error("error> generateFunccode should receive function declaration!");
        }
        /*
        lam.source = std::make_shared<abyss::String>
            (Cast::to<const abyss::string&>(name->var));
        Show::println(*lam.source);
        */

        lam.name = Cast::to<const abyss::string&>(name->var);
        fs.np = hasParams ? name_and_params->list.size() - 1 : 0;
        lam.n_params = fs.np;
        fs.nv = hasParams ? name_and_params->list.size() - 1 : 0;
        fs.nk = 0;
        fs.nx = fs.np + 1;
        fs.top = fs.nx;

        /* Handling parameter symbol table */
        if(hasParams) {
            I32 i = 0;
            for(auto it = name_and_params->list.begin();
                it != name_and_params->list.end(); ++it) {
                if((*it)->getType() == SVar) {
                    fs.sym_table.insert
                        ({Cast::to<const abyss::string&>((*it)->var), EnvIndex(RegIndex, i)});
                    ++i;
                }
            }
        }

        /* Handling body part */
        auto body_iter = list.begin() + 2;
        for(auto it(body_iter); it != list.end(); ++it) {
            //Traversing body part
            //Show::println(">>>");
            //Show::println(**it);
            /* Unimplemented! handle set! assignment */
            /* Expressions */
            markK(**it, fs);

            Reg rx = generateFuncCodeFromSExpr(**it, fs).val;
            if(it + 1 == list.end()) {
                fs.lam.code.push_back(instructions::RETURN(rx));
            }
            //fs.lam.code.push_back(instructions::RETURN(rx));
        }

        unordered_map<string, I32> local_table;
        for(const auto &t : fs.sym_table) {
            local_table.insert({t.first, t.second.val});
        }
        lam.sym_table = local_table;
        //lam.size_k = fs.nk;
        /*
        Show::println("Found " + Show::show(fs.nk) + " constants");
        Show::println("Found " + Show::show(fs.np) + " parameters");
        Show::println("---------- Symbol table ----------");
        for(const auto &x : fs.sym_table) {
            Show::print(x.first + ": ");
            Show::println(x.second);
        }
        Show::println("---------- end Symbol table ----------");
        Show::println("---------- Constant table ----------");
        int j = 0;
        for(const auto &x : fs.lam.k) {
            Show::print(Show::show(j++) + ": ");
            Show::println(x);
        }
        Show::println("---------- end Constant table ----------");
        // Unimplemented!
        //generateFuncCodeFromBody()
        Show::println(tree);
        Show::println("");
        Show::println("---------- Codes ----------");
        for(const auto &code : fs.lam.code) {
            Show::println(code);
        }
        Show::println("---------- end Codes ----------");
        Show::println("---------- Up values ----------");
        for(const auto &var : fs.lam.upvalues) {
            Show::println(var);
        }
        Show::println("---------- end Up values ----------");

Show::println("");
Show::println("");
Show::println("");
Show::println("");
        */
        return fs;
    }

}//end namespace abyss
