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
#include "web_server.h"

String phoneNumber = "";



void setup() {
    Serial.begin(115200);
    setup_communication();

    init_sd_card(); 
    setup_i2s();
    #if USE_WEBSERVER 
        setup_web_server();
    #endif
    pinMode(ROTARY_PIN, INPUT_PULLUP); 
    attachInterrupt(digitalPinToInterrupt(ROTARY_PIN), rotaryInterrupt, RISING);
}

void loop() {
    loop_communication();

    if (pulseCount > 0 && millis() - lastPulseTime > digitTimeout) {
        int digit = pulseCount;
        pulseCount = 0;

        if (digit == 10) {
            digit = 0;
        }

        phoneNumber += String(digit);
        Serial.print("Dialed digit: ");
        Serial.println(digit);

        lastPulseTime = millis();
    }

    if (phoneNumber.length() > 0 && millis() - lastPulseTime > 3000) {
        Serial.print("Complete phone number: ");
        Serial.println(phoneNumber);

        // Load mappings from mapping.txt
        std::vector<Mapping> mappings = readMappings();
        for(const auto &mapping : mappings) {
            Serial.print("Mapping: ");
            Serial.print(mapping.fileName);
            Serial.print(" -> ");
            Serial.println(mapping.phoneNumber);
            Serial.print("Is solution: ");
            Serial.println(mapping.isSolution ? "Yes" : "No");
        }

        bool matchFound = false;
        if (active) {
            for (const auto &mapping : mappings) {
                if (phoneNumber == mapping.phoneNumber) {
                    Serial.println("Correct phone number dialed! Playing sound...");
                    play_audio(("/" + mapping.fileName).c_str()); 
                    if (mapping.isSolution) {
                        publish_done();
                    } 
                    matchFound = true;
                    break;
                }
            }
        }

        if (!matchFound) {
            Serial.println("Incorrect phone number.");
        }

        phoneNumber = "";
    }
}