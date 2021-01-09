#ifndef REPL_HPP
#define REPL_HPP

#include "State.hpp"
#include "Parser.hpp"
#include "Code.hpp"
#include "VM.hpp"


namespace abyss {

    void repl() {

        FuncState fs;
        CallInfo ci;
        GlobalState Gs;
        State S = State(&Gs);

        fs = generateFuncState
            (parseSExpr("(define (repl) )", 0, 0).first);

        I32 saved_pc = 0;
        string line = "";
        printAbyssInfo();
        for(;;) {
            std::cout << "abyss> " << std::flush;
            std::getline(std::cin, line);
            if(line == ".quit") {
                break;
            }
            if(line == ".help") {
                Show::println("->");
                Show::println("Help has not been completed yet...\n");
                continue;
            }
            string src = line;
            auto exp_and_idx = parseSExpr(src, 0, 0);
            auto exp = exp_and_idx.first;
            if(exp.getType() == SNone) {
                continue;
            }
            //Show::println(exp);
            if(!exp.isDefinition()) markK(exp, fs);
            auto idx = generateFuncCodeFromSExpr(exp, fs);

            if(exp.getType() == abyss::SVar) {
                //if()
                Show::println("-> " + Show::show(S.stack[idx.val]) + "\n");
                continue;
            }
            /*
            if(idx.val <= fs.nv) {
                //Is a local variable
                Show::println("-> " + Show::show(S.stack[idx.val]) + "\n");
                continue;
                }*/

            auto cl = S.newFixedObject<LClosure>(fs.lam);
            /*
            Show::println("---------- Codes ----------");
            for(const auto &code : cl.lam.code) {
                Show::println(code);
            }
            Show::println("---------- end Codes ----------");
            */
            S.stack[0] = Value(&cl);
            //S.printCurrentStackFrame();
            CallInfo &ci = *S.call(S.stack.begin(), 1).value();
            ci.l.savedpc += saved_pc;
            /*
            if(ci.l.savedpc == ci.l.lam->code.end()) {
                ci.l
                }*/
            vm::execute(S, &ci);
            //S.printCurrentStackFrame();
            saved_pc = ci.l.savedpc - ci.l.lam->code.begin() + 1;

            Show::println("-> " + Show::show(S.stack[S.top]) + "\n");
        }
        Show::println("Bye bye.");
    }
}

#endif
