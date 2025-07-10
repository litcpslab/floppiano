#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include "config.h"

struct Mapping {
    char fileName[AUDIOFILENAME_MAX];
    int phoneNumber;
    //char phoneNumber[PHONENUMBER_MAX];
    int rank;
};

extern Mapping mappings[10];
extern int mappingCount;
extern int maxRank;
extern char notAvailableFileName[AUDIOFILENAME_MAX];

void refreshMappings();
void setup_web_server();

#endif
