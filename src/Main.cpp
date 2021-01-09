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
#include "Repl.hpp"



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
  (define (square x) (* x x))\
  (define (or a b) (if a True b))\
  (define (and a b) (if a b False))\
  (define (not b) (if b False True))\
  (define (fact x) (if (< 1 x) (* x (fact (- x 1))) 1))       \
\
  (fact 6)\
)";

            /*
        abyss::string src = "\
(define (base a b c d)\
  (define s \"hello\")\
  (* 2 3)\
)";
            */

        auto exp_and_idx = abyss::parseSExpr(src, 0, 0);
        auto exp = exp_and_idx.first;

        Show::println(exp);

        abyss::FuncState fs = abyss::generateFuncState(exp);
        abyss::State S = abyss::State(&Gs);

        auto cl = S.newGCObject<abyss::LClosure>(fs.lam);
        abyss::StkId sid = S.setClosure(cl);

        S.push(1)
         .push(2)
         .push(3)
         .push(4);

        abyss::CallInfo &ci = *S.call(sid, 1).value();
        S.printCurrentStackFrame();
        abyss::vm::execute(S, &ci);

        S.printCurrentStackFrame();
        //S.base_ci.printStackFrame();
        //abyss::printAbyssInfo();
        abyss::repl();
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
