#include <avr/io.h>

#define FOSC 16000000 // 16MHz clock
#define BAUD 9600 // Transmit baud rate
#define MYUBRR FOSC/16/BAUD-1

// function prototypes
void USART_Transmit(uint8_t);
void USART_Init(unsigned int);

char myString[11] = {'A', 'T', 'm', 'e', 'g', 'a', '3', '2', '8', 'P', '\0'};

int main(void) {
  USART_Init(MYUBRR);

  while(1)  {
    for (int i = 0; i < strlen(myString); i++) {
        USART_Transmit(myString[i]);
      }
      USART_Transmit('\n');
  }
  return 0;
}

void USART_Init(unsigned int ubrr)  {
  // Set baud rate 
  UBRR0H = (unsigned char) (ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  // Enable both transmit and receive
  UCSR0B = (1<<RXEN0) | (1<<TXEN0);
  // Set frame format: 8data, no parity, 1 stop bit
  UCSR0C =  (3<<UCSZ00); 
}

void USART_Transmit(uint8_t DataByte)
{
  while (( UCSR0A & (1 << UDRE0)) == 0) {}; // Wait until UDR is ready
  UDR0 = DataByte;
}
