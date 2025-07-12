// Transmitter Main
#include "Arduino.h"
#include "pitches.h"
#include "MatrixKeys.h"

#define LED_PIN (13)
#define MATRIX_KEYS_INPUT_1 (14)
#define MATRIX_KEYS_INPUT_2 (17)
#define MATRIX_KEYS_INPUT_3 (18)

#define MATRIX_KEYS_OUTPUT_1 (16)
#define MATRIX_KEYS_OUTPUT_2 (19)
#define MATRIX_KEYS_OUTPUT_3 (15)

#define PWM_PIN (8)

#define BUFFER_SIZE (255)

char Buffer[BUFFER_SIZE]; 
Matrix ButtonMatrix = Default_MatrixKeys; 

int notes[] = {
  NOTE_C6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6
};

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    // Init button matrix -> set IN/OUT Pins
    ButtonMatrix.IO[0].InputPin  = MATRIX_KEYS_INPUT_1;
    ButtonMatrix.IO[0].OutputPin = MATRIX_KEYS_OUTPUT_1;
    ButtonMatrix.IO[1].InputPin  = MATRIX_KEYS_INPUT_2;
    ButtonMatrix.IO[1].OutputPin = MATRIX_KEYS_OUTPUT_2;
    ButtonMatrix.IO[2].InputPin  = MATRIX_KEYS_INPUT_3;
    ButtonMatrix.IO[2].OutputPin = MATRIX_KEYS_OUTPUT_3;

    MatrixKeys_Init(&ButtonMatrix);
}

void loop() {
  MatrixKeys_Scan(&ButtonMatrix);

  MatrixKeys_Print(&ButtonMatrix, Buffer);

  Serial.println(Buffer);

  // Iterate through the button matrix
  // play the corresponding tone if a button press is detected
  for (uint8_t i = 0; i < MATRIX_SIZE*MATRIX_SIZE; i++){
    if(ButtonMatrix.input[i/MATRIX_SIZE][i%MATRIX_SIZE]){
      tone(PWM_PIN, notes[i], 500);
      break;
    }
  }

  delay(10);
}
