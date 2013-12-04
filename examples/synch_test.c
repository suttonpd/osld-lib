#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "filesource.h"
#include "filesink.h"
#include "pss.h"
#include "sss.h"

char *input_file_name;
char *output_file_name="abs_corr.txt";
int nof_frames=-1, frame_length=1920;
float corr_peak_threshold=10000;

void usage(char *prog) {
	printf("Usage: %s [onlt] -i input_file\n", prog);
	printf("\t-o output_file\n");
	printf("\t-l frame_length\n");
	printf("\t-n number of frames\n");
	printf("\t-t correlation threshold\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "ionlt")) != -1) {
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
	pss_synch_t pss[3]; // One for each N_id_2
	sss_synch_t sss[3]; // One for each N_id_2
	int peak_pos;
	float peak_value;
	int frame_cnt;
	cf_t *input;
	int m0, m1;
	float m0_value, m1_value;
	int N_id_2;

	if (argc < 3) {
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

	/* We have 2 options here:
	 * a) We create 3 pss objects, each initialized with a different N_id_2
	 * b) We create 1 pss object which scans for each N_id_2 one after another.
	 * a) requries more memory but has less latency and is paralellizable.
	 */
	for (N_id_2=0;N_id_2<3;N_id_2++) {
		if (pss_synch_init(&pss[N_id_2], frame_length)) {
			fprintf(stderr, "Error initializing PSS object\n");
			exit(-1);
		}
		if (pss_synch_set_N_id_2(&pss[N_id_2], N_id_2)) {
			fprintf(stderr, "Error initializing N_id_2\n");
			exit(-1);
		}
		if (sss_synch_init(&sss[N_id_2])) {
			fprintf(stderr, "Error initializing SSS object\n");
			exit(-1);
		}
		if (sss_synch_set_N_id_2(&sss[N_id_2], N_id_2)) {
			fprintf(stderr, "Error initializing N_id_2\n");
			exit(-1);
		}
	}

	printf("\n\tFr.Cnt\tN_id_2\tN_id_1\tSubf\tPSS Peak\tIdx\tm0\tm1\n");
	printf("\t==================================================================\n");

	/* read all file or nof_frames */
	frame_cnt = 0;
	while (frame_length == filesource_read(&fsrc, input, frame_length)
			&& (frame_cnt < nof_frames || nof_frames == -1)) {

		for (N_id_2=0;N_id_2<3;N_id_2++) {
			peak_pos = pss_synch_find_pss(&pss[N_id_2], input, &peak_value);

			/* We save the abs correlation for the N_id_2=0 only */
			if (N_id_2 == 0) {
				filesink_write(&fsink, pss[N_id_2].conv_abs, frame_length);
			}

			/* If peak detected */
			if (peak_value > corr_peak_threshold) {

				sss_synch_m0m1(&sss[N_id_2], &input[SSS_SYMBOL_ST(frame_length, 137)],
						&m0, &m0_value, &m1, &m1_value);

				printf("\t%d\t%d\t%d\t%d\t%g\t\t%d\t%d\t%d\n",
						frame_cnt,N_id_2, sss_synch_N_id_1(&sss[N_id_2], m0, m1), sss_synch_subframe(m0, m1), peak_value, peak_pos, m0, m1);

			}
		}

		frame_cnt++;
	}

	for (N_id_2=0;N_id_2<3;N_id_2++) {
		pss_synch_free(&pss[N_id_2]);
		sss_synch_free(&sss[N_id_2]);
	}

	printf("Done\n");
	exit(0);
}
