#ifndef ROTARY_PHONE_H
#define ROTARY_PHONE_H

#include <SD.h>
#include "driver/i2s.h"
#include "config.h"

struct WAVHeader {
    char riff[4];
    uint32_t fileSize;
    char wave[4];
    char fmt[4];
    uint32_t fmtSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char data[4];
    uint32_t dataSize;
};

const unsigned long digitTimeout = 300;

extern volatile int pulseCount;
extern volatile bool digitComplete;
extern unsigned long lastPulseTime;
extern bool active;

void IRAM_ATTR rotaryInterrupt();
void init_sd_card();
void setup_i2s();
void play_audio(const char* file_path);

#endif  // ROTARY_PHONE_H