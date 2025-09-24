#include "CTA.h"

static bool InitDone = false;
static complex* mix_up = NULL;
static complex* conv_res = NULL;

void Init_CTA(const float w0, const float dw){
  complex W = {cos(dw),  -sin(dw)};
  // Generation of h1
  fill_cmplx((complex){0.0, 0.0}, h1, N + M - 1);
  for(uint16_t n = 0 ; n < N + M - 1; n++) {
      const float currIndex =((float)n-(float)N+1.0); 
      const complex z = {-((currIndex)*(currIndex)/2.0), 0.0};
      pow_cmplx(&W, &z, &h1[n]);
  }

  // Generation of m1
  fill_cmplx((complex){0.0, 0.0}, m1, N + M - 1);
  for(uint16_t n = 0; n < N; n++) {
      complex z1 = {0, 0};
      complex exponent = {(n*n)/2.0, 0};
      pow_cmplx(&W, &exponent, &z1);
      
      complex z2 = {cos(w0 * n), -sin(w0 * n)};
      mult_cmplx(&z1, &z2, &m1[n]);
  }

  // Generation of m2
  fill_cmplx((complex){0.0, 0.0}, m2, N + M - 1);
  for(uint16_t n = 0; n < M; n++) {
      const float currIndex = n; 
      const complex z = {(currIndex)*(currIndex)/2.0, 0};
      pow_cmplx(&W, &z, &m2[n]);
  }

  // allocate memory
  mix_up   = malloc(sizeof(complex) * (N + M - 1));
  conv_res = malloc(2*sizeof(complex) * (N + M - 1));

  // check memory
  if (!mix_up || !conv_res) {
    return;
  }

  // Init convulution memory
  if(!Init_conv_cmplx(N + M - 1, N + M - 1)){
    return;
  } 

  InitDone = true;
}

void CTA(complex* x, const float w0, const float dw, complex *y) {
  // check Init
  if (!InitDone) {
    return;
  }

  fill_cmplx((complex){0.0, 0.0}, mix_up, N + M - 1);
  mult_cmplx_arr(x, m1, mix_up, N);

  conv_cmplx(mix_up, h1, conv_res);

  mult_cmplx_arr(&conv_res[N-1], m2, y, M);
}


void convertSignal_getMetrics(int16_t *data_ptr, complex *x, float* offset, float* power){
  // Remove DC offset
  for(uint16_t i = 0; i < N; i++) {
      x[i].real = (float)data_ptr[i];
      x[i].imag = 0.0;  
      *offset += x[i].real;
  }
  *offset /= (float)N;

  // calc power
  for(uint16_t i = 0; i < N; i++) {
    x[i].real -= *offset;
    x[i].imag -= *offset;
    // Calculate power
    float z = abs_cmplx(&x[i]);
    *power += z * z;
  }
  *power /= (float)N;
}
