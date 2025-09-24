#ifndef TASK_CTA_H
#define TASK_CTA_H

#include "Arduino.h"
#include "complex.h"
#include "CTA.h"
#include "config.h"

// Timer base frequency in Hz
#define F_TIMER (1e6)

typedef struct {
  float val;
  float dc;
  float pw;
} FFT_DataPacket;

extern QueueHandle_t FFT_Data_Queue;

/* CTA main task */
void Task_handle_CTA(void *pvParameters);

/* sampling interrupt ISR-Funktion */
void ARDUINO_ISR_ATTR sampleData();

/* print h1, m1, m2 */
void print_parameter();

/* print y */
void print_result(complex *y);

/* print FFT */
void print_result_abs(complex *y);

#endif
