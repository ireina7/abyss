#ifndef OPCODE_JPP
#define OPCODE_JPP

#include "Limit.hpp"
#include "Utils.hpp"

namespace abyss {
    enum OpMode {iABC, iABx, iAsBx, iAx, isJ};  /* basic instruction formats */

    /*
    ** grep "ORDER OP" if you change these enums
    */
    enum OpCode {
                 /*----------------------------------------------------------------------
                   name		args	description
                   ------------------------------------------------------------------------*/
                 OP_MOVE,/*	A B	R[A] := R[B]					*/
                 OP_LOADI,/*	A sBx	R[A] := sBx					*/
                 OP_LOADF,/*	A sBx	R[A] := (abyss_Number)sBx				*/
                 OP_LOADK,/*	A Bx	R[A] := K[Bx]					*/
                 OP_LOADKX,/*	A	R[A] := K[extra arg]				*/
                 OP_LOADFALSE,/*	A	R[A] := false					*/
                 OP_LFALSESKIP,/*A	R[A] := false; pc++				*/
                 OP_LOADTRUE,/*	A	R[A] := true					*/
                 OP_LOADNIL,/*	A B	R[A], R[A+1], ..., R[A+B] := nil		*/
                 OP_GETUPVAL,/*	A B	R[A] := UpValue[B]				*/
                 OP_SETUPVAL,/*	A B	UpValue[B] := R[A]				*/

                 OP_GETTABUP,/*	A B C	R[A] := UpValue[B][K[C]:string]			*/
                 OP_GETTABLE,/*	A B C	R[A] := R[B][R[C]]				*/
                 OP_GETI,/*	A B C	R[A] := R[B][C]					*/
                 OP_GETFIELD,/*	A B C	R[A] := R[B][K[C]:string]			*/

                 OP_SETTABUP,/*	A B C	UpValue[A][K[B]:string] := RK(C)		*/
                 OP_SETTABLE,/*	A B C	R[A][R[B]] := RK(C)				*/
                 OP_SETI,/*	A B C	R[A][B] := RK(C)				*/
                 OP_SETFIELD,/*	A B C	R[A][K[B]:string] := RK(C)			*/

                 OP_NEWTABLE,/*	A B C k	R[A] := {}					*/

                 OP_SELF,/*	A B C	R[A+1] := R[B]; R[A] := R[B][RK(C):string]	*/

                 OP_ADDI,/*	A B sC	R[A] := R[B] + sC				*/

                 OP_ADDK,/*	A B C	R[A] := R[B] + K[C]				*/
                 OP_SUBK,/*	A B C	R[A] := R[B] - K[C]				*/
                 OP_MULK,/*	A B C	R[A] := R[B] * K[C]				*/
                 OP_MODK,/*	A B C	R[A] := R[B] % K[C]				*/
                 OP_POWK,/*	A B C	R[A] := R[B] ^ K[C]				*/
                 OP_DIVK,/*	A B C	R[A] := R[B] / K[C]				*/
                 OP_IDIVK,/*	A B C	R[A] := R[B] // K[C]				*/

                 OP_BANDK,/*	A B C	R[A] := R[B] & K[C]:integer			*/
                 OP_BORK,/*	A B C	R[A] := R[B] | K[C]:integer			*/
                 OP_BXORK,/*	A B C	R[A] := R[B] ~ K[C]:integer			*/

                 OP_SHRI,/*	A B sC	R[A] := R[B] >> sC				*/
                 OP_SHLI,/*	A B sC	R[A] := sC << R[B]				*/

                 OP_ADD,/*	A B C	R[A] := R[B] + R[C]				*/
                 OP_SUB,/*	A B C	R[A] := R[B] - R[C]				*/
                 OP_MUL,/*	A B C	R[A] := R[B] * R[C]				*/
                 OP_MOD,/*	A B C	R[A] := R[B] % R[C]				*/
                 OP_POW,/*	A B C	R[A] := R[B] ^ R[C]				*/
                 OP_DIV,/*	A B C	R[A] := R[B] / R[C]				*/
                 OP_IDIV,/*	A B C	R[A] := R[B] // R[C]				*/

                 OP_BAND,/*	A B C	R[A] := R[B] & R[C]				*/
                 OP_BOR,/*	A B C	R[A] := R[B] | R[C]				*/
                 OP_BXOR,/*	A B C	R[A] := R[B] ~ R[C]				*/
                 OP_SHL,/*	A B C	R[A] := R[B] << R[C]				*/
                 OP_SHR,/*	A B C	R[A] := R[B] >> R[C]				*/

                 OP_MMBIN,/*	A B C	call C metamethod over R[A] and R[B]		*/
                 OP_MMBINI,/*	A sB C k	call C metamethod over R[A] and sB	*/
                 OP_MMBINK,/*	A B C k		call C metamethod over R[A] and K[B]	*/

