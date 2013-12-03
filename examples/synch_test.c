#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "filesource.h"
#include "filesink.h"
#include "pss.h"
#include "sss.h"

char *input_file_name;
char *output_file_name="output.txt";
int nof_frames=-1, frame_length=1920;
float corr_peak_threshold=10000;
int N_id_2;

void usage(char *prog) {
	printf("Usage: %s [onlt] -i input_file -N N_id_2\n", prog);
	printf("\t-o output_file\n");
	printf("\t-l frame_length\n");
	printf("\t-n number of frames\n");
	printf("\t-t correlation threshold\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "ionltN")) != -1) {
		switch(opt) {
		case 'i':
			input_file_name = argv[optind];
			break;
		case 'o':
			output_file_name = argv[optind];
			break;
		case 'n':
			nof_frames = atoi(argv[optind]);
			break;
		case 'l':
			frame_length = atoi(argv[optind]);
			break;
		case 't':
			corr_peak_threshold = atof(argv[optind]);
			break;
		case 'N':
			N_id_2 = atoi(argv[optind]);
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
	if (!input_file_name) {
		usage(argv[0]);
		exit(-1);
	}
}

int main(int argc, char **argv) {
	filesource_t fsrc;
	filesink_t fsink;
	pss_synch_t pss;
	sss_synch_t sss;
	int peak_pos;
	float peak_value;
	int frame_cnt;
	cf_t *input;
	int m0, m1;
	float m0_value, m1_value;

	if (argc < 4) {
		usage(argv[0]);
		exit(-1);
	}

	parse_args(argc,argv);

	if (filesource_init(&fsrc, input_file_name, COMPLEX_FLOAT)) {
		fprintf(stderr, "Error opening file %s\n", input_file_name);
		exit(-1);
	}
	if (filesink_init(&fsink, output_file_name, FLOAT)) {
		fprintf(stderr, "Error opening file %s\n", output_file_name);
		exit(-1);
	}

	input = malloc(frame_length*sizeof(cf_t));
	bzero(input, sizeof(cf_t)*frame_length);

	if (pss_synch_init(&pss, frame_length)) {
		fprintf(stderr, "Error initializing PSS object\n");
		exit(-1);
	}
	if (pss_synch_set_N_id_2(&pss, N_id_2)) {
		fprintf(stderr, "Error initializing N_id_2\n");
		exit(-1);
	}

	if (sss_synch_init(&sss)) {
		fprintf(stderr, "Error initializing SSS object\n");
		exit(-1);
	}
	if (sss_synch_set_N_id_2(&sss, N_id_2)) {
		fprintf(stderr, "Error initializing N_id_2\n");
		exit(-1);
	}

	frame_cnt = 0;
	while (frame_length == filesource_read(&fsrc, input, frame_length)
			&& (frame_cnt < nof_frames || nof_frames == -1)) {

		peak_pos = pss_synch_find_pss(&pss, input, &peak_value);

		if (peak_value > corr_peak_threshold) {

			sss_synch_m0m1(&sss, &input[SSS_SYMBOL_ST(frame_length, 137)],
					&m0, &m0_value, &m1, &m1_value);
			printf("Found m0=%d, m1=%d (%g, %g)\n", m0, m1, m0_value, m1_value);

			printf("Found peak value %g at pos %d (Subframe %d)\n", peak_value, peak_pos, sss_synch_subframe(m0, m1));

		}

		filesink_write(&fsink, pss.conv_abs, frame_length);
		frame_cnt++;
	}

	pss_synch_free(&pss);

	printf("Done\n");
	exit(0);
}
