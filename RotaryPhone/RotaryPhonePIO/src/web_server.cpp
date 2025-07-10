#include <SD.h>
#include "web_server.h"

AsyncWebServer server(80);


std::vector<Mapping> readMappings() {
    std::vector<Mapping> mappings;
    File mappingFile = SD.open("/mapping.txt", FILE_READ);

    if (mappingFile) {
        while (mappingFile.available()) {
            String line = mappingFile.readStringUntil('\n');
            int colonIndex = line.indexOf(':');
            if (colonIndex > 0) {
                Mapping mapping;
                mapping.fileName = line.substring(0, colonIndex);
                mapping.phoneNumber = line.substring(colonIndex + 1);
                if (mapping.phoneNumber.endsWith(":solution")) {
                    mapping.phoneNumber = mapping.phoneNumber.substring(0, mapping.phoneNumber.lastIndexOf(":solution"));
                }
                mappings.push_back(mapping);
            }
        }
        mappingFile.close();
    } else {
        Serial.println("Failed to open mapping.txt");
    }

    return mappings;
}

void setup_web_server() {
    if (!SD.begin()) {
        Serial.println("An error occurred while initializing the SD card");
        return;
    }

    Serial.println("SD card initialized successfully");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Handling root request");

        File indexFile = SD.open("/index.html", FILE_READ);
        if (!indexFile) {
            Serial.println("index.html file not available");
            String fallbackHtml = "<html><body><h1>Default Page</h1><p>The index.html file is missing.</p></body></html>";
            request->send(200, "text/html", fallbackHtml);
            return;
        }

        std::vector<Mapping> mappings = readMappings();

        String fileList = "<table border='1'><tr><th>File Name</th><th>Phone Number</th><th>Solution</th><th>Actions</th></tr>";
        File root = SD.open("/");
        File file = root.openNextFile();

        while (file) {
            if (String(file.name()).endsWith(".wav")) {
                fileList += "<tr>";
                fileList += "<td>" + String(file.name()) + "</td>";
                String phoneNumber = "";
                for (const auto &mapping : mappings) {
                    if (mapping.fileName == String(file.name())) {
                        phoneNumber = mapping.phoneNumber;
                        break;
                    }
                }
                fileList += "<td><input type='text' id='" + String(file.name()) + "_number' value='" + phoneNumber + "' disabled></td>";
                fileList += "<td><input type='radio' name='solution' id='" + String(file.name()) + "_solution'></td>";
                fileList += "<td><button onclick=\"deleteFile('" + String(file.name()) + "')\">Delete</button></td>";
                fileList += "</tr>";
            }
            file = root.openNextFile();
        }
        fileList += "</table>";
        fileList += "<button onclick='enableAllFields()'>Edit All</button>";
        fileList += "<button onclick='saveAllMappings()'>Save All</button>";

        String htmlContent = "";
        while (indexFile.available()) {
            htmlContent += indexFile.readStringUntil('\n');
        }
        indexFile.close();

        htmlContent.replace("%FILELIST%", fileList);

        request->send(200, "text/html", htmlContent);
    });

    server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "File uploaded successfully");
    }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index) {
            Serial.printf("UploadStart: %s\n", filename.c_str());
            request->_tempFile = SD.open("/" + filename, FILE_WRITE);
        }
        if (request->_tempFile) {
            request->_tempFile.write(data, len);
        }
        if (final) {
            Serial.printf("UploadEnd: %s, %u bytes\n", filename.c_str(), index + len);
            if (request->_tempFile) {
                request->_tempFile.close();
            }
        }
    });

    server.on("/save_mapping", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("file", true) && request->hasParam("number", true)) {
            String file = request->getParam("file", true)->value();
            String number = request->getParam("number", true)->value();
            bool isSolution = request->hasParam("solution", true) && request->getParam("solution", true)->value() == "true";

            File mappingFile = SD.open("/mapping.txt", FILE_READ);
            String updatedMappings = "";
            bool mappingExists = false;

            if (mappingFile) {
                while (mappingFile.available()) {
                    String line = mappingFile.readStringUntil('\n');
                    if (line.startsWith(file + ":")) {
                        updatedMappings += file + ":" + number + (isSolution ? ":solution" : "") + "\n";
                        mappingExists = true;
                    } else {
                        // Ensure only one solution mapping exists
                        if (line.endsWith(":solution")) {
                            line = line.substring(0, line.lastIndexOf(":solution"));
                        }
                        updatedMappings += line + "\n";
                    }
                }
                mappingFile.close();
            }

            if (!mappingExists) {
                updatedMappings += file + ":" + number + (isSolution ? ":solution" : "") + "\n";
            }

            mappingFile = SD.open("/mapping.txt", FILE_WRITE);
            if (mappingFile) {
                mappingFile.print(updatedMappings);
                mappingFile.close();
                request->send(200, "text/plain", "Mapping saved successfully");
            } else {
                request->send(500, "text/plain", "Failed to save mapping");
            }
        } else {
            request->send(400, "text/plain", "Invalid parameters");
        }
    });

    server.on("/save_all", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("mappings", true)) {
            String mappings = request->getParam("mappings", true)->value();

            File mappingFile = SD.open("/mapping.txt", FILE_WRITE);
            if (mappingFile) {
                mappingFile.print(mappings);
                mappingFile.close();
                request->send(200, "text/plain", "All mappings saved successfully");
            } else {
                request->send(500, "text/plain", "Failed to save mappings");
            }
        } else {
            request->send(400, "text/plain", "Invalid parameters");
        }
    });

    server.on("/delete_file", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("file", true)) {
            String file = request->getParam("file", true)->value();
            Serial.printf("Deleting file: %s\n", file.c_str());
            if (SD.remove("/" + file)) {
                request->send(200, "text/plain", "File deleted successfully");
            } else {
                request->send(500, "text/plain", "Failed to delete file");
            }
        } else {
            request->send(400, "text/plain", "Invalid parameters");
        }
    });

    server.begin();
    Serial.println("Web server started");
}