                 OP_UNM,/*	A B	R[A] := -R[B]					*/
                 OP_BNOT,/*	A B	R[A] := ~R[B]					*/
                 OP_NOT,/*	A B	R[A] := not R[B]				*/
                 OP_LEN,/*	A B	R[A] := #R[B] (length operator)			*/

                 OP_CONCAT,/*	A B	R[A] := R[A].. ... ..R[A + B - 1]		*/

                 OP_CLOSE,/*	A	close all upvalues >= R[A]			*/
                 OP_TBC,/*	A	mark variable A "to be closed"			*/
                 OP_JMP,/*	sJ	pc += sJ					*/
                 OP_EQ,/*	A B k	if ((R[A] == R[B]) ~= k) then pc++		*/
                 OP_LT,/*	A B k	if ((R[A] <  R[B]) ~= k) then pc++		*/
                 OP_LE,/*	A B k	if ((R[A] <= R[B]) ~= k) then pc++		*/

                 OP_EQK,/*	A B k	if ((R[A] == K[B]) ~= k) then pc++		*/
                 OP_EQI,/*	A sB k	if ((R[A] == sB) ~= k) then pc++		*/
                 OP_LTI,/*	A sB k	if ((R[A] < sB) ~= k) then pc++			*/
                 OP_LEI,/*	A sB k	if ((R[A] <= sB) ~= k) then pc++		*/
                 OP_GTI,/*	A sB k	if ((R[A] > sB) ~= k) then pc++			*/
                 OP_GEI,/*	A sB k	if ((R[A] >= sB) ~= k) then pc++		*/

                 OP_TEST,/*	A k	if (not R[A] == k) then pc++			*/
                 OP_TESTSET,/*	A B k	if (not R[B] == k) then pc++ else R[A] := R[B]	*/

                 OP_CALL,/*	A B C	R[A], ... ,R[A+C-2] := R[A](R[A+1], ... ,R[A+B-1]) */
                 OP_TAILCALL,/*	A B C k	return R[A](R[A+1], ... ,R[A+B-1])		*/

                 OP_RETURN,/*	A B C k	return R[A], ... ,R[A+B-2]	(see note)	*/
                 OP_RETURN0,/*		return						*/
                 OP_RETURN1,/*	A	return R[A]					*/

                 OP_FORLOOP,/*	A Bx	update counters; if loop continues then pc-=Bx; */
                 OP_FORPREP,/*	A Bx	<check values and prepare counters>;
                                if not to run then pc+=Bx+1;			*/

                 OP_TFORPREP,/*	A Bx	create upvalue for R[A + 3]; pc+=Bx		*/
                 OP_TFORCALL,/*	A C	R[A+4], ... ,R[A+3+C] := R[A](R[A+1], R[A+2]);	*/
                 OP_TFORLOOP,/*	A Bx	if R[A+2] ~= nil then { R[A]=R[A+2]; pc -= Bx }	*/

                 OP_SETLIST,/*	A B C k	R[A][C+i] := R[A+i], 1 <= i <= B		*/

                 OP_CLOSURE,/*	A Bx	R[A] := closure(KPROTO[Bx])			*/

                 OP_VARARG,/*	A C	R[A], R[A+1], ..., R[A+C-2] = vararg		*/

                 OP_VARARGPREP,/*A	(adjust vararg parameters)			*/

                 OP_EXTRAARG,/*	Ax	extra (larger) argument for previous opcode	*/
                 OP_PASS,/* Do nothing */
    };


} //end namespace abyss



namespace Show {

    template<>
    inline std::string show(const abyss::OpCode &op) {
        std::string ans = "";
        switch(op) {
        case abyss::OP_MOVE: ans = "MOVE" ; break;
        case abyss::OP_LOADI: ans = "LOADI"; break;
        case abyss::OP_LOADF: ans = "LOADF"; break;
        case abyss::OP_LOADK: ans = "LOADK"; break;
        case abyss::OP_LOADKX: ans = "LOADKX"; break;
        case abyss::OP_LOADFALSE: ans = "LOADFALSE"; break;
        case abyss::OP_LFALSESKIP: ans = "LFALSESKIP"; break;
        case abyss::OP_LOADTRUE: ans = "LOADTRUE"; break;
        case abyss::OP_LOADNIL: ans = "LOADNIL"; break;
        case abyss::OP_GETUPVAL: ans = "GETUPVAL"; break;
        case abyss::OP_SETUPVAL: ans = "SETUPVAL"; break;
        case abyss::OP_GETTABUP: ans = "GETTABUP"; break;
        case abyss::OP_GETTABLE: ans = "GETTABLE"; break;
        case abyss::OP_GETI: ans = "GETI"; break;
        case abyss::OP_GETFIELD: ans = "GETFIELD"; break;
        case abyss::OP_SETTABUP: ans = "SETTABUP"; break;
        case abyss::OP_SETTABLE: ans = "SETTABLE"; break;
        case abyss::OP_SETI: ans = "SETI"; break;
        case abyss::OP_SETFIELD: ans = "SETFIELD"; break;
        case abyss::OP_NEWTABLE: ans = "NEWTABLE"; break;
        case abyss::OP_SELF: ans = "SELF"; break;
        case abyss::OP_ADDI: ans = "ADDI"; break;
        case abyss::OP_ADDK: ans = "ADDK"; break;
        case abyss::OP_SUBK: ans = "SUBK"; break;
        case abyss::OP_MULK: ans = "MULK"; break;
        case abyss::OP_MODK: ans = "MODK"; break;
        case abyss::OP_POWK: ans = "POWK"; break;
        case abyss::OP_DIVK: ans = "DIVK"; break;
        case abyss::OP_IDIVK: ans = "IDIVK"; break;
        case abyss::OP_BANDK: ans = "BANDK"; break;
        case abyss::OP_BORK: ans = "BORK"; break;
        case abyss::OP_BXORK: ans = "BXORK"; break;
        case abyss::OP_SHRI: ans = "SHRI"; break;
        case abyss::OP_SHLI: ans = "SHLI"; break;
        case abyss::OP_ADD: ans = "ADD"; break;
        case abyss::OP_SUB: ans = "SUB"; break;
        case abyss::OP_MUL: ans = "MUL"; break;
        case abyss::OP_MOD: ans = "MOD"; break;
        case abyss::OP_POW: ans = "POW"; break;
        case abyss::OP_DIV: ans = "DIV"; break;
        case abyss::OP_IDIV: ans = "IDIV"; break;
        case abyss::OP_BAND: ans = "BAND"; break;
        case abyss::OP_BOR: ans = "BOR"; break;
        case abyss::OP_BXOR: ans = "BXOR"; break;
        case abyss::OP_SHL: ans = "SHL"; break;
        case abyss::OP_SHR: ans = "SHR"; break;
        case abyss::OP_MMBIN: ans = "MMBIN"; break;
        case abyss::OP_MMBINI: ans = "MMBINI"; break;
        case abyss::OP_MMBINK: ans = "MMBINK"; break;
        case abyss::OP_UNM: ans = "UNM"; break;
        case abyss::OP_BNOT: ans = "BNOT"; break;
        case abyss::OP_NOT: ans = "NOT"; break;
        case abyss::OP_LEN: ans = "LEN"; break;
        case abyss::OP_CONCAT: ans = "CONCAT"; break;
        case abyss::OP_CLOSE: ans = "CLOSE"; break;
        case abyss::OP_TBC: ans = "TBC"; break;
        case abyss::OP_JMP: ans = "JMP"; break;
        case abyss::OP_EQ: ans = "EQ"; break;
        case abyss::OP_LT: ans = "LT"; break;
        case abyss::OP_LE: ans = "LE"; break;
        case abyss::OP_EQK: ans = "EQK"; break;
        case abyss::OP_EQI: ans = "EQI"; break;
        case abyss::OP_LTI: ans = "LTI"; break;
        case abyss::OP_LEI: ans = "LEI"; break;
        case abyss::OP_GTI: ans = "GTI"; break;
        case abyss::OP_GEI: ans = "GEI"; break;
        case abyss::OP_TEST: ans = "TEST"; break;
        case abyss::OP_TESTSET: ans = "TESTSET"; break;
        case abyss::OP_CALL: ans = "CALL"; break;
        case abyss::OP_TAILCALL: ans = "TAILCALL"; break;
        case abyss::OP_RETURN: ans = "RETURN"; break;
        case abyss::OP_RETURN0: ans = "RETURN0"; break;
        case abyss::OP_RETURN1: ans = "RETURN1"; break;
        case abyss::OP_FORLOOP: ans = "FORLOOP"; break;
        case abyss::OP_FORPREP: ans = "FORPREP"; break;
        case abyss::OP_TFORPREP: ans = "TFORPREP"; break;
        case abyss::OP_TFORCALL: ans = "TFORCALL"; break;
        case abyss::OP_TFORLOOP: ans = "TFORLOOP"; break;
        case abyss::OP_SETLIST: ans = "SETLIST"; break;
        case abyss::OP_CLOSURE: ans = "CLOSURE"; break;
        case abyss::OP_VARARG: ans = "VARARG"; break;
        case abyss::OP_VARARGPREP: ans = "VARARGPREP"; break;
        case abyss::OP_EXTRAARG: ans = "EXTRAARG"; break;
        case abyss::OP_PASS: ans = "PASS"; break;
            //default: ans = "UNKNOWN";
        }
        return ans;
    }

    template<>
    inline std::string show(const abyss::OpMode &m) {
        std::string ans = "";
        switch(m) {
        case abyss::iABC : ans = "iABC" ; break;
        case abyss::iABx : ans = "iABx" ; break;
        case abyss::iAsBx: ans = "iAsBx"; break;
        case abyss::iAx  : ans = "iAx"  ; break;
        case abyss::isJ  : ans = "isJ"  ; break;
        default: ans = show(m);
        }
        return ans;
    }
}






#endif
