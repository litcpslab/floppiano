#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "driver/i2s.h"
#include "esp_spiffs.h"
#include <SD.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <vector>

#include "config.h"
#include "communication_manager.h"
#include "rotary_phone.h"
#include "file_manager.h"

char phoneNumber[PHONENUMBER_MAX] = "";
bool active = false;

Mapping mappings[MAPPING_MAX];
int mappingCount = 0;
int currentRank = 1;
int maxRank = 0;


void setup() {
    Serial.begin(115200);
    setup_communication();

    init_sd_card(); 
    setup_i2s();
    #if USE_WEBSERVER 
        setup_web_server();
    #endif
    pinMode(rotary_pin, INPUT_PULLUP); 
    attachInterrupt(digitalPinToInterrupt(rotary_pin), rotaryInterrupt, RISING);

    refreshMappings();
    

}


void loop() {
    loop_communication();

    if (pulseCount > 0 && millis() - lastPulseTime > digitTimeout) {
        int digit = pulseCount;
        pulseCount = 0;

        if (digit == 10) {
            digit = 0;
        }

        int len = strlen(phoneNumber);
        phoneNumber[len] = '0' + digit;
        phoneNumber[len + 1] = 0;
        Serial.print("Dialed digit: ");
        Serial.println(digit);

        lastPulseTime = millis();
    }

    if (strlen(phoneNumber) > 0 && millis() - lastPulseTime > 3000) {
        Serial.print("Complete phone number: ");
        Serial.println(phoneNumber);

        bool matchFound = false;
        for (int i=0; i < mappingCount && i < 10; i++) {
            if (!strcmp(phoneNumber, mappings[i].phoneNumber)) {
                if (mappings[i].rank == currentRank && active) {
                    #if DEBUG
                        Serial.print("Correct phone number dialed: ");
                        Serial.println(phoneNumber);
                    #endif
                    char filePath[64];
                    snprintf(filePath, sizeof(filePath), "/%s", mappings[i].fileName);
                    play_audio(filePath); 
                    if (mappings[i].rank == 1) {
                        publish_done();
                        currentRank = maxRank;
                    } else {
                        currentRank--;
                    }
                    matchFound = true;
                    break;
                } else if (mappings[i].rank == 0) {
                    #if DEBUG
                        Serial.println("Easter egg found!");
                    #endif
                    char filePath[64];
                    snprintf(filePath, sizeof(filePath), "/%s", mappings[i].fileName);
                    play_audio(filePath); 
                }
            }
        }

        if (!matchFound) {
            #if DEBUG
                Serial.println("Incorrect phone number.");
            #endif
        }

        phoneNumber[0] = 0;
    }
}