#ifndef __CONV_F32_H__
#define __CONV_F32_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* y = sum(h[m] * x[n-m]) */
void conv_f32(float *x, uint16_t x_len,  // x
              float *h, uint16_t h_len,  // h 
              float *y                   // y
              );   

#ifdef __cplusplus
}
#endif

#endif