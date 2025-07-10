#include <SPI.h>
#include <Adafruit_PN532.h>
#include "config.h"
#include "communication_manager.h"

#define PN532_SCK   18
#define PN532_MOSI  23
#define PN532_MISO  19
#define PN5321_SS   5   // RFID Sensor 1
#define PN5322_SS   17  // RFDI Sensor 2
#define RELAY_PIN   13
#define MODE_SELECT_PIN 22 // Pin to select mqtt (LOW) or no mqtt (HIGH)
#define LED 2

Adafruit_PN532 nfc1(PN5321_SS);
Adafruit_PN532 nfc2(PN5322_SS);

unsigned long lastCheck = 0;
const unsigned long checkInterval = 200; // check 5 times per second
bool mqtt_enabled = true; // Set to false if MQTT is not used

void setup() {
    Serial.begin(115200);

    pinMode(MODE_SELECT_PIN, INPUT_PULLDOWN);
    if (digitalRead(MODE_SELECT_PIN) == HIGH) {
        mqtt_enabled = false; // If MODE_SELECT_PIN is HIGH, disable MQTT
    }

    if (mqtt_enabled) {
        setup_communication();
    }

    pinMode(RELAY_PIN, OUTPUT);

    Serial.println("Initializing SPI bus...");
    SPI.begin(PN532_SCK, PN532_MISO, PN532_MOSI);

    Serial.println("Initializing PN532 #1...");
    nfc1.begin();
    if (!nfc1.getFirmwareVersion()) {
        Serial.println("PN532 #1 not found");
        while (1);
    }
    nfc1.SAMConfig();

    Serial.println("Initializing PN532 #2...");
    nfc2.begin();
    if (!nfc2.getFirmwareVersion()) {
        Serial.println("PN532 #2 not found");
        while (1);
    }
    nfc2.SAMConfig();
    Serial.println("Both PN532 sensors initialized!");

    pinMode(LED,OUTPUT);
    // blink once mqtt enabled 3 if not
    if (mqtt_enabled) { 
        blink_led(1, 100); 
    } else { 
        blink_led(3, 100); 
    }
}

void blink_led(int times, int delay_time) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED, HIGH);
        delay(delay_time);
        digitalWrite(LED, LOW);
        delay(delay_time);
    }
}

bool read_sensor(Adafruit_PN532 &nfc, uint8_t &sensor_val, const char* sensor_name) {
    uint8_t uid[7];
    uint8_t uidLength;
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50)) {
        uint8_t data[4];
        if (nfc.ntag2xx_ReadPage(6, data)) {
            sensor_val = data[1];
            Serial.print("Sensor ");
            Serial.print(sensor_name);
            Serial.print(" read value: ");
            Serial.println((char)sensor_val);
            return true;
        }
    }
    return false;
}

void loop() {
    if (mqtt_enabled) {
        loop_communication();
    }
    if (millis() - lastCheck >= checkInterval) {
        lastCheck = millis();

        uint8_t sensor1_val = 0;
        uint8_t sensor2_val = 0;

        bool s1_detected = read_sensor(nfc1, sensor1_val, "rfid_1");
        bool s2_detected = read_sensor(nfc2, sensor2_val, "rfid_2");

        static bool relayState = LOW;
        bool lock_open = (s1_detected && sensor1_val == '1') && (s2_detected && sensor2_val == '2');

        if (lock_open != relayState) {
            relayState = lock_open;
            digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
            Serial.printf("Relay %s\n", relayState ? "ON (OPEN)" : "OFF (LOCKED)");
            if (lock_open && can_send_finished_message() && mqtt_enabled) {
                client.publish(mqtt_topic_general, "finished"); // only publish if ack received before 
            }
        }
    }
}