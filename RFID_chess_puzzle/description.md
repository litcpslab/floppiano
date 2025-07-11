# RFID Puzzle Game

## Overview
This is an ESP32-based puzzle game that requires players to place the correct RFID tags on two sensors in the proper sequence to unlock a box.
The puzzle is part of an escape room and therefore communicates via mqtt with a basestation to sent a confirmation once the puzzle has been solved. 

## Hardware Components
- **ESP32 microcontroller** - Main control unit
- **2x PN532 RFID sensors** - For reading RFID tags
- **Relay module** - https://www.amazon.de/dp/B07XY2C5M5?ref=nb_sb_ss_w_as-reorder_k0_1_5&amp=&crid=1ODOBIR2U4J1W&amp=&sprefix=relai
 Controls the lock mechanism
- **RFID tags** - https://www.amazon.de/Aufkleber-NFC-Nagelsticker-Elektronische-Wiederbeschreibbar-Kurzbefehle/dp/B0D2H4YHTZ?pd_rd_w=Tscai&content-id=amzn1.sym.7f9b9996-bc03-4d04-b9b7-40b61293137b%3Aamzn1.symc.ca948091-a64d-450e-86d7-c161ca33337b&pf_rd_p=7f9b9996-bc03-4d04-b9b7-40b61293137b&pf_rd_r=KMCED3BC0AH6HBHRB45W&pd_rd_wg=Clty4&pd_rd_r=e100b436-36ed-4438-a5a5-760ac798d686&pd_rd_i=B0D2H4YHTZ&th=1
Programmed with specific values ('1' and '2'). Values are written with an nfc capable smartphone. 
- **12V power supply** - for supplying magnetic lock and microcontroller with power
- **Voltage downconverter** - https://www.amazon.de/dp/B07YWLCTLK?ref=ppx_yo2ov_dt_b_fed_asin_title for converting 12V of supply to 5V for ESP
- **Magnetic lock** https://www.amazon.de/dp/B07SJ6D4RH?ref=ppx_yo2ov_dt_b_fed_asin_title

## Game Mechanics
The puzzle requires players to:
1. Place an RFID tag with value '1' on the first sensor
2. Simultaneously place an RFID tag with value '2' on the second sensor
3. When both correct tags are detected, the relay activates to unlock the mechanism

## Operating Modes

### MQTT Mode (Networked)
- **Activation**: Connect pin D22 to GND during startup
- **Features**:
  - Connects to WiFi network
  - Communicates with MQTT broker
  - Waits for "initialize" message before allowing completion (finished message is only sent if initialize was received)
  - LED of ESP blinks once during startup to indicate MQTT mode

### Standalone Mode
- **Activation**: Connect Pin D22 to 3V3 or Vin during startup
- **Features**:
  - No network setup or communication takes place 
  - Puzzle functions independently
  - LED of ESP blinks three times during startup to indicate standalone mode

## Technical Specifications

### Pin Configuration (see also picture ESP_pins.png)
Wires are connected via the screw terminals to the ESP and the relay
```
PN532_SCK   = 18    (SPI Clock) (White Wires)
PN532_MOSI  = 23    (SPI Master Out) (Yellow Wires)
PN532_MISO  = 19    (SPI Master In) (Green Wires)
PN5321_SS   = 5     (RFID Sensor 1 Chip Select) (Blue Wire)
PN5322_SS   = 17    (RFID Sensor 2 Chip Select) (Blue Wire)
RELAY_PIN   = 13    (Relay Control) (Yellow Wire)
MODE_SELECT = 22    (MQTT/Standalone Selection) (Black Wire)
LED         = 2     (Mode Indicator)
```

### MQTT Communication
- **Broker**: Configurable in `config.h`
- **Topics**: `rfid/puz2/general`
- **Messages**:
  - Receives: `"initialize"` - Enables puzzle completion
  - Sends: `"initialize_ack"` - Confirms initialization
  - Sends: `"finished"` - Puzzle completion notification
  - Sends: `"I am online"` - Connection status

### RFID Tag Requirements
- **Tag Type**: NTAG2xx compatible
- **Data Location**: is found on Page 6, Byte 1
- **Required Values**:
  - Sensor 1: ASCII '1' (0x31)
  - Sensor 2: ASCII '2' (0x32)

### Libraries used
- **SPI.h** - Standard Arduino SPI communication library for PN532 sensors
- **Adafruit_PN532.h** - Adafruit library for PN532 NFC/RFID breakout boards
- **WiFi.h** - ESP32 WiFi library for network connectivity
- **PubSubClient.h** - MQTT client library for broker communication
- **Arduino.h** - Standard Arduino core library

## Setup Instructions

### Hardware Setup 
1. Connect PN532 sensors via SPI according to pin configuration + to 3V3 (Red Wires of sensors) and GND (Black wires of sensor)
2. Connect Downcoverter to supply and ESP (In+ and - to + and - of supply, Out+ to Vin of ESP and Out- to GND)
3. Connect relay (Pin D13 to In, Vcc- to GND and Vcc+ to Vin, +Port of power supply to NO of relay)
4. Connect lock (positive(red) to COM port of relais and - port of supply to negative(black) wire of lock)
5. For MQTT mode: Connect wire from D22 to GND
5. For standalone mode: Connect D22 to 3V3 or Vin

### Software Configuration
1. Update WiFi credentials in `config.h`
2. Configure MQTT broker settings in `config.h`
3. Upload code to ESP32
4. Monitor serial output for status messages

### RFID Tag Programming
App used for programming: NFC.cool
Program RFID tags with the required values:
- Tag 1: Write '1' to page 6, byte 1
- Tag 2: Write '2' to page 6, byte 1

## Usage Flow
Note: the relay closes automatically 10 seconds after the lock has been opened to prevent it from getting too hot.
If you want to open the lock again. you have to remove the figures and place them again on the correct position.
### MQTT Mode
1. Power on ESP32 with D22 connected to 3V3
2. Device connects to WiFi and MQTT broker
3. Send "initialize" message to enable puzzle
4. Place correct RFID tags on both sensors
5. Relay activates and "finished" message is sent
6. Box can now be opened 

### Standalone Mode
1. Power on ESP32 with D22 unconnected
2. Device operates without network connectivity
3. Place correct RFID tags on both sensors
4. Relay activates immediately

## Debug Information
Messages are sent to serial monitor with BAUD of 115200
if hardware needs to be changed. Box can be opened by unscrewing the screws on the bottom of the box
The solution of the puzzle is: Knight to F6 and Rook to B4
