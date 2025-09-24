#include "complex.h"

// memory pointer for complex conv
static float* h1_re = NULL;
static float* h1_im = NULL;
static float* x_re  = NULL;
static float* x_im  = NULL;
static float* ReRe  = NULL;
static float* ImRe  = NULL;
static float* ReIm  = NULL;
static float* ImIm  = NULL;
static uint16_t cmplx_conv_size = 0;

/* compute complex abs() */
float abs_cmplx(complex *x){
    return sqrt(x->real * x->real + x->imag * x->imag);
}

/* fill up array with val */
void fill_cmplx(complex val, complex* out, uint16_t size){
  for(uint16_t i = 0; i < size; i++){
    out[i].real = val.real;
    out[i].imag = val.imag;
  }
}

/* find index of the max absolute value */
uint16_t find_max_index(complex *x, uint16_t size){
  float max_val = 0.0;
  uint16_t index = 0;

  for(int i = 0; i < size; i++) {
    float abs_value = abs_cmplx(&x[i]);
    if(max_val <= abs_value){
      max_val = abs_value;
      index = i;
    }
  }

  return index;
}

/* out = x * y (inline safe) */ 
void mult_cmplx_arr(complex* x, complex* y, complex* out, uint16_t size){
  for(int i = 0; i < size; i++){
    mult_cmplx(&x[i], &y[i], &out[i]);
  }
}

/* out[] = x[] * y[] (inline safe) */
/* (x_RE + jx_IM) * (y_RE + jy_IM) = (x_RE * y_RE - x_IM * y_IM) + j(x_RE * x_IM + x_IM * x_RE) */
void mult_cmplx(complex* x, complex* y, complex* out){
    complex tmp;
    tmp.real = x->real * y->real - x->imag * y->imag;
    tmp.imag = x->real * y->imag + x->imag * y->real;

    out->real = tmp.real;
    out->imag = tmp.imag;
}

/* ln(x) = ln|x| + i * arg(x) */
void ln_cmplx(complex* x, complex* out) {
    float magnitude = abs_cmplx(x); // sqrt(x->real*x->real + x->imag*x->imag);
    float angle = atan2(x->imag, x->real);

    out->real = log(magnitude);
    out->imag = angle;
}

/* exp(z) = e^re * (cos im + i sin im) */
void exp_cmplx(complex* x, complex* out) {
    float exp_re = exp(x->real);
    out->real = exp_re * cos(x->imag);
    out->imag = exp_re * sin(x->imag);
}

/* base^exponent = exp(w * ln(z)) */
void pow_cmplx(complex* base, complex* exponent, complex* out){
    complex tmp;
    ln_cmplx(base, &tmp);
    mult_cmplx(&tmp, exponent, &tmp);
    exp_cmplx(&tmp, out);
}

/* Init complex conv, allocate memory for results */
bool Init_conv_cmplx(uint16_t x_len, uint16_t h_len){
  cmplx_conv_size = x_len + h_len - 1;
  // allocate memory
  h1_re = malloc(sizeof(float)   * cmplx_conv_size);
  h1_im = malloc(sizeof(float)   * cmplx_conv_size);
  x_re  = malloc(sizeof(float)   * cmplx_conv_size);
  x_im  = malloc(sizeof(float)   * cmplx_conv_size);
  ReRe  = malloc(2*sizeof(float) * cmplx_conv_size);
  ImRe  = malloc(2*sizeof(float) * cmplx_conv_size);
  ReIm  = malloc(2*sizeof(float) * cmplx_conv_size);
  ImIm  = malloc(2*sizeof(float) * cmplx_conv_size);

    // check memory
  if (!h1_re || !h1_im || !x_re || !x_im || !ReRe || !ImRe || !ReIm || !ImIm) {
      return false;
  }

  return true;
}

/* De Init complex conv, allocate memory for results */
bool DeInit_conv_cmplx(){
  // check memory
  if (!h1_re || !h1_im || !x_re || !x_im || !ReRe || !ImRe || !ReIm || !ImIm) {
      return false;
  }

  cmplx_conv_size = 0;
  free(h1_re); free(h1_im);
  free(x_re);  free(x_im);
  free(ReRe);  free(ImRe); free(ReIm); free(ImIm);

  return true;
}

/* compute the convulution of complex numbers */
void conv_cmplx(complex *x, complex *h, complex *y){
  for (int i = 0; i < cmplx_conv_size; i++) {
    x_re[i]  = x[i].real;
    x_im[i]  = x[i].imag;
    h1_re[i] = h[i].real;
    h1_im[i] = h[i].imag;
  }

  // convolution
  // (x_RE + jx_IM) x (y_RE + jy_IM) = (x_RE x y_RE - x_IM x y_IM) + j(x_RE x x_IM + x_IM x x_RE) */
  conv_f32(x_re, cmplx_conv_size, h1_re, cmplx_conv_size, ReRe);
  conv_f32(x_im, cmplx_conv_size, h1_re, cmplx_conv_size, ImRe);
  conv_f32(x_re, cmplx_conv_size, h1_im, cmplx_conv_size, ReIm);
  conv_f32(x_im, cmplx_conv_size, h1_im, cmplx_conv_size, ImIm);

  // compute result
  for (int i = 0; i < cmplx_conv_size; i++) {
    y[i].real = ReRe[i] - ImIm[i];
    y[i].imag = ReIm[i] + ImRe[i];
  }
}