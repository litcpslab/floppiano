#ifndef FSEQ_H
#define FSEQ_H

#include "Arduino.h"
#include "config.h"
#include "communication_manager.h"

typedef struct {
  float freq;           // target frequency (Hz)
  uint8_t duration;     // symbol duration (in INTERVAL_CTA_MILLIS units -> (250 ms default))
} FrequencySymbol;

//extern bool is_initialized;
typedef void (*FseqCallback)();  // Function pointer type for callback

void Init_fseq(const FrequencySymbol* sequence, uint8_t length, FseqCallback callback);
void process_fseq(float detected_freq);
void reset_fseq();

#endif