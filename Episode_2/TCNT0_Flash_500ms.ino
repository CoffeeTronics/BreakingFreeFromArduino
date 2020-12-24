#include <avr/io.h>
#include <avr/interrupt.h>

// volatile makes the compiler load this variable from RAM
// and not from storage register which may be outdated
volatile uint8_t counter = 0; 
// this counts the number of timer overflows

int main(void)
{
  DDRB |= (1 << DDB5);     // set Arduino pin 13 as output
  TCCR0A |= (1 << WGM01);  // Set the Timer Mode to CTC
  OCR0A = 249;             // Set the value that we want TC0 to count to (249)
  TIMSK0 |= (1 << OCIE0A); //Set the ISR COMPA vect
  sei();                   //enable global interrupts
  TCCR0B |= (1 << CS02);   // set prescaler to 256 and start the timer
  
  while (1)
  {
    if (counter >= 125) {   // 4ms * 125 = 0.5 sec
      PORTB ^= (1 << 5);    // toggle pin 13 LED every 0.5s
      counter = 0;          // reset the counter
    }
  }
}

ISR (TIMER0_COMPA_vect)  // timer0 compare A register interrupt
{
  // Interrupt Service Routine should be as short as possible
  // every 4ms increment counter
  counter++;  
}
