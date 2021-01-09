#include "VM.hpp"


namespace abyss {
    namespace vm {

        void handle_arith(State &S, CallInfo &ci,
                          vector<Atom>::const_iterator &pc, const Atom &a) {

            StkId base = S.stack.begin() + ci.func;
            StkId ra = base + a.getArg_A();

            Value v1 = Cast::to<Value&>(*(base + a.getArg_B()));
            Value v2 = Cast::to<Value&>(*(base + a.getArg_C()));

            auto farith =
                [] (const Atom &a, const F64 &x, const F64 &y) {
                    switch(a.getOpCode()) {
                    case OP_ADD: return x + y;
                    case OP_SUB: return x - y;
                    case OP_MUL: return x * y;
                    case OP_DIV: return x / y;
                    default: {
                        error("error: VM.hpp: 48> VM: real number arith error");
                    }
                    }
                    return 0.0;
                };
            auto iarith =
                [] (const Atom &a, const I32 &x, const I32 &y) {
                    switch(a.getOpCode()) {
                    case OP_ADD: return x + y;
                    case OP_SUB: return x - y;
                    case OP_MUL: return x * y;
                    case OP_DIV: return x / y;
                    default: {
                        error("error: Vm.hpp: 61> VM: Integer number arith error");
                    }
                    }
                    return 0;
                };

            if(v1.getType() == object::TINTEGER &&
               v2.getType() == object::TINTEGER) {
                Integer res =
                    iarith(a,
                           Cast::to<const Integer&>(v1),
                           Cast::to<const Integer&>(v2));
                ra->val = res;
                S.top = a.getArg_A();
                //++pc;
            }
            else if(v1.getType() == object::TNUMBER &&
                    v2.getType() == object::TNUMBER) {
                Real res =
                    farith(a,
                           Cast::to<const Real&>(v1),
                           Cast::to<const Real&>(v2));
                ra->val = res;
                S.top = a.getArg_A();
                //++pc;
            }
            else {
                error("error> VM: Arithmetic error: No such type of arith!");
            }
        }

        void handle_equal(State &S, CallInfo &ci,
                          vector<Atom>::const_iterator &pc, const Atom &a) {
            StkId base = S.stack.begin() + ci.func;
            StkId ra = base + a.getArg_A();

            Value v1 = Cast::to<Value&>(*(base + a.getArg_B()));
            Value v2 = Cast::to<Value&>(*(base + a.getArg_C()));

            if(v1.getType() == object::TINTEGER &&
               v2.getType() == object::TINTEGER) {
                Bool res =
                    Cast::to<const Integer&>(v1) ==
                    Cast::to<const Integer&>(v2);
                ra->val = res;
                S.top = a.getArg_A();
                //++pc;
            }
            else if(v1.getType() == object::TNUMBER &&
                    v2.getType() == object::TNUMBER) {
                Bool res =
                    Cast::to<const Real&>(v1) ==
                    Cast::to<const Real&>(v2);
                ra->val = res;
                S.top = a.getArg_A();
                //++pc;
            }
            else {
                error("error> VM: Arithmetic error: No such type of arith!");
            }
        }

        void handle_less(State &S, CallInfo &ci,
                          vector<Atom>::const_iterator &pc, const Atom &a) {
            StkId base = S.stack.begin() + ci.func;
            StkId ra = base + a.getArg_A();

            Value v1 = Cast::to<Value&>(*(base + a.getArg_B()));
            Value v2 = Cast::to<Value&>(*(base + a.getArg_C()));

            if(v1.getType() == object::TINTEGER &&
               v2.getType() == object::TINTEGER) {
                Bool res =
                    Cast::to<const Integer&>(v1) <
                    Cast::to<const Integer&>(v2);
                ra->val = res;
                S.top = a.getArg_A();
                //++pc;
            }
            else if(v1.getType() == object::TNUMBER &&
                    v2.getType() == object::TNUMBER) {
                Bool res =
                    Cast::to<const Real&>(v1) <
                    Cast::to<const Real&>(v2);
                ra->val = res;
                S.top = a.getArg_A();
                //++pc;
            }
            else {
                error("error> VM: Arithmetic error: No such type of arith!");
            }
        }

        void handle_lessEq(State &S, CallInfo &ci,
                          vector<Atom>::const_iterator &pc, const Atom &a) {
            StkId base = S.stack.begin() + ci.func;
            StkId ra = base + a.getArg_A();

            Value v1 = Cast::to<Value&>(*(base + a.getArg_B()));
            Value v2 = Cast::to<Value&>(*(base + a.getArg_C()));

            if(v1.getType() == object::TINTEGER &&
               v2.getType() == object::TINTEGER) {
                Bool res =
                    Cast::to<const Integer&>(v1) <=
                    Cast::to<const Integer&>(v2);
                ra->val = res;
                S.top = a.getArg_A();
                //++pc;
            }
            else if(v1.getType() == object::TNUMBER &&
                    v2.getType() == object::TNUMBER) {
                Bool res =
                    Cast::to<const Real&>(v1) <=
                    Cast::to<const Real&>(v2);
                ra->val = res;
                S.top = a.getArg_A();
                //++pc;
            }
            else {
                error("error> VM: Arithmetic error: No such type of arith!");
            }
        }

