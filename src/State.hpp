#ifndef STATE_HPP
#define STATE_HPP

#include "Abyss.hpp"
#include "Object.hpp"
#include "Instruction.hpp"
#include "Stack.hpp"
#include "GC.hpp"
#include <list>

namespace abyss {


    struct State;
    /*
    ** Information about a call.
    */
    struct CallInfo {
        vector<StackValue>::size_type func;  /* function index in the stack */
        vector<StackValue>::size_type top;   /* top for this function */
        I32 n_top;
        struct CallInfo *previous, *next;  /* dynamic call link */
        struct {  /* only for Abyss functions */
            StkId base;
            Lambda *lam; //may be removed later...
            std::vector<Atom>::const_iterator savedpc;
            std::vector<Atom>::const_iterator endpc;
            //volatile l_signalT trap;
            int nextraargs;  /* # of extra arguments in vararg functions */
        } l;
        struct {  /* only for C functions */
            KFunction k;  /* continuation in case of yields */
            ptrdiff_t old_errfunc;
            KContext ctx;  /* context info. in case of yields */
        } c;
        I16 nresults;  /* expected number of results from this function */
        U16 callstatus;
        I8 ready = false;
        CallInfo() = default;

        /* Debug only */
        void printStackFrame() {
            Show::println("========== Stack Frame ==========");
            Show::println(">> SIZE: " + Show::show(top - func));
            Show::println(func);
            for(auto it = func; it <= top; ++it) {
                Show::print((l.base + it)->val);
                Show::println("\t\t|");
            }
            Show::println("========== end Frame ==========");
        }
    };

    /*
    ** 'Global state', shared by all threads of this state
    */
    struct GlobalState {
        //abyss_Alloc frealloc;  /* function to reallocate memory */
        void *ud;         /* auxiliary data to 'frealloc' */
        MemSize totalbytes;  /* number of bytes currently allocated - GCdebt */
        MemSize lastbytes;
        MemSize GCdebt;  /* bytes allocated not yet compensated by the collector */
        MemSize GCestimate;  /* an estimate of the non-garbage memory in use */
        MemSize lastatomic;  /* see function 'genstep' in file 'lgc.c' */
        //stringtable strt;  /* hash table for strings */
        Value l_registry;
        Value nilvalue;  /* a nil value */
        unsigned int seed;  /* randomized seed for hashes */
        U8 currentwhite;
        U8 gcstate;  /* state of garbage collector */
        U8 gckind;  /* kind of GC running */
        U8 genminormul;  /* control for minor generational collections */
        U8 genmajormul;  /* control for major generational collections */
        U8 gcrunning;  /* true if GC is running */
        U8 gcemergency;  /* true if this is an emergency collection */
        U8 gcpause;  /* size of pause between successive GCs */
        U8 gcstepmul;  /* GC "speed" */
        U8 gcstepsize;  /* (log2 of) GC granularity */
        list<GCObject*> allgc;  /* list of all collectable objects */
        GCObject **sweepgc;  /* current position of sweep in list */
        GCObject *finobj;  /* list of collectable objects with finalizers */
        GCObject *gray;  /* list of gray objects */
        GCObject *grayagain;  /* list of objects to be traversed atomically */
        GCObject *weak;  /* list of tables with weak values */
        GCObject *ephemeron;  /* list of ephemeron tables (weak keys) */
        GCObject *allweak;  /* list of all-weak tables */
        GCObject *tobefnz;  /* list of userdata to be GC */
        list<GCObject*> fixedgc;  /* list of objects not to be collected */
        /* fields for generational collector */
        GCObject *survival;  /* start of objects that survived one GC cycle */
        GCObject *old1;  /* start of old1 objects */
        GCObject *reallyold;  /* objects more than one cycle old ("really old") */
        GCObject *firstold1;  /* first OLD1 object in the list (if any) */
        GCObject *finobjsur;  /* list of survival objects with finalizers */
        GCObject *finobjold1;  /* list of old1 objects with finalizers */
        GCObject *finobjrold;  /* list of really old objects with finalizers */
        State *twups;  /* list of threads with open upvalues */
        CFunction panic;  /* to be called in unprotected errors */
        State *mainthread;
        String memerrmsg;  /* message for memory-allocation errors */
        //String tmname[TM_N];  /* array with tag-method names */
        //struct Table *mt[ABYSS_NUMTAGS];  /* metatables for basic types */
        //TString *strcache[STRCACHE_N][STRCACHE_M];  /* cache for strings in API */
        //abyss_WarnFunction warnf;  /* warning function */
        //void *ud_warn;         /* auxiliary data to 'warnf' */

        GlobalState()
            : totalbytes(0),
              lastbytes(200),
              nilvalue(abyss::Nil),
              gcrunning(0)
        {}

        GlobalState(void *ud)
            : GlobalState()
        {
            this->ud = ud;
        }

