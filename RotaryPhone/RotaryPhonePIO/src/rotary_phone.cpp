#include "rotary_phone.h"

// Define extern variables
volatile int pulseCount = 0;
volatile bool digitComplete = false;
volatile bool cancelPlayback = false;
unsigned long lastPulseTime = 0;

void IRAM_ATTR rotaryInterrupt() {
    unsigned long currentTime = millis();
    if (currentTime - lastPulseTime > 100) {
        pulseCount++;
        lastPulseTime = currentTime;
        cancelPlayback = true;
        #if DEBUG
            Serial.print("[Interrupt] Pulse count: ");
            Serial.println(pulseCount);
        #endif
    }
}

void init_sd_card() {
    if (!SD.begin(SD_CS_PIN, SPI, 10000000)) {
        #if DEBUG
            Serial.println("Failed to initialize SD card");
        #endif
        return;
    }
    #if DEBUG
        Serial.println("SD card initialized successfully");
        Serial.println("Listing files:");
        File root = SD.open("/");
        if (!root) {
            Serial.println("Failed to open root directory");
            return;
        }
        File file = root.openNextFile();
        while (file) {
            Serial.print(" - ");
            Serial.println(file.name());
            file = root.openNextFile();
        }
    #endif
}

void setup_i2s() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S, 
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DATA_IO,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);
}

void play_audio(const char* file_path) {
    cancelPlayback = false; 

    File audioFile = SD.open(file_path);
    if (!audioFile) {
        Serial.printf("Failed to open file: %s\n", file_path);
        return;
    }

    Serial.printf("File opened successfully, size: %d bytes\n", audioFile.size());

    // Read WAV header
    WAVHeader header;
    size_t headerRead = audioFile.read((uint8_t*)&header, sizeof(header));

    if (headerRead != sizeof(header)) {
        Serial.printf("Failed to read WAV header, got %d bytes\n", headerRead);
        audioFile.close();
        return;
    }

    // Validate WAV file
    if (strncmp(header.riff, "RIFF", 4) != 0 || strncmp(header.wave, "WAVE", 4) != 0) {
        Serial.println("Invalid WAV file format");
        audioFile.close();
        return;
    }

    Serial.printf("WAV Info:\n");
    Serial.printf("  Sample Rate: %d Hz\n", header.sampleRate);
    Serial.printf("  Channels: %d\n", header.numChannels);
    Serial.printf("  Bits per Sample: %d\n", header.bitsPerSample);
    Serial.printf("  Data Size: %d bytes\n", header.dataSize);

    // Validate format
    if (header.audioFormat != 1) {
        Serial.println("Only PCM format supported");
        audioFile.close();
        return;
    }

    // Update I2S sample rate if needed
    if (header.sampleRate != SAMPLE_RATE) {
        Serial.printf("Adjusting sample rate to %d Hz\n", header.sampleRate);
        i2s_set_sample_rates(I2S_NUM, header.sampleRate);
    }

    Serial.println("Starting playback...");

    // Volume factor (0.0 = mute, 1.0 = full volume)
    float volume = 0.5; // Adjust this value to set the desired volume

    // Play audio data
    uint8_t buffer[1024];
    size_t bytesRead = 0;
    size_t totalBytesRead = 0;

    while (totalBytesRead < header.dataSize && audioFile.available() && !cancelPlayback) {
        bytesRead = audioFile.read(buffer, sizeof(buffer));
        if (bytesRead > 0) {
            // Apply volume scaling
            for (size_t i = 0; i < bytesRead; i += 2) {
                int16_t sample = buffer[i] | (buffer[i + 1] << 8); // Combine two bytes into a 16-bit sample
                sample = (int16_t)(sample * volume); // Scale the sample by the volume factor
                buffer[i] = sample & 0xFF;          // Extract the lower byte
                buffer[i + 1] = (sample >> 8) & 0xFF; // Extract the upper byte
            }

            size_t bytesWritten;
            esp_err_t result = i2s_write(I2S_NUM, buffer, bytesRead, &bytesWritten, pdMS_TO_TICKS(1000));

            if (result != ESP_OK) {
                Serial.printf("I2S write error: %d\n", result);
                break;
            }

            totalBytesRead += bytesRead;

            // Print progress every 10%
            if (header.dataSize > 0 && totalBytesRead % (header.dataSize / 10) < sizeof(buffer)) {
                float progress = (float)totalBytesRead / header.dataSize * 100;
                Serial.printf("Progress: %.1f%%\n", progress);
            }
        }
    }

    audioFile.close();

    if (cancelPlayback) {
        Serial.println("Playback canceled by interrupt.");
    } else {
        Serial.println("Playback finished!");
    }
}