#include "task_cta.h"

complex *h1 = NULL;
complex *m1 = NULL;
complex *m2 = NULL;

static complex x[N];
static complex y[M];

static volatile uint16_t sampleIndex = 0;
static volatile int16_t adc_buffer[2 * N] = { 0 };

// Buffer state machine to track sampling progress
enum BufferState {
  BUFFER_WAIT_HALF_FULL,
  BUFFER_HALF_FULL,
  BUFFER_WAIT_FULL,
  BUFFER_FULL
};

static volatile BufferState bufferState = BUFFER_WAIT_HALF_FULL;

// Timer and synchronization primitives
static hw_timer_t *timer = NULL;
static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
static volatile SemaphoreHandle_t timerSemaphore;


/* 
############################################################################
 * Task_handle_CTA
 * Main task to perform Chirp Transform Algorithm (CTA).
 * Handles memory allocation, ADC sampling via timer interrupts,
 * signal processing using CTA, and result posting via queue.
 ############################################################################
*/

void Task_handle_CTA(void *pvParameters) {
  // allocate memory
  h1 = (complex *)malloc(sizeof(complex) * (N + M - 1));
  m1 = (complex *)malloc(sizeof(complex) * (N + M - 1));
  m2 = (complex *)malloc(sizeof(complex) * (N + M - 1));

  // Abort if memory allocation failed
  if (!h1 || !m1 || !m2) {
    return;
  }

  // Initialize CTA parameters
  Init_CTA(w0, dw);

  // Optionally print initial parameters for debugging
  #if DEBUG && PRINT_PARAMETER
    Serial.print("Amplitude: ");
    Serial.println(Amplitude);
    Serial.print("w0: ");
    Serial.println(w0, 5);
    Serial.print("dw: ");
    Serial.println(dw, 5);
    Serial.println();
    print_parameter();
    Serial.println();
  #endif
  
  // If in test signal mode: generate signal, process it once, and stop
  #if INPUT_MODE == 0
    for (uint16_t i = 0; i < N; i++) {
      float argA = 2.0 * PI * f1 * (float)i * dt;
      float sinA = Amplitude * sin(argA);

      float argB = 2.0 * PI * f2 * (float)i * dt;
      float sinB = Amplitude * sin(argB);

      adc_buffer[i] = (int16_t)(sinA + sinB);
    }

    float dc = 0.0, pw = 0.0;

    convertSignal_getMetrics((int16_t*)&adc_buffer[0], x, &dc, &pw);
    CTA(x, w0, dw, y);
    
    #if DEBUG
      delay(1000);
      print_result_abs(y);
    #endif  

    while(1){ } // stay here forever (to send data only once)
  #endif

  FFT_DataPacket packet;
  bufferState = BUFFER_WAIT_HALF_FULL;

  analogReadResolution(12);

  // Create semaphore to be triggered by ISR
  timerSemaphore = xSemaphoreCreateBinary();

  // Setup hardware timer and ISR
  timer = timerBegin(F_TIMER);
  timerAttachInterrupt(timer, &sampleData);
  timerAlarm(timer, F_TIMER / fs, true, 0);

  unsigned long t0 = 0;
  const unsigned long interval = INTERVAL_CTA_MILLIS;

  // Main loop: wait for ADC buffer, process CTA, and enqueue results
  while (1) {
    unsigned long t = millis();

    if (t - t0 >= interval) {
      if (xSemaphoreTake(timerSemaphore, portMAX_DELAY)) {
        t0 = t;

        int16_t *data_ptr = nullptr;

        // Select buffer half depending on sampling progress
        portENTER_CRITICAL(&timerMux);
        if (bufferState == BUFFER_HALF_FULL) {
          data_ptr = (int16_t*)&adc_buffer[0];
          bufferState = BUFFER_WAIT_FULL;
        } else if (bufferState == BUFFER_FULL) {
          data_ptr = (int16_t*)&adc_buffer[N];
          bufferState = BUFFER_WAIT_HALF_FULL;
        }
        portEXIT_CRITICAL(&timerMux);

        packet.dc = 0.0;
        packet.pw = 0.0;

        // Convert raw ADC values and calculate power/DC level
        if (data_ptr != NULL) {
          convertSignal_getMetrics(data_ptr, x, &packet.dc, &packet.pw);
        }

        // Only process CTA if power exceeds threshold
        if (packet.pw >= THRESHOLD) {
          CTA(x, w0, dw, y);
          packet.val = f0 + (float)find_max_index(y, N) * df * fs;

          // Try to enqueue result without blocking
          if (xQueueSend(FFT_Data_Queue, &packet, 0) != pdTRUE) {
            #if DEBUG
              Serial.println("Failed to post the data");
            #endif   
          }
        } else {
          // No signal: mark result as invalid
          packet.val = NAN;
        }
      }
    }
  }
}

/*
############################################################################
 * sample interrupt
 * Interrupt service routine (ISR) called by hardware timer.
 * Samples the microphone ADC and fills the adc_buffer.
 * Signals the main task once half or full buffer is filled.
 ############################################################################
*/

void ARDUINO_ISR_ATTR sampleData() {
  // check if buffer is full
  if ((sampleIndex < N && bufferState != BUFFER_WAIT_HALF_FULL) || (sampleIndex >= N && bufferState != BUFFER_WAIT_FULL)) return;

  int16_t val = (int16_t)analogRead(MICROPHONE_ADC_PIN);

  portENTER_CRITICAL_ISR(&timerMux);

  adc_buffer[sampleIndex] = val;
  sampleIndex++;

  // Notify when half/full buffer is filled
  if (sampleIndex == N) {
    bufferState = BUFFER_HALF_FULL;
    xSemaphoreGiveFromISR(timerSemaphore, NULL);
  } else if (sampleIndex >= 2 * N) {
    bufferState = BUFFER_FULL;
    sampleIndex = 0;
    xSemaphoreGiveFromISR(timerSemaphore, NULL);
  }

  portEXIT_CRITICAL_ISR(&timerMux);
}


/*
############################################################################
 * print functions
 ############################################################################
*/

/* print h1, m1, m2 */
void print_parameter() {
  Serial.println("h1:");
  for (int i = 0; i < N + M - 1; i++) {
    Serial.print(h1[i].real, 5);
    Serial.print("+");
    Serial.print(h1[i].imag, 5);
    Serial.print("i");
    Serial.println();
  }

  Serial.println();
  Serial.println("m1:");
  for (int i = 0; i < N + M - 1; i++) {
    Serial.print(m1[i].real, 5);
    Serial.print("+");
    Serial.print(m1[i].imag, 5);
    Serial.print("i");
    Serial.println();
  }

  Serial.println();
  Serial.println("m2:");
  for (int i = 0; i < N + M - 1; i++) {
    Serial.print(m2[i].real, 5);
    Serial.print("+");
    Serial.print(m2[i].imag, 5);
    Serial.print("i");
    Serial.println();
  }
}

/* print y */
void print_result(complex *y) {
  Serial.println("y:");
  for (int i = 0; i < M; i++) {
    Serial.print(y[i].real, 5);
    Serial.print("+");
    Serial.print(y[i].imag, 5);
    Serial.print("i");
    Serial.println();
  }
}

/* print absolute values of y */
void print_result_abs(complex *y) {
  Serial.println("abs(y)");
  for (int i = 0; i < M; i++) {
    float abs_value = abs_cmplx(&y[i]);
    Serial.print("FFT:");
    Serial.println(abs_value);
  }
  Serial.println();
}