        ~GlobalState() {
            for(auto &p : this->allgc) {
                //Show::println("deleting " + Show::show(*p));
                //Show::println("GS: deleting... " + Show::show(p->getType()));
                delete p;
            }
            for(auto &p : this->fixedgc) {
                //Show::println("deleting " + Show::show(*p));
                //Show::println("GS: deleting... " + Show::show(p->getType()));
                delete p;
            }
        }
    };

    /*
    ** 'per thread' state
    */
    struct State : public GCObject {
        U8 status;
        U8 allowhook;
        StkIdx top;  /* first free slot in the stack */
        GlobalState *l_G;
        list<CallInfo> cis;  /* call info for current function */
        list<CallInfo>::iterator ci;
        StkIdx base;  /* end of stack (last element + 1) */
        vector<StackValue> stack;  /* stack base */
        vector<UpVal> openupval;  /* list of open upvalues in this stack */
        GCObject *gclist;
        State *twups;  /* list of threads with open upvalues */
        //struct abyss_longjmp *errorJmp;  /* current error recover point */
        CallInfo base_ci;  /* CallInfo for first level (C calling Abyss) */
        //volatile abyss_Hook hook;
        ptrdiff_t errfunc;  /* current error handling function (stack index) */
        U32 nCcalls;  /* number of nested (non-yieldable | C)  calls */
        I32 oldpc;  /* last pc traced */
        I32 basehookcount;
        I32 hookcount;
        State()
            : status(1),
              allowhook(1),
              errfunc(0),
              nCcalls(0),
              oldpc(0),
              basehookcount(0)
        {
            initStack();
        }
        State(GlobalState *g)
            : status(1),
              allowhook(1),
              errfunc(0),
              nCcalls(0),
              oldpc(0),
              basehookcount(0)
        {
            initStack();
            cis = {};
            ci = cis.begin();
            l_G = g;
        }
        /* Each State may have a base callinfo when created */
        State(GlobalState *g, const CallInfo &ci_base)
            : status(1),
              allowhook(1),
              base_ci(ci_base),
              errfunc(0),
              nCcalls(0),
              oldpc(0),
              basehookcount(0)
        {
            initStack();
            cis = {ci_base};
            ci = cis.begin();
            l_G = g;
        }
        /**
           Register new CallInfo!
         */
        State &appendCallInfo(const CallInfo &ci) {
            //ensure(S.ci + 1 == S.cis.end());
            if(this->cis.empty()) {
                this->base_ci = ci;
                this->cis.push_back(ci); //Chain this ci
                this->ci = cis.begin();
                return *this;
            }
            this->cis.push_back(ci); //Chain this ci
            this->ci++; //Move to current ci
            this->ci->ready = true;
            return *this;
        }
        void printCurrentStackFrame() {
            auto &cci = ci == cis.begin() ? base_ci : *ci;
            auto func = cci.func;
            auto top = cci.top;
            unsigned long i = 0;
            Show::println("========== Current Stack Frame ==========");
            Show::println(">> SIZE: " + Show::show(top - func));
            for(auto it = func; i < 20 && it <= top; ++it, ++i) {
                Show::print(stack[it].val);
                Show::print("\t\t|");
                Show::println(i == (this->top - cci.func) ? "<----- top" : "");
            }
            Show::println("========== end Current Frame ==========");
        }
        State &nextCI() {
            /* Unimplemented! */
            return *this;
        }
        template <class T, class... Args>
        T &newGCObject(Args&&... args) {
            T *obj = new T(args...);
            G()->allgc.push_back(obj);
            G()->totalbytes += sizeof(T);
            if(G()->lastbytes * 2 <= G()->totalbytes) {
                //collect(*this);
            }
            return *obj;
        }
        template <class T, class... Args>
        T &newFixedObject(Args&&... args) {
            T *obj = new T(args...);
            G()->fixedgc.push_back(obj);
            return *obj;
        }

        optional<CallInfo*> call(StkId func, I32 n_results);//I dont know why C++ 17 does not support optional<T&>...
        LClosure &setMainClosure() {
            auto lam = Lambda();
            lam.name = "main";
            lam.code = {};
            auto &cl = this->newFixedObject<LClosure>(lam);
            Value cv = Value(&cl);
            this->stack[0] = cv;
            return cl;
        }
        ~State() {
        }

    private:
        void initStack() {
            this->stack = std::vector<StackValue>(100, abyss::None);
            this->top = 0;//this->stack.begin();
            //this->stack_last = this->top + STACK_SIZE;
            CallInfo &ci = this->base_ci;
            ci.next = ci.previous = nullptr;
            ci.func = this->top;
            ci.c.k = nullptr;
            ci.nresults = 0;
            this->top += 1;
            ci.top = this->top + 20;
            ci.n_top = 20;
            ci.ready = true;
        }
        GlobalState *G() {
            return this->l_G;
        }

    };
    inline GlobalState *G(State *S) {
        return S->l_G;
    }


    /**
       This is evil trick!
       If func is C closure, we call it directly and return nullptr
       to indicate this is C closure;

       If func is abyss closure, we set basic callinfo and return it;
       @author: Ireina
     */
    CallInfo *precall(State &S, StkId func, I32 n_results);


}







#endif