        void execute(State &S, CallInfo *ci) {

            auto pc = ci->l.savedpc;
            Atom instr = *pc;
            StkId base = S.stack.begin() + ci->func;

            abyss::LClosure &cl =
                Cast::to<LClosure&>(Cast::to<Value&>(*base));

            vector<Value>::iterator k = cl.lam.k.begin(); //ci->l.lam->k.begin();
            auto R = [&base](I32 i) -> StackValue& { return *(base + i); };
            auto K = [&k](I32 i) -> Value& { return *(k + i); };
            auto UV = [&cl](I32 i) -> Value& { return *(cl.upvals[i].v); };

            int i = 0;
            for(; i < 20 && pc != cl.lam.code.end(); ++pc, ++i) {
                //S.printCurrentStackFrame();
                //Show::println(*pc);
                instr = *pc;
                I32 a = instr.getArg_A();
                StackValue &ra = R(a);
                switch(instr.getOpCode()) {
                case OP_MOVE: {
                    ra.val = Cast::to<const Value&>(R(instr.getArg_B()));
                    break;
                }
                case OP_LOADI: {
                    Integer i = instr.getArg_sBx();
                    ra.val = i;
                    break;
                }
                case OP_LOADF: {
                    Real n = instr.getArg_sBx();
                    ra.val = n;
                    break;
                }
                case OP_LOADK: {
                    /* Unimplemented! We have to check liveness before assigning */
                    ra.val = K(instr.getArg_Bx());
                    S.top = a;
                    //S.top = S.top < (ra - base) ? ra-base : S.top;
                    break;
                }
                case OP_LOADKX: {
                    /* Unimplemented! OP_LOADKX */
                    error("error> Unimplemented! OP_LOADKX");
                    break;
                }
                case OP_LOADFALSE: {
                    ra.val = Bool(false);
                    break;
                }
                case OP_LOADTRUE: {
                    ra.val = Bool(true);
                    break;
                }
                case OP_LFALSESKIP: {
                    ra.val = Bool(false);
                    pc++; //Skip next instruction
                    break;
                }
                case OP_LOADNIL: {
                    ra.val = Nil;
                }
                case OP_GETUPVAL: {
                    I32 b = instr.getArg_B();
                    //ra->val = *(cl.upvals[b]->v);
                    break;
                }
                case OP_SETUPVAL: {
                    //UpVal *uv = cl.upvals[a.getArg_B()];
                    //*(uv->v) = ra->val;
                    /* Unimplemented! GC is needed here */
                    error("error> VM: unimplemented! OP_SETUPVAL");
                    break;
                }
                case OP_GETTABUP: {
                    ra.val = UV(instr.getArg_C());
                    /* Unimplemented! */
                    //error("error> VM: unimplemented! OP_GETTABUP");
                    break;
                }
                case OP_GETTABLE: {
                    /* Unimplemented! */
                    error("error> VM: unimplemented! OP_GETTABLE");
                    break;
                }
                case OP_GETI: {
                    /* Unimplemented! */
                    error("error> VM: unimplemented! OP_GETI");
                    break;
                }
                case OP_GETFIELD: {
                    /* Unimplemented! */
                    error("error> VM: unimplemented! OP_GETFIELD");
                    break;
                }
                case OP_SETTABUP: {
                    /* Unimplemented! */
                    error("error> VM: unimplemented! OP_SETTABUP");
                    break;
                }
                case OP_SETTABLE: {
                    /* Unimplemented! */
                    error("error> VM: unimplemented! OP_SETTABLE");
                    break;
                }
                case OP_SETI: {
                    /* Unimplemented! */
                    error("error> VM: unimplemented! OP_SETI");
                    break;
                }
                case OP_SETFIELD: {
                    /* Unimplemented! */
                    error("error> VM: unimplemented! OP_SETFIELD");
                    break;
                }
                case OP_NEWTABLE: {
                    /* Unimplemented! */
                    error("error> VM: unimplemented! OP_NEWTABLE");
                    break;
                }
                case OP_SELF: {
                    const Value *slot;
                    Value &rb = (base + instr.getArg_B())->val;
                    Value &rc = instr.testArg_k()
                        ? *(k   + instr.getArg_C())
                        : (base + instr.getArg_C())->val;
                    /* Unimplemented! */
                    error("error: VM.hpp: 215> VM: unimplemented! OP_SELF");
                    break;
                }
                case OP_ADDI: {
                    Value &v1 = (base + instr.getArg_B())->val;
                    I32 imm = instr.getArg_sC();
                    if(v1.getType() == object::TINTEGER) {
                        Integer iv1 = 0;
                    }
                    else if(v1.getType() == object::TNUMBER) {
                        Real nb = 0;
                    }
                    break;
                }
                case OP_ADD: {
                    handle_arith(S, *ci, pc, instr);
                    break;
                }
                case OP_SUB: {
                    handle_arith(S, *ci, pc, instr);
                    break;
                }
                case OP_MUL: {
                    handle_arith(S, *ci, pc, instr);
                    break;
                }
                case OP_DIV: {
                    handle_arith(S, *ci, pc, instr);
                    break;
                }
                case OP_EQ: {
                    handle_equal(S, *ci, pc, instr);
                    break;
                }
                case OP_LT: {
                    handle_less(S, *ci, pc, instr);
                    break;
                }
                case OP_LE: {
                    handle_lessEq(S, *ci, pc, instr);
                    break;
                }
                    /* cases unimplemented ... */
                case OP_JMP: {
                    pc += instr.getArg_sBx() - 1;
                    /* Unimplemented! traps not handled */
                    //trap = ci.u.l.trap;
                    break;
                }
                case OP_TEST: {
                    ensure(ra.getType() == object::TBOOLEAN ||
                           ra.getType() == object::TINTEGER ||
                           ra.getType() == object::TNIL,
                           "error: VM.hpp: 255> VM: OP_TEST cond not boolean or nil type!");
                    U32 cond;
                    if(ra.getType() == object::TINTEGER) {
                        cond = Cast::to<const Integer&>(ra.val) == 0 ? false : true;
                    }
                    else {
                        cond = Cast::to<Bool>(ra.val);
                    }
                    if(cond == static_cast<Bool>(instr.getArg_C())) pc++;
                    else {
                        /* Unimplemented! update traps */
                    }
                    break;
                }
                case OP_CALL: {
                    optional<CallInfo *> newci;
                    I32 b = instr.getArg_B();
                    I32 n_results = instr.getArg_C() - 1;
                    if(b != 0) { /* fixed number of arguments? */
                        S.top = ci->func + a + b;
                    }
                    ci->l.savedpc = pc;
                    if((newci = S.call(base + a, n_results))) {//Yes, we use assignment to test! not ==
                        newci.value()->func = ci->func + a;
                        execute(S, newci.value()); //well, we shall remove recursion by evil gotos later...
                    }
                    else {
                        //Unimplemented!
                    }
                    break;
                }
                case OP_RETURN: {
                    Integer n = instr.getArg_B() - 1;
                    Integer nparams1 = instr.getArg_C();
                    break;
                }
                case OP_RETURN0: {
                    S.ci--;
                    //S.top = base - 1;
                    break;
                }
                case OP_RETURN1: {
                    //break;
                    I32 nres = ci->nresults;
                    if(S.ci != S.cis.begin()) {
                        S.ci --;
                        S.cis.pop_back();
                    }

                    if(nres == 0) S.top = ci->func;
                    else {
                        *base = ra;
                        S.top = ci->func;
                        while(--nres > 0) {
                            S.stack[S.top] = abyss::None;
                            ++S.top;
                        }
                    }

                    if(base->getType() == object::TFUNCTION) {
                        //actually should be TFUNCTION_LCLOSURE
                        Value &v = base->val;
                        auto &ret_cl = Cast::to<LClosure&>(v);
                        for(auto &uv : ret_cl.upvals) {
                            if(uv.isOpen()) {
                                uv.value = *uv.v;
                                uv.v = &uv.value;
                            }
                        }
                    }
                    //abyss::collect(S);
                    ci->l.savedpc = pc;
                    return; //ad-hoc, should be removed later!
                    break;
                }
                case OP_CLOSURE: {
                    //Show::println(cl.lam->p.size());
                    shared_ptr<Lambda> lam = cl.lam.p[instr.getArg_Bx()];
                    LClosure &lcl = S.newGCObject<LClosure>(*lam);
                    //lcl.lam = *lam;
                    for(const auto &upvaldesc : lam->upvalues) {
                        //Show::println(upvaldesc);
                        /*
                        std::find_if
                            (S.openupval.begin(),
                             S.openupval.end(),
                             [](const UpVal &val) {

                             });
                        */
                        UpVal val;
                        val.v = &(base + upvaldesc.idx)->val;
                        S.openupval.push_back(val);
                        lcl.upvals.push_back(S.openupval.back());
                    }
                    ra = Value(&lcl);
                    S.top = a;
                    //S.top = S.top < (ra - base) ? ra-base : S.top;
                    /* Unimplemented! */
                    break;
                }
                default: {
                    error("error: VM.hpp: 2xx> VM: Unknown instruction");
                    break;
                }
                }//end switch OpCode
                ci->l.savedpc = pc;
            }//end for

        }//end execute

    }//end namespace vm
}//end namespace abyss
