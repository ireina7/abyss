#include "GC.hpp"
#include "State.hpp"

namespace abyss {

    I32 traverseStack(State &S, I32 bound) {

        vector<StackValue>::size_type i;
        for(i = 0; i <= S.top; ++i) {

            Value &v = S.stack[i].val;
            if(v.tag.tag != static_cast<U8>(object::TNONE) &&
               v.tag.canCollect()) {
                //Show::println(v);
                auto &gc = Cast::to<GCObject&>(v);
                gc.marked = Black;
                //Show::println("marked: " + Show::show(gc.marked));
                //Show::println("< collect!");
            }

        }

        //Show::println("end collect");
        return 0;
    }

    I32 traverseAll(State &S) {
        traverseStack(S);
        return 0;
    }

    I32 collect(State &S) {
        traverseAll(S);
        I32 cnt = 0;
        auto it = S.l_G->allgc.begin();
        ++it, ++it;
        for(; it != S.l_G->allgc.end();) {
            //Show::println("collect!");
            auto &gc = *it;
            //Show::println(gc->marked);
            if(gc->marked == Black) {
                //Show::println("Black: " + Show::show(*gc));
                gc->marked = White;
                ++it;
            }
            else {
                Show::println("> deleting " + Show::show(*gc));
                GCObject *gg = gc;
                //Cast::to<Value&>(*gc) = None;
                delete gc;
                cnt += 1;
                it = S.l_G->allgc.erase(it);
            }
        }
        return cnt;
    }

}
