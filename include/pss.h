/* 
 * Copyright (c) 2013, Ismael Gomez-Miguelez <ismael.gomez@tsc.upc.edu>.
 * This file is part of ALOE++ (http://flexnets.upc.edu/)
 * 
 * ALOE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ALOE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with ALOE++.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdbool.h>
#include "nco.h"
#include "utils/convolution.h"

typedef _Complex float cf_t; /* this is only a shortcut */

#define CONVOLUTION_FFT
#define DEFAULT_CORRELATION_TH 10000
#define DEFAULT_NOSYNC_TIMEOUT	5

#define PSS_LEN_FREQ 	129
#define PSS_LEN			62
#define PSS_RE			6*12



/** The pss_synch_t object provides functions for fast computation of the crosscorrelation
 * between the PSS and received signal and CFO estimation. Also, the function pss_synch_periodic() is designed
 * to be called periodically every subframe, taking care of the correct data alignment with respect
 * to the PSS sequence.
 */


/* Low-level API */
typedef struct {

#ifdef CONVOLUTION_FFT
	conv_fft_cc_t conv_fft;
#endif

	int subframe_size;
	int N_id_2;
	float current_cfo;
	bool cfo_auto;					// default true
	int nof_nosync_frames;
	int nosync_timeout_frames;		// default 5
	float correlation_threshold; 	// default 10000
	int frame_start_idx;
	int fb_wp;

	nco_t nco;

	cf_t *pss_signal_freq;
	cf_t *tmp_input;
	float *conv_abs;
	cf_t *frame_buffer;
	cf_t *conv_output;
	cf_t *tmp_nco;
}pss_synch_t;

/* Basic functionality */

int pss_synch_init(pss_synch_t *q, int subframe_size);
void pss_synch_free(pss_synch_t *q);
int pss_generate(cf_t *signal, int direction, int N_id_2);

int pss_synch_set_N_id_2(pss_synch_t *q, int N_id_2);
int pss_synch_find_pss(pss_synch_t *q, cf_t *input, float *corr_peak_value);
float pss_synch_cfo_compute(pss_synch_t* q, cf_t *pss_recv);
void pss_synch_cfo_correct(pss_synch_t *q, cf_t *input, float cfo); // if cfo_auto=true, cfo parameter is ignored






/* Automatic frame management functions (for periodic calling) */
int pss_synch_periodic(pss_synch_t *q, cf_t *input, cf_t *output, int nsamples);
void pss_synch_set_timeout(pss_synch_t *q, int nof_frames);
void pss_synch_set_threshold(pss_synch_t *q, float threshold);
void pss_synch_set_cfo_mode(pss_synch_t *q, bool cfo_auto);
float pss_synch_get_cfo(pss_synch_t *q);
int pss_synch_get_frame_start_idx(pss_synch_t *q);








/* High-level API */

typedef struct {
	pss_synch_t obj;
	struct pss_synch_init {
		int frame_size;				// if 0, 2048
		int unsync_nof_pkts;
		int N_id_2;
		int do_cfo;
	} init;
	cf_t *input;
	int in_len;
	struct pss_synch_ctrl_in {
		int correlation_threshold;
		float manual_cfo;
	} ctrl_in;
	cf_t *output;
	int *out_len;
}pss_synch_hl;

#define DEFAULT_FRAME_SIZE		2048

int pss_synch_initialize(pss_synch_hl* h);
int pss_synch_work(pss_synch_hl* hl);
int pss_synch_stop(pss_synch_hl* hl);
