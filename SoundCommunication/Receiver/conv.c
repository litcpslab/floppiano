#include "conv.h"

void conv_f32(float *x, uint16_t x_len,   // x
              float *h, uint16_t h_len,   // h
              float *y                    // y
             )   
{
    uint16_t y_len = x_len + h_len - 1;
    uint16_t half_M = h_len / 2;

    for (uint16_t n = 0; n < y_len; n++) {
        float sum = 0.0f;

        for (uint16_t m = 0; m < half_M; m++) {
            // // Check if range is valid
            if (n >= m && (n - m) < x_len) {
                sum += h[m] * x[n - m];
            } 
            if (n >= (m + half_M) && (n - m - half_M) < x_len) {
                sum += h[m + half_M] * x[n - m - half_M];
            }
        }
        
        y[n] = sum;
    }
}