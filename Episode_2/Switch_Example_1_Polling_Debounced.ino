#include <avr/io.h>
#include <avr/interrupt.h>

// function prototypes
uint16_t RawKeyPressed(void);
bool DebounceSwitch(void);

int main(void)
{
  DDRB |= (1 << DDB5);  // set Arduino pin 13 as output (Arduino builtin LED)
  DDRD &= ~(1 << DDD5); // set Arduino digital pin 5 as input
  PORTD |= (1 << PD5);  // turn on input pullup

  // set up Timer/Counter 0 for 1ms interrupt
  TCCR0A |= (1 << WGM01);  // Set the Timer Mode to CTC
  OCR0A = 62;             // Set the value that we want TC0 to count to (62)
  TIMSK0 |= (1 << OCIE0A); //Set the ISR COMPA vect
  sei();                   //enable global interrupts
  TCCR0B |= (1 << CS02);   // set prescaler to 256 and start the timer

  while (1)  {
  }
}

//this ISR gets called every 1ms
ISR (TIMER0_COMPA_vect)  // timer0 compare A register interrupt
{
  if (DebounceSwitch()) 
    PORTB ^= 1 << 5;
}

bool DebounceSwitch() {
  static uint16_t State = 0; // current debounce status
  State = (State << 1) | RawKeyPressed() | 0xe000;
  if (State == 0xf000) return true;
  return false;
}

uint16_t RawKeyPressed()  {
  return (PIND & 1<<5) >> 5;
}
