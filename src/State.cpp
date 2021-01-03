#include "State.hpp"


namespace abyss {
    /**
       This is evil trick!
       If func is C closure, we call it directly and return nullptr
       to indicate this is C closure;

       If func is abyss closure, we set basic callinfo and return it;
       @old time

       Update: Now we can safely use optional<_>!
       @author: Ireina
    */
    optional<CallInfo*> State::call(StkId func, I32 n_results) {
        CFunction f;
        const object::Tag tag = func->getTag();
        if(tag == TFUNCTION_CCLOSURE) {
            f = Cast::to<const CClosure&>(func->val).f;
            I32 n = (*f)(this); // Do the actual call!
            return nothing;
        }
        else if(tag == TFUNCTION_LCLOSURE) {
            Lambda &lam =
                Cast::to<LClosure&>(func->val).lam;
            CallInfo ci;
            ci.nresults = n_results;
            ci.l.lam = &lam;
            ci.l.savedpc = lam.code.cbegin();
            ci.l.endpc = lam.code.end();
            ci.top = (func - this->stack.begin()) + 1 + 10;//lam->max_stacksize;
            ci.func = (func - this->stack.begin());
            this->appendCallInfo(ci);
            return &*(this->ci);
        }
        else {
            error("error> precall unknown function type!");
        }
        return nullptr;
    }

}
