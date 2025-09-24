#ifndef CONFIG_H
#define CONFIG_H

/*
 * WIFI Settings (will be used inside communication_manager.h)
*/
static const char* ssid = "EmbeddedSys";
static const char* password = "EmbeddedSys#1234";

/*
 * Selects MQTT client needs a user authentication :
 * 0 = no user needed
 * 1 = user needed
*/
#define MQTT_NEEDS_USER 1

/*
 * MQTT Settings (will be used inside communication_manager.h)
*/
static const char* mqtt_broker = "192.168.0.2";
static const int mqtt_port = 1883;
static const char* mqtt_username = "user";
static const char* mqtt_password = "user";
static const char* mqtt_topic_general = "rfid/puz2/general";
static const char* mqtt_topic_points = "rfid/puz2/points";

/*
 * Enables or disables debug output via Serial.
 * Set to 1 to enable Serial debugging.
*/
#define DEBUG (1)

#endif  // CONFIG_H