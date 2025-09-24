#ifndef CONFIG_H
#define CONFIG_H

/*
 * WIFI Settings (will be used inside communication_manager.h)
*/
static const char* ssid = "TODO";
static const char* password = "TODO";

/*
 * Selects MQTT client needs a user authentication :
 * 0 = no user needed
 * 1 = user needed
*/
#define MQTT_NEEDS_USER 1

/*
 * MQTT Settings (will be used inside communication_manager.h)
*/
static const char* mqtt_broker = "TODO";
static const int mqtt_port = "TODO";
static const char* mqtt_username = "TODO";
static const char* mqtt_password = "TODO";
static const char* mqtt_topic_general = "TODO";
static const char* mqtt_topic_points = "TODO";

/*
 * GPIO Settings
*/
#define MICROPHONE_ADC_PIN   (34)
#define MICROPHONE_GAIN_PIN  (35)
#define MICROPHONE_AR_PIN    (32)
#define LED_PIN               (2)

/*
 * MAX9814 microphone settings
 * AR (Attack/Release) ratio:
 * 0 = Float (1:4000), 1 = Vdd (1:2000), 2 = GND (1:500)
 * Gain setting:
 * 0 = Float (60 dB), 1 = GND (50 dB), 2 = Vdd (40 dB)
*/

#define MAX9814_AR_MODE (0)
#define MAX9814_GAIN_MODE (0)

/*
 * Allowed frequency deviation (Hz)
*/
#define FREQ_MATCH_TOLERANCE (2.5f * (float)df * (float)fs)

/*
 * Max duration of incorrect symbol (in INTERVAL_CTA_MILLIS steps -> default = 250 ms)
*/
#define FREQ_DURATION_RESET (2)  

/*
 * Enables or disables debug output via Serial.
 * Set to 1 to enable Serial debugging.
*/
#define DEBUG (1)

/*
 * Enables printing of h1, m1, m2 parameters used in CTA.
 * Set to 1 to enable detailed output.
*/
#define PRINT_PARAMETER (0) 

/*
 * Selects the FFT operation mode:
 * 0 = full FFT spectrum,
 * 1 = partial FFT (950 Hz to 2070 Hz, df Hz resolution).
*/
#define FFT_MODE (1)   

/*
 * Selects input mode:
 * 0 = use generated test signal (two sine waves),
 * 1 = use real-time ADC input.
*/            
#define INPUT_MODE (1)  
                       
/*
 * Power threshold above which the CTA analysis is triggered.
*/
#define THRESHOLD (9000)

/*
 * Interval (in milliseconds) between consecutive CTA evaluations (min 230 ms!).
*/
#define INTERVAL_CTA_MILLIS (250);

/*
 * Number of max qued frequency values stored in the buffer.
*/
#define FREQ_BUFFER_SIZE (10)

/* 
 * CTA Settings
*/
#define fs (4000)
#define dt (1.0/fs)
#define M (160)
#define N (128)

#if FFT_MODE
  const float f0 = 950;
  const float df = 7.0 / fs;
  const float w0 = 2.0 * PI * f0 / fs;
  const float dw = 2.0 * PI * df;
#else
  const float f0 = 0;
  const float df = (1 - 1 / (float)M) / (float)M;
  const float w0 = 2.0 * PI * f0 / fs;
  const float dw = 2.0 * PI * df;
#endif

/*
 * Test Signal Configuration
*/
const float Amplitude = pow(2, 8) - 1;
const float f1 = 1500;
const float f2 = 1800;

#endif