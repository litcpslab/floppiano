#ifndef CTA_H
#define CTA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "complex.h"
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"

extern complex *h1;
extern complex *m1; 
extern complex *m2;

/* Init CTA */
void Init_CTA(const float w0, const float dw);

/* compute CTA */
void CTA(complex* x, const float w0, const float dw, complex *y);

/* convert signal into float values without DC-offset and calculate power */
void convertSignal_getMetrics(int16_t *data_ptr, complex *x, float* offset, float* power);

#ifdef __cplusplus
}
#endif

#endif