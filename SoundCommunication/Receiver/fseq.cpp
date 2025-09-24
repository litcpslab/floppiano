#include "fseq.h"

static const FrequencySymbol* symbol_sequence = nullptr;
static uint8_t symbol_length = 0;

static uint8_t match_index = 0;
static uint8_t current_freq_duration = 0;

// Callback function to be triggered when sequence is complete
static FseqCallback solved_callback = nullptr;

/* 
##############################################################################
 * Init_fseq
 * Initializes the frequency sequence detection.
 *
 * @param sequence   Pointer to an array of FrequencySymbol structs
 * @param length     Number of symbols in the sequence
 * @param callback   Function to call when the entire sequence has been matched
##############################################################################
*/
void Init_fseq(const FrequencySymbol* sequence, uint8_t length, FseqCallback callback) {
  symbol_sequence = sequence;
  symbol_length = length;
  solved_callback = callback;

  digitalWrite(LED_PIN, LOW);
  reset_fseq();
}

/* 
##############################################################################
 * process_fseq
 * Processes the currently detected frequency and compares it to the 
 * expected symbol in the sequence. If all symbols have been matched 
 * in order and with correct timing, the configured callback function 
 * is triggered.
 *
 * @param detected_freq The frequency value detected in the current time slice
 ##############################################################################
*/
void process_fseq(float detected_freq) {
  //check for a valid seuqence and whether the puzzle is already solved
  if (!symbol_sequence || match_index >= symbol_length) return;

  const FrequencySymbol& target = symbol_sequence[match_index];

  if (fabs(detected_freq - (float)target.freq) <= FREQ_MATCH_TOLERANCE) {
    current_freq_duration++;

    if (current_freq_duration >= target.duration) {
      // Current sequence has been matched -> proceed to next symbol
      match_index++;
      current_freq_duration = 0;

      if (match_index >= symbol_length) {
        #if DEBUG
          Serial.println("Sequence ->> Puzzle Solved!");
        #endif
        // Trigger external callback
        if (solved_callback && is_initialized) solved_callback();
      } else {
        #if DEBUG
          Serial.print("Sequence ->> Symbol: ");
          Serial.println(match_index);
        #endif
      }
    }
  } else if (match_index > 0) {
    // Wrong frequency during an active symbol
    current_freq_duration++;
    if (current_freq_duration > FREQ_DURATION_RESET) {
      reset_fseq();  // incorrect input -> reset state
    }
  }
}

/* 
##############################################################################
 * Resets the state to the beginning of the sequence.
##############################################################################
*/
void reset_fseq() {
  match_index = 0;
  current_freq_duration = 0;
  #if DEBUG
    Serial.println("Sequence ->> Reset");
  #endif
}