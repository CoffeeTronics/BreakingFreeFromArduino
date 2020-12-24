#include <avr/io.h>
#include <avr/interrupt.h>

// volatile tells the compiler not to optimize this variable away 
volatile uint8_t counter = 0; 
// this counts the number of timer overflows

int main(void)
{
            // set Arduino pin 13 as output
            // Set the Timer Mode to CTC
            // Set the value that we want TC0 to count to (249)
            //Set the ISR COMPA vect
            //enable global interrupts
            // set prescaler to 256 and start the timer
  
  while (1)
  {
            // when 0.5s has elapsed, toggle the LED and reset counter
  }
}

ISR (TIMER0_COMPA_vect)  // timer0 compare A register interrupt
{
  // Interrupt Service Routine should be as short as possible
  // every 4ms increment counter
  counter++;  
}
