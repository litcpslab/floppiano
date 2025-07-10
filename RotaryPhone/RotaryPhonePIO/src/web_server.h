#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

struct Mapping {
    String fileName;
    String phoneNumber;
    bool isSolution;
};

std::vector<Mapping> readMappings();
void setup_web_server();

#endif
