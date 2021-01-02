#include <iostream>
#include <fstream>

#include "Abyss.hpp"
#include "Object.hpp"
#include "Instruction.hpp"
#include "VM.hpp"
#include "State.hpp"
#include "Stack.hpp"
#include "Parser.hpp"
#include "Code.hpp"
#include "Debug.hpp"



int testVM() {
    try {
        println("abyss> This is the main function of abyss-cpp project.");
        abyss::tests();

        abyss::GlobalState Gs;
        //abyss::Lambda main_lam;
        abyss::Lambda lam;
        std::vector<abyss::Atom> codes {
            abyss::Atom::create_iAsBx(abyss::OP_LOADI, 1, 7),
            abyss::Atom::create_iAsBx(abyss::OP_LOADI, 2, 3),
            //abyss::Atom::create_iABC(abyss::OP_MOVE, 1, 1, 0, false),
            //abyss::Atom::create_iABC(abyss::OP_MOVE, 2, 2, 0, false),
            abyss::Atom::create_iABC(abyss::OP_MUL, 3, 1, 2, false),
            abyss::Atom::create_iABC(abyss::OP_RETURN1, 3, 0, 0, 0),
        };
        /*
        abyss::string src = "\
(define (base a b c d)\
  (define (f x) (+ x 1))\
  (define (g x) (* x x))\
  (define (multi x y z q w e) (* q (* w (* e (* x z)))))\
  (define (five x) (* x (* x (* x (* x x)))))\
  (+ (f c) (multi a b c d (five c) d)))";
        */

        /*
            abyss::string src = "\
(define (base a b c d)\
  (define (f x) (+ x 1))\
  (define (g x) (* 2 (f x)))\
  (define (h x y) (+ (f x) (g y)))\
  (g (h c d))\
)";
*/
        /*
            abyss::string src = "\
(define (base a b c d)\
  (define (cons x y)\
    (define (f m) (m x y))\
    f)\
  (define (car xy)\
    (define (select p q) p)\
    (xy select))\
  (define (cdr xy)\
    (define (select p q) q)\
    (xy select))\
  (define (square x) (* x x))\
\
  (car (cons (square 3) 4))\
)";
*/

             abyss::string src = "\
(define (base a b c d)\
  (define (square x) (* x x)n)\
  (define (or a b) (if a True b))\
  (define (and a b) (if a b False))\
  (define (not b) (if b False True))\
  (define (fact x) (if (< 1 x) (* x (fact (- x 1))) 1))       \
\
  (fact 5)\
)";

        auto exp_and_idx = abyss::parseSExpr(src, 0, 0);
        auto exp = exp_and_idx.first;
        Show::println(exp);

        abyss::FuncState fs = abyss::generateFuncState(exp);

        lam = fs.lam;
        Show::println(lam.p.size());
        abyss::CallInfo ci, main_ci;
        ci.l.lam = &lam;
        ci.l.savedpc = lam.code.cbegin();
        ci.l.endpc = lam.code.cend();

        abyss::State S = abyss::State(&Gs, main_ci);
        S.base_ci.l.base = S.stack.begin();

        S.base_ci.func = 0;//S.stack.begin();
        S.base_ci.top = 2;//S.stack.begin() + 10;


        S.setMainClosure();

        auto cl = S.newFixedObject<abyss::LClosure>(lam);
        abyss::Value clv = &cl;
        S.stack[2] = clv;
        ci.func = 2;//S.stack.begin() + 2;
        S.stack[3].val = 1;
        S.stack[4].val = 2;
        S.stack[5].val = 3;
        S.stack[6].val = 4;
        ci.top = ci.func + 20;
        ci.l.base = S.stack.begin() + ci.func + 1;
        ci.nresults = 1;
        S.appendCallInfo(ci);
        S.printCurrentStackFrame();
        S.base_ci.printStackFrame();


        abyss::vm::execute(S, &ci);
        //S.printCurrentStackFrame();
        //S.base_ci.func = 0;//S.stack.begin();
        //S.base_ci.l.base = S.stack.begin();
        S.base_ci.printStackFrame();
        abyss::printAbyssInfo();
    }
    catch(const std::exception& e) {
        print_exception(e, 1);
    }
    return 0;
}

int main(int argc, char **argv) {

    testVM();
    try {
        println("abyss> This is the main function of Abyss project.");
        //abyss::tests();
        auto instr = abyss::instructions::MOVE(1, -2);
        Show::println(instr);
        Show::println(static_cast<int>(static_cast<unsigned int>(-1)));
    }
    catch(const std::exception& e) {
        print_exception(e, 1);
    }

    return 0;
}
