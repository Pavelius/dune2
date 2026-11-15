#include "game.h"

#define RS(n) (sprite*)bin_##n

extern unsigned char bin_new6p[];
extern unsigned char bin_new8p[];
extern unsigned char bin_new10p[];
extern unsigned char bin_std3p[];
extern unsigned char bin_intro16[];
extern unsigned char bin_mentats[];
extern unsigned char bin_menshpa[];
extern unsigned char bin_menshph[];
extern unsigned char bin_menshpo[];
extern unsigned char bin_menshpm[];
extern unsigned char bin_shapes[];
extern unsigned char bin_icons[];
extern unsigned char bin_arrows[];
extern unsigned char bin_mouse[];
extern unsigned char bin_screen[];
extern unsigned char bin_choam[];
extern unsigned char bin_units[];
extern unsigned char bin_units1[];
extern unsigned char bin_units2[];
extern unsigned char bin_4slab[];
extern unsigned char bin_afinala[];
extern unsigned char bin_afinalb[];
extern unsigned char bin_barrac[];
extern unsigned char bin_carryall[];
extern unsigned char bin_construc[];
extern unsigned char bin_efinala[];
extern unsigned char bin_efinalb[];
extern unsigned char bin_fartr[];
extern unsigned char bin_fhark[];
extern unsigned char bin_fordos[];
extern unsigned char bin_fremen[];
extern unsigned char bin_gold_bb[];
extern unsigned char bin_harktank[];
extern unsigned char bin_harvest[];
extern unsigned char bin_headqrts[];
extern unsigned char bin_hfinala[];
extern unsigned char bin_hfinalb[];
extern unsigned char bin_hfinalc[];
extern unsigned char bin_hitcftry[];
extern unsigned char bin_htank[];
extern unsigned char bin_hvyftry[];
extern unsigned char bin_hyinfy[];
extern unsigned char bin_infantry[];
extern unsigned char bin_intro1[];
extern unsigned char bin_intro10[];
extern unsigned char bin_intro11[];
extern unsigned char bin_intro2[];
extern unsigned char bin_intro3[];
extern unsigned char bin_intro4[];
extern unsigned char bin_intro5[];
extern unsigned char bin_intro6[];
extern unsigned char bin_intro7a[];
extern unsigned char bin_intro7b[];
extern unsigned char bin_intro8a[];
extern unsigned char bin_intro8b[];
extern unsigned char bin_intro8c[];
extern unsigned char bin_intro9[];
extern unsigned char bin_ix[];
extern unsigned char bin_liteftry[];
extern unsigned char bin_lostbild[];
extern unsigned char bin_lostvehc[];
extern unsigned char bin_ltank[];
extern unsigned char bin_mcv[];
extern unsigned char bin_meanwhil[];
extern unsigned char bin_ofinala[];
extern unsigned char bin_ofinalb[];
extern unsigned char bin_ofinalc[];
extern unsigned char bin_ofinald[];
extern unsigned char bin_ordrtank[];
extern unsigned char bin_orni[];
extern unsigned char bin_otrike[];
extern unsigned char bin_palace[];
extern unsigned char bin_quad[];
extern unsigned char bin_refinery[];
extern unsigned char bin_repair[];
extern unsigned char bin_rtank[];
extern unsigned char bin_rturret[];
extern unsigned char bin_saboture[];
extern unsigned char bin_sardukar[];
extern unsigned char bin_slab[];
extern unsigned char bin_stank[];
extern unsigned char bin_starport[];
extern unsigned char bin_static[];
extern unsigned char bin_storage[];
extern unsigned char bin_trike[];
extern unsigned char bin_turret[];
extern unsigned char bin_wall[];
extern unsigned char bin_westwood[];
extern unsigned char bin_win1[];
extern unsigned char bin_win2[];
extern unsigned char bin_windtrap[];
extern unsigned char bin_wor[];
extern unsigned char bin_worm[];

sprite* res_data[LastRes + 1] = {
	0, RS(new6p), RS(new8p), RS(new10p), RS(std3p),
	RS(intro16), RS(mentats), RS(menshpa), RS(menshph), RS(menshpo),
	RS(menshpm), RS(shapes), RS(icons), RS(arrows), RS(mouse),
	RS(screen), RS(choam), RS(units), RS(units1), RS(units2),
	RS(4slab), RS(afinala), RS(afinalb), RS(barrac), RS(carryall),
	RS(construc), RS(efinala), RS(efinalb), RS(fartr), RS(fhark),
	RS(fordos), RS(fremen), RS(gold_bb), RS(harktank), RS(harvest),
	RS(headqrts), RS(hfinala), RS(hfinalb), RS(hfinalc), RS(hitcftry),
	RS(htank), RS(hvyftry), RS(hyinfy), RS(infantry), RS(intro1),
	RS(intro10), RS(intro11), RS(intro2), RS(intro3), RS(intro4),
	RS(intro5), RS(intro6), RS(intro7a), RS(intro7b), RS(intro8a),
	RS(intro8b), RS(intro8c), RS(intro9), RS(ix), RS(liteftry),
	RS(lostbild), RS(lostvehc), RS(ltank), RS(mcv), RS(meanwhil),
	RS(ofinala), RS(ofinalb), RS(ofinalc), RS(ofinald), RS(ordrtank),
	RS(orni), RS(otrike), RS(palace), RS(quad), RS(refinery),
	RS(repair), RS(rtank), RS(rturret), RS(saboture), RS(sardukar),
	RS(slab), RS(stank), RS(starport), RS(static), RS(storage),
	RS(trike), RS(turret), RS(wall), RS(westwood), RS(win1),
	RS(win2), RS(windtrap), RS(wor), RS(worm)
};