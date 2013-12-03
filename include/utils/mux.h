void mux(void **input, void *output, int *input_lengths, int *input_padding_pre, int nof_inputs,
		int sample_sz);

void demux(void *input, void **output, int *output_lengths,
		int *output_padding_pre, int *output_padding_post, int nof_outputs,
		int sample_sz);
