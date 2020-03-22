#include <Arduino.h>

#define trigPin 26                                    // Pin 12 trigger output
#define echoPin 27                                    // Pin 2 Echo input


volatile uint32_t echo_start = 0;                         // Records start of echo pulse 
volatile uint32_t echo_end = 0;                           // Records end of echo pulse
volatile uint16_t echo_duration = 0;                      // Duration - difference between end and start
volatile uint16_t trigger_time_count = 0;                  // Count down counter to trigger pulse time
volatile uint8_t  type_point = 0;
volatile bool     Sonar = false;
// --------------------------
// timer_handle_interrupts(int timer) called every 50 uS to schedule trigger pulses.
// Generates a pulse one timer tick long.
// Minimum trigger pulse width for the HC-SR04 is 10 us. This system
// delivers a 50 uS pulse.
// --------------------------

void timer_handle_interrupts () {

if ( type_point == 1 || type_point == 2 || Sonar ){

  static volatile uint8_t state = 0;                 // State machine variable
  
  trigger_time_count--;

      if ( 0 >= trigger_time_count )                 // Count to 200mS
      {                                              // Time out - Initiate trigger pulse
         trigger_time_count = 4000;                  // Reload
         state = 1;                                  // Changing to state 1 initiates a pulse
      }
    
      switch(state)                                  // State machine handles delivery of trigger pulse
      {
        case 0:                                      // Normal state does nothing
            break;
        
        case 1:                                      // Initiate pulse
           digitalWrite(trigPin, HIGH);              // Set the trigger output high
           state = 2;                                // and set state to 2
           break;
        
        case 2:                                      // Complete the pulse
        default:      
           digitalWrite(trigPin, LOW);               // Set the trigger output low
           state = 0;                                // and return state to normal 0
           break;
     }
  }
}
// --------------------------
// echo_interrupt() External interrupt from HC-SR04 echo signal. 
// Called every time the echo signal changes state.
//
// Note: this routine does not handle the case where the timer
//       counter overflows which will result in the occassional error.
// --------------------------
void echo_interrupt()
{  
  switch (digitalRead(echoPin))                     // Test to see if the signal is high or low
  {
    case HIGH:                                      // High so must be the start of the echo pulse
      echo_end = 0;                                 // Clear the end time
      echo_start = micros();                        // Save the start time
      break;
      
    case LOW:                                       // Low so must be the end of hte echo pulse
      echo_end = micros();                          // Save the end time
      echo_duration = echo_end - echo_start;        // Calculate the pulse duration
      break;
  }
}

//------------------------------------------------------------------------------
/// Interrupt handler for timers
//------------------------------------------------------------------------------

 void TC6_Handler () {
    // clear interrupt
    TC2->TC_CHANNEL[0].TC_SR; // TC - timer counter
    // reset the timer
    TC2->TC_CHANNEL[0].TC_CCR |= TC_CCR_SWTRG;

    timer_handle_interrupts();
}



static void _initISR(Tc *tc, uint16_t channel, uint16_t id, uint16_t tclock, uint16_t adjustment, IRQn_Type irqn) {
    pmc_enable_periph_clk(id);
    TC_Configure(tc, channel,
        tclock |                     // timer clock (prescaler)
        TC_CMR_WAVE |                // Waveform mode
        TC_CMR_WAVSEL_UP_RC );       // Counter running up and reset when equals to RC

    // timer clock adjustment
    TC_SetRA(tc, channel, adjustment);

    /* Configure and enable interrupt */
    NVIC_EnableIRQ(irqn);
    // TC_IER_CPAS: RA Compare
    tc->TC_CHANNEL[channel].TC_IER = TC_IER_CPAS;

    // Enables the timer clock and performs a software reset to start the counting
    TC_Start(tc, channel);
}
