#include <complex.h>

typedef struct {
	int size;
	float *cost;
	float *sint;
}nco_t;

void nco_init(nco_t *nco, int size);
void nco_destroy(nco_t *nco);

float nco_sin(nco_t *nco, float phase);
float nco_cos(nco_t *nco, float phase);
void nco_sincos(nco_t *nco, float phase, float *sin, float *cos);
_Complex float nco_cexp(nco_t *nco, float arg);

void nco_sin_f(nco_t *nco, float *x, float freq, int len);
void nco_cos_f(nco_t *nco, float *x, float freq, int len);
void nco_cexp_f(nco_t *nco, _Complex float *x, float freq, int len);
