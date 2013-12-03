#include <stdio.h>
#include <stdlib.h>

#include "binsource.h"
#include "utils/bit.h"

int main(int argc, char **argv) {
	binsource_t bs;
	uint8_t* output;

	binsource_init(&bs);
	binsource_seed_time(&bs);

	output = malloc(100);

	if (binsource_generate(&bs,output,100)) {
		printf("Error generating bits\n");
		exit(-1);
	}
	printf("output: ");
	bit_fprint(stdout,output,100);
	printf("Done\n");
	exit(0);
}
