# Transmitter

The transmitter is responsible for generating and sending 9 distinct audio tones in the frequency range of 1 kHz to 2 kHz. These tones are triggered via a button matrix, allowing users to manually select which tone to emit.

- The transmitter is powered by an Arduino Nano.
- Tones are generated using the Arduino `tone()` library and played through a buzzer.
- The tone pitches can be configured in the `pitches.h` file.
- Frequencies must be above 800 Hz, otherwise the buzzer may resonate uncontrollably, impairing reliable detection.
- A KiCad schematic of the transmitter circuit is included in the repository.

# Receiver

The receiver uses a MAX9814 microphone amplifier to detect tones in the frequency range of 1 kHz to 2 kHz, using a Chirp Transform Algorithm (CTA).

- The receiver is based on an ESP32 microcontroller.
- The receiver require the Arduino `WiFi.h` and `PubSubClient.h>` library.
- The CTA algorithm scans from 950 Hz to 2070 Hz in 7 Hz steps to detect incoming tones.
- Configuration is handled via the `config.h` file, which includes:
  - WiFi credentials  
  - MQTT broker parameters  
  - Microphone and CTA settings  
- The `pitches.h` file is reused to match transmitted and received tones.
- The ESP32 and MAX9814 microphone are soldered directly onto the same PCB as the transmitter.
- For accurate detection, a quiet environment is required.
- The detection algorithm is implemented in the `Receiver` source file.

# Enclosure

- The entire circuit is housed in a 3D-printed enclosure.
- A transparent acrylic glass lid is mounted on top of the enclosure, providing both physical protection and visibility of the internal components.