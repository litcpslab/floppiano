#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include "Arduino.h"
#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>

extern WiFiClient espClient;
extern PubSubClient client;
extern bool initialize_received;

/*
Function for connection to a WIFI network and connect to a MQTT broker

SSID, password, MQTT is defined in config.h
*/
void setup_communication();

/*
Function for allowing the MQTT handler to be executed
*/
void loop_communication();

/*
Function which will be called when a message is received
*/
void mqtt_callback(char *topic, byte *payload, unsigned int length);

bool can_send_finished_message();

#endif