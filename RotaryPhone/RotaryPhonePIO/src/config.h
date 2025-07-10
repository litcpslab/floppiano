#ifndef CONFIG_H
#define CONFIG_H

#include "credentials.h"

/*
 * WIFI Settings (will be used inside communication_manager.h)
*/
static const char* ssid = WIFI_SSID;
static const char* password = WIFI_PASSWORD;

/*
 * Selects MQTT client needs a user authentication :
 * 0 = no user needed
 * 1 = user needed
*/
#define MQTT_NEEDS_USER 0

/*
 * MQTT Settings (will be used inside communication_manager.h)
*/
static const char* mqtt_broker = MQTT_BROKER;
static const int mqtt_port = MQTT_PORT;
static const char* mqtt_username = MQTT_USERNAME;
static const char* mqtt_password = MQTT_PASSWORD;
static const char* mqtt_topic_general = MQTT_TOPIC_GENERAL;
static const char* mqtt_topic_points = MQTT_TOPIC_POINTS;

/*
 * Enables or disables debug output via Serial.
 * Set to 1 to enable Serial debugging.
*/
#define DEBUG (1)

/*
 * Enable or disables the web server for puzzle configuration.
*/
#define USE_WEBSERVER (1)

#define AUDIOFILENAME_MAX 32
#define PHONENUMBER_MAX 16
#define MAPPING_MAX 10

static const char* mapping_file = "/mapping.txt";
static const int rotary_pin = 14;


static const float volume = 0.5;

// I2S configuration
#define SAMPLE_RATE     44100
#define I2S_NUM         I2S_NUM_0
#define I2S_BCK_IO      26   // Bit Clock
#define I2S_WS_IO       12    // Word Select
#define I2S_DATA_IO     32  // Data Output

#define SD_CS_PIN 5 

#endif // CONFIG_H