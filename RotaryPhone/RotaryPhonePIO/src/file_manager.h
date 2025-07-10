#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include "config.h"

struct Mapping {
    char fileName[AUDIOFILENAME_MAX];
    char phoneNumber[PHONENUMBER_MAX];
    int rank;
};

extern Mapping mappings[10];
extern int mappingCount;
extern int maxRank;

void refreshMappings();
void setup_web_server();

#endif
