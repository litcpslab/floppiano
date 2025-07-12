#ifndef COMPLEX_H
#define COMPLEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "conv.h"
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct complex {
    float real;
    float imag;
} complex;

/* compute complex abs() */
float abs_cmplx(complex *x);

/* fill up array with val */
void fill_cmplx(complex val, complex* out, uint16_t size);

/* find index of the max absolute value */
uint16_t find_max_index(complex *x, uint16_t size);

/* out = x * y (inline safe) */ 
void mult_cmplx(complex *x, complex *y, complex *out);

/* out[] = x[] * y[] (inline safe) */
/* (x_RE + jx_IM) * (y_RE + jy_IM) = (x_RE * y_RE - x_IM * y_IM) + j(x_RE * x_IM + x_IM * x_RE) */
void mult_cmplx_arr(complex *x, complex *y, complex *out, uint16_t size);

/* base^exponent */
void pow_cmplx(complex* base, complex* exponent, complex* out);

/* compute the convulution of complex numbers */
void conv_cmplx(complex *x, complex *h, complex *y);

/* Init/DeInit convulution of complex numbers */
bool Init_conv_cmplx(uint16_t x_len, uint16_t h_len);
bool DeInit_conv_cmplx();



#ifdef __cplusplus
}
#endif

#endif