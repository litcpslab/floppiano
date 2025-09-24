#include "task_cta.h"
#include "fseq.h"
#include "pitches.h"
#include "config.h"
#include "communication_manager.h"

// queue for FFT/CTA results
QueueHandle_t FFT_Data_Queue;
TaskHandle_t Task_Main;
TaskHandle_t Task_CTA;

void Task_handle_Data(void *pvParameters);

/*
 * Define your code {frequency | duration (in INTERVAL_CTA_MILLIS steps -> default = 250 ms)}
*/

/*
const FrequencySymbol puzzle_code[] = {
  {NOTE_A6,  2}, // 9
  {NOTE_GS6, 2}, // 8
  {NOTE_G6,  2}, // 7
  {NOTE_FS6, 2}, // 6
  {NOTE_F6,  2}, // 5
  {NOTE_E6,  2}, // 4
  {NOTE_DS6, 2}, // 3
  {NOTE_D6,  2}, // 2
  {NOTE_C6,  2}, // 1
};
*/

const FrequencySymbol puzzle_code[] = {
  {NOTE_DS6, 2}, // 3
  {NOTE_E6,  2}, // 4
  {NOTE_FS6, 2}, // 6
  {NOTE_G6,  2}, // 7
  {NOTE_G6,  2}, // 7
};

/* 
##############################################################################
 * onPuzzleSolved
 * Callback function that is triggered when the entire frequency sequence has 
 * been correctly detected.
##############################################################################
*/
void onPuzzleSolved() {
  #if DEBUG
    Serial.println("!!! CALLBACK: PUZZLE DONE !!!");
    Serial.println("Publishing MQTT message");
  #endif

  digitalWrite(LED_PIN, HIGH);
  client.publish(mqtt_topic_general, "finished");
  reset_fseq();
  
  is_initialized = false;
}


/* SETUP */
void setup() {
  delay(100);
  
  pinMode (LED_PIN, OUTPUT);

  // setup communication start 
  digitalWrite(LED_PIN, HIGH);

  #if DEBUG
    Serial.begin(115200);
    while (!Serial);
    Serial.println();
  #endif

  // AR pin configuration
  if (MAX9814_AR_MODE == 0) {
    pinMode(MICROPHONE_AR_PIN, INPUT);           // Float (high impedance)
  } else {
    pinMode(MICROPHONE_AR_PIN, OUTPUT);
    digitalWrite(MICROPHONE_AR_PIN, MAX9814_AR_MODE == 1 ? HIGH : LOW);
  }

  // Gain pin configuration
  if (MAX9814_GAIN_MODE == 0) {
    pinMode(MICROPHONE_GAIN_PIN, INPUT);         // Float (high impedance)
  } else {
    pinMode(MICROPHONE_GAIN_PIN, OUTPUT);
    digitalWrite(MICROPHONE_GAIN_PIN, MAX9814_GAIN_MODE == 2 ? HIGH : LOW);
  }

  // Setup WIFI connection and connect to MQTT broker
  setup_communication();

  // setup communication done 
  digitalWrite(LED_PIN, LOW);

  FFT_Data_Queue = xQueueCreate(FREQ_BUFFER_SIZE, sizeof(FFT_DataPacket));

  /*
  ######################################################################################
  * Task_handle_Data
  * Task running on core 0. Placeholder for future data WLAN handling tasks.
  ######################################################################################
  */
  xTaskCreatePinnedToCore(Task_handle_Main, "CPU_0", 32768, NULL, 1, &Task_Main, 0);

  /*
  ######################################################################################
  * Task_handle_CTA
  * Task running on core 1 for continuous-time analysis using CTA and ADC input.
  ######################################################################################
  */
  xTaskCreatePinnedToCore(Task_handle_CTA,  "CPU_1", 32768, NULL, 1, &Task_CTA,  1);
}

// not used because FreeRTOS tasks manage the flow
void loop() {
}


/*
######################################################################################
 * Task_handle_Main
 * Receives data packets from the CTA task
 ######################################################################################
*/
void Task_handle_Main(void *pvParameters) {

  Init_fseq(puzzle_code, sizeof(puzzle_code)/sizeof(puzzle_code[0]), onPuzzleSolved);
  FFT_DataPacket packet;

  #if DEBUG
    unsigned long t0 = 0, t_meas = 0;
    float freq_buffer[FREQ_BUFFER_SIZE] = { 0 };
  #endif

  while (1) {
    // Needed for MQTT
    loop_communication();

    // check if new data is available
    if (xQueueReceive(FFT_Data_Queue, &packet, 10) == pdTRUE) {

      process_fseq(packet.val);

      #if DEBUG && PRINT_PARAMETER
        for (int i = FREQ_BUFFER_SIZE - 1; i > 0; i--) {
          freq_buffer[i] = freq_buffer[i - 1];
        }
        freq_buffer[0] = packet.val;

        unsigned long t = micros();
        t_meas = t - t0;
        t0 = t;

        Serial.print("DC: ");
        Serial.println(packet.dc);

        Serial.print("Power: ");
        Serial.println(packet.pw);

        Serial.print("Time (µs): ");
        Serial.println(t_meas);
        Serial.println();

        #if INPUT_MODE == 1
          Serial.print("Freq (Hz) :[ ");
          for (int i = 0; i < FREQ_BUFFER_SIZE; i++) {
            Serial.print(freq_buffer[i]);
            if (i == FREQ_BUFFER_SIZE-1) Serial.println(" ]");
            else                         Serial.print(", ");
          }
        #endif
      #endif
    }
  }
}