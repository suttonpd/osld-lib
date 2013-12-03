
#include <stdio.h>
#include <stdlib.h>

#include "filesource.h"

int filesource_init(filesource_t *q, char *filename, file_data_type_t type) {
	q->f = fopen(filename, "r");
	if (!q->f) {
		perror("fopen");
		return -1;
	}
	q->type = type;
	return 0;
}

void filesource_free(filesource_t *q) {
	if (q->f) {
		fclose(q->f);
	}
}

int filesource_read(filesource_t *q, void *buffer, int nsamples) {
	int i;
	float *fbuf = (float*) buffer;
	_Complex float *cbuf = (_Complex float*) buffer;
	_Complex short *sbuf = (_Complex short*) buffer;

	switch(q->type) {
	case FLOAT:
		for (i=0;i<nsamples;i++) {
			if (EOF == fscanf(q->f,"%g\n",&fbuf[i]))
				break;
		}
		break;
	case COMPLEX_FLOAT:
		for (i=0;i<nsamples;i++) {
			if (EOF == fscanf(q->f,"%g%gi\n",&(__real__ cbuf[i]),&(__imag__ cbuf[i])))
				break;
		}
		break;
	case COMPLEX_SHORT:
		for (i=0;i<nsamples;i++) {
			if (EOF == fscanf(q->f,"%hd%hdi\n",&(__real__ sbuf[i]),&(__imag__ sbuf[i])))
				break;
		}
		break;
	case BINARY:
		i = fread(buffer,1,nsamples,q->f);
		break;
	default:
		i = -1;
		break;
	}
	return i;
}


int filesource_initialize(filesource_hl* h) {
	return filesource_init(&h->obj, h->init.file_name, h->init.data_type);
}

int filesource_work(filesource_hl* h) {
	*h->out_len = filesource_read(&h->obj, h->output, h->ctrl_in.nsamples);
	if (*h->out_len < 0) {
		return -1;
	}
	return 0;
}

int filesource_stop(filesource_hl* h) {
	filesource_free(&h->obj);
	return 0;
}
