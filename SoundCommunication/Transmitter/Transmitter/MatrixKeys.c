#include "MatrixKeys.h"

bool input_reg[MATRIX_SIZE][MATRIX_SIZE] = {false};
bool InitDone = false;


/* 
##############################################################################
 * Init Key-Matrix
##############################################################################
*/
void MatrixKeys_Init(Matrix *m){
  InitDone = true;

  for(uint8_t i = 0; i < MATRIX_SIZE; i++){
    bool Init_Input = true, Init_Output = true;

    // init input pins
    if(m->IO[i].InputPin != 255){
      pinMode(m->IO[i].InputPin, INPUT);
    } else{
      Init_Input = false;
    }

    // init output pins
    if(m->IO[i].OutputPin != 255){
      pinMode(m->IO[i].OutputPin, OUTPUT);
      digitalWrite(m->IO[i].OutputPin, HIGH);
    } else{
      Init_Output = false;
    }
    InitDone = InitDone & Init_Input & Init_Output;

    // init input array
    for(uint8_t j = 0; j < MATRIX_SIZE; j++){
      m->input[i][j] = false;
    }
  }
}

/* 
##############################################################################
 * scan Key-Matrix
##############################################################################
*/
void MatrixKeys_Scan(Matrix *m){
  if(InitDone == true){
    for(uint8_t i = 0; i < MATRIX_SIZE; i++){
      // set output line
      digitalWrite(m->IO[i].OutputPin, LOW);
      delay(1);
      // read button inputs -> detect falling edge OR signal state
      for(uint8_t j = 0; j < MATRIX_SIZE; j++){
        // detect falling edge
        const bool readPin = digitalRead(m->IO[j].InputPin);

        // detect ssignal state
        //m->input[i][j] = !readPin;
        m->input[i][j] = input_reg[i][j] & !readPin;
        input_reg[i][j] = readPin;
      }
      
      // reset output line
      digitalWrite(m->IO[i].OutputPin, HIGH);
    }
  }
}

/* 
##############################################################################
 * print Key-Matrix
##############################################################################
*/
void MatrixKeys_Print(Matrix *m, char *Buffer){
  uint8_t offset = 0;
  for (uint8_t i = 0; i < MATRIX_SIZE; i++){
    for (uint8_t j = 0; j < MATRIX_SIZE; j++){
      offset += sprintf(&Buffer[offset], "%d ", m->input[i][j]);
    }
    offset += sprintf(&Buffer[offset], "\n");
  }
}