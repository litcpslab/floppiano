#ifndef __MATRIX_KEYS_H__
#define __MATRIX_KEYS_H__

#ifdef __cplusplus
extern "C" {
#endif
  #include "Arduino.h"

  #define MATRIX_SIZE (3)

  typedef struct {
      uint8_t InputPin;
      uint8_t OutputPin;
  }IO_Pins;

  typedef struct {
      bool input[MATRIX_SIZE][MATRIX_SIZE];
      IO_Pins IO[MATRIX_SIZE];
  }Matrix;

  const Matrix Default_MatrixKeys = {
    {{false, false, false}, {false, false, false}, {false, false, false}},
    {{255, 255},
    {255, 255},
    {255, 255}}  
  };

  void MatrixKeys_Init(Matrix *m);
  void MatrixKeys_Scan(Matrix *m);
  void MatrixKeys_Print(Matrix *m, char *Buffer);

#ifdef __cplusplus
}
#endif

#endif
