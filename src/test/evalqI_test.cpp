#include "HEAAN/HEAAN.h"
#include "HEAAN/HEAAN_bootstrap.h"
#include "util/util.h"

#include "experiment/rns_target.h"

#include <iostream>

void evalqI_test()
{
    int s[N], s_sq[N];
    HEAAN<LOGQ,N>::keygen(H,s);

    Message<LOGN> z, z_out;
    set_test_message(z);

    R_Q<LOGq, N> pt;
    R_Q_square<LOGq,N> ct;
    encode(z,Delta,pt);
    HEAAN<LOGq,N>::enc(pt,s,ct);

	// ModRaise
	R_Q_square<LOGQ,N> ct_modraise;
	mod_raise<LOGq,LOGQ,N>(ct,ct_modraise);

	Timer t1("cts");
	// CoeffToSlot
	R_Q_square<LOGQ,N> ct_cts[2];
	CoeffToSlot<LOGQ,LOGN,LOGDELTA_boot_tilde,G>(ct_modraise,s,ct_cts);
	const int LOGQ_after_cts = LOGQ-(LOGN-1)/G*LOGDELTA_boot_tilde;
	R_Q_square<LOGQ_after_cts,N> ct_ctsrs[2];
	for(int i=0;i<2;i++)
	    RS<LOGQ,LOGQ_after_cts,N>(ct_cts[i],ct_ctsrs[i]);
	t1.stop();

	Timer t2("evalqI");
	// EvalMod
	const int LOGQ_after_evalmod = LOGQ_after_cts - 12*LOGDELTA_boot;
	R_Q_square<LOGQ_after_evalmod,N> ct_evalmod[2];
	for(int i=0;i<2;i++)
	    EvalMod<LOGQ_after_cts,N,LOGDELTA_boot,K>(ct_ctsrs[i],s,ct_evalmod[i]);
	
	// EvalqI
	R_Q_square<LOGQ_after_evalmod,N> ct_evalqI[2];
	for(int i = 0; i < 2; i++) {
		resize(ct_ctsrs[i], ct_evalqI[i]);
		ct_evalqI[i] -= ct_evalmod[i];
	}
	t2.stop();
	
	Timer t3("stc");
	// SlotToCoeff
	const int LOGQ_after_stc = LOGQ_after_evalmod - (LOGN-1)/G*LOGDELTA_boot;
    R_Q_square<LOGQ_after_evalmod,N> ct_stc;
	R_Q_square<LOGQ_after_stc,N> ct_qI;
	SlotToCoeff<LOGQ_after_evalmod,LOGN,LOGDELTA_boot,G>(ct_evalqI[0],ct_evalqI[1],s,ct_stc);
	RS<LOGQ_after_evalmod,LOGQ_after_stc,N>(ct_stc, ct_qI);
	t3.stop();
	
	// Sub
	R_Q_square<LOGQ_after_stc,N> ct_boot;
	resize(ct_modraise, ct_boot);
	ct_boot -= ct_qI;

	R_Q<LOGQ_after_stc,N> pt_out;
	HEAAN<LOGQ_after_stc,N>::dec(ct_boot,s,pt_out);
	decode_log(pt_out,LOGDELTA,z_out);
	print("z", z);
	print("z_out", z_out);
	print_max_error<LOGN>(z,z_out);
}

int main()
{
	evalqI_test();
}
