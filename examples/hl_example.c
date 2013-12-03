#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>

#include "osld-lib.h"
#include "utils/bit.h"

void usage(char *arg) {
	printf("Usage: %s nbits snr_db\n",arg);
}

int main(int argc, char **argv) {
	binsource_hl bs;
	mod_hl mod;
	ch_awgn_hl ch;
	demod_soft_hl demod_s;
	demod_hard_hl demod_h;

	bzero(&bs,sizeof(bs));
	bzero(&mod,sizeof(mod));
	bzero(&ch,sizeof(ch));
	bzero(&demod_s,sizeof(demod_s));
	bzero(&demod_h,sizeof(demod_h));

	if (argc<3) {
		usage(argv[0]);
		exit(-1);
	}

	int nbits = atoi(argv[1]);
	float snr_db = atof(argv[2]);
	float var = sqrt(pow(10,-snr_db/10));

	bs.init.seed = 0;
	bs.init.cache_seq_nbits = 0;
	bs.ctrl_in.nbits = nbits;
	bs.output = malloc(nbits);

	mod.in_len = nbits;
	mod.init.std = LTE_BPSK;
	mod.input = bs.output;
	mod.output = malloc(nbits*sizeof(_Complex float));

	ch.in_len = nbits;
	ch.input = mod.output;
	ch.ctrl_in.variance = var;
	ch.output = malloc(nbits*sizeof(_Complex float));

	demod_h.in_len = nbits;
	demod_h.init.std = LTE_BPSK;
	demod_h.input = ch.output;
	demod_h.output = malloc(nbits);

	demod_s.in_len = nbits;
	demod_s.init.std = LTE_BPSK;
	demod_s.input = ch.output;
	demod_s.output = malloc(sizeof(float)*nbits);
	demod_s.ctrl_in.alg_type = APPROX;
	demod_s.ctrl_in.sigma = var;

	if (	binsource_initialize(&bs) 		||
			mod_initialize(&mod) 			||
			ch_awgn_initialize(&ch) 			||
			demod_hard_initialize(&demod_h) ||
			demod_soft_initialize(&demod_s)
	) {
		printf("Error initializing modules\n");
		exit(-1);
	}

	binsource_work(&bs);
	mod_work(&mod);
	ch_awgn_work(&ch);
	demod_hard_work(&demod_h);
	demod_soft_work(&demod_s);

	/* hard decision for soft demodulation */
	uint8_t* tmp = malloc(nbits);
	for (int i=0;i<nbits;i++) {
		tmp[i] = demod_s.output[i]>0?1:0;
	}

	printf("Hard errors: %d/%d\n",bit_diff(bs.output,demod_h.output,nbits),nbits);
	printf("Soft errors: %d/%d\n",bit_diff(bs.output,tmp,nbits),nbits);

	free(bs.output);
	free(mod.output);
	free(ch.output);
	free(demod_h.output);
	free(demod_s.output);
	free(tmp);

	printf("Exit\n");
	exit(0);
}
