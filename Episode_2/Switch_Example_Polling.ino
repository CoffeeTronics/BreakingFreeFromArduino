#include <avr/io.h>
#include <avr/interrupt.h>

// function prototype
uint8_t ButtonPressed(void);

int main(void)
{
  DDRB |= (1 << DDB5);  // set Arduino pin 13 as output
  DDRD &= ~(1 << DDD5); // set Arduino digital pin 5 as input
  PORTD |= (1 << PD5);  // turn on input pullup

  while (1)  {
    ButtonPressed();
  }
}

uint8_t ButtonPressed(void)  {
  static uint8_t buttonState = 0;
  static uint8_t lastButtonState = 0;
  buttonState = (PIND & (1<<5)) >> 5;
  if ((buttonState != lastButtonState) && (buttonState == 0)) {
    PORTB ^= 1 << 5;
  }
  lastButtonState = buttonState;
  return buttonState;
}
