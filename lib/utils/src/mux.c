#include <string.h>

/**
 * Multiplexes a signal from nof_inputs interfaces.
 * Copies output_lengths[i] samples to the i-th interfaces, ignores output_padding_pre[i] samples
 * from the beginning each input interface.
 */
void mux(void **input, void *output, int *input_lengths, int *input_padding_pre, int nof_inputs,
		int sample_sz) {
	int i,r;
	char *out = (char*) output;
	char **in = (char**) input;

	r=0;
	for (i=0;i<nof_inputs;i++) {
		memcpy(&out[r*sample_sz],&in[i][sample_sz*input_padding_pre[i]],sample_sz*input_lengths[i]);
		r+=input_lengths[i];
	}
}

/**
 * De-multiplexes a signal to nof_outputs interfaces.
 * Copies output_lengths[i] samples to the i-th interfaces, adds output_padding_pre[i] zeros
 * to the beginning and output_padding_post[i] zeros to the end.
 */
void demux(void *input, void **output, int *output_lengths,
		int *output_padding_pre, int *output_padding_post, int nof_outputs,
		int sample_sz) {
	int i,r;
	char **out = (char**) output;
	char *in = (char*) input;

	r=0;
	for (i=0;i<nof_outputs;i++) {
		memset(&out[i][0],0,sample_sz*output_padding_pre[i]);
		memcpy(&out[i][sample_sz*output_padding_pre[i]],&in[r*sample_sz],sample_sz*output_lengths[i]);
		memset(&out[i][sample_sz*(output_padding_pre[i]+output_lengths[i])],0,sample_sz*output_padding_post[i]);
		r+=output_lengths[i]+output_padding_post[i];
	}
}
