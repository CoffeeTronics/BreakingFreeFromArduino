#include <avr/io.h>

#define FOSC 16000000 // 16MHz clock

uint8_t myString[11] = {'A', 'T', 'm', 'e', 'g', 'a', '3', '2', '8', 'P', '\0'};
uint8_t *ptr = &myString[0];

//function forward declarations
void USART_Init(uint16_t, char, uint8_t, uint8_t);
void USART_TransmitByte(uint8_t*);

int main(void) {
  // set up Built-In LED (Arduino pin 13) as output, and set LOW
  DDRB |= 1<<PB5;
  PORTB &= ~(1<<PB5);
  // initialize USART to 9600, N, 8, 1
  USART_Init(9600, 'n', 8, 1);

  while (1)  {
      for (int j = 0; j < strlen(myString); j++) {
      USART_TransmitByte(*ptr++);
    }
    USART_TransmitByte('\n');
    ptr = &myString[0];
  }
  return 0;
}

// user enters required baud rate, parity ('o' =odd, 'e' =even, 'n' = none), 
// data bits and stopbits
void USART_Init(uint16_t baudRate, char parity, uint8_t dataBits, uint8_t stopBits)  {
  uint16_t ubrr = (FOSC / 16UL / baudRate - 1); // get UBRR value from FOSC and baudRate
  // Set baud rate
  UBRR0H = (unsigned char) (ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable both transmit and receive
  
  switch (parity) { // set up parity
    case 'o':
      UCSR0C |= ((1 << UPM00) | (1 << UPM01));
      break;
    case 'e':
      UCSR0C |= (1 << UPM01);
      UCSR0C &= ~(1 << UPM00);
      break;
    case 'n':
      UCSR0C &= ~(1 << UPM01);
      UCSR0C &= ~(1 << UPM00);
      break;
    default:  // default is no parity
      UCSR0C &= ~(1 << UPM01);
      UCSR0C &= ~(1 << UPM00);
      break;
  }
  
  switch (dataBits) { // set up num data bits
    case 5:       // UCSZ0[2:0] = 000
      UCSR0C &= ~(1 << UCSZ00 | 1 << UCSZ01 | 1 << UCSZ02); 
      break;
    case 6:       // UCSZ0[2:0] = 001
      UCSR0C &= ~(1 << UCSZ01 | 1 << UCSZ02); 
      UCSR0C |= (1 << UCSZ00);
      break;
    case 7:
      UCSR0C &= ~(1 << UCSZ00 | 1 << UCSZ02); // UCSZ0[2:0] = 010
      UCSR0C |= (1 << UCSZ01);
      break;
    case 8:       // UCSZ0[2:0] = 011
      UCSR0C &= ~(1 << UCSZ02);           
      UCSR0C |= (1 << UCSZ00 | 1 << UCSZ01);
      break;
    default: // defult is 8 databits  // UCSZ0[2:0] = 011
      UCSR0C &= ~(1 << UCSZ02);           
      UCSR0C |= (1 << UCSZ00 | 1 << UCSZ01);
  }
  
  switch (stopBits) { //set up num stop bits
    case 1: // 1 stop bit
      UCSR0C &= ~(1 << USBS0);
      break;
    case 2: // 2 stop bits
      UCSR0C |= (1 << USBS0);
      break;
    default:  // default is 1 stop bit
      UCSR0C &= ~(1 << USBS0);
      break;
  }
}

void USART_TransmitByte(uint8_t *DataByte)
{
  // Wait until UDR is ready
  while (( UCSR0A & (1 << UDRE0)) == 0) {}; 
  UDR0 = DataByte;
}

unsigned char USART_Receive( void )
{
  /* Wait for data to be received */
  while ( !(UCSR0A & (1 << RXC0)) )
    ;
  /* Get and return received data from buffer */
  return UDR0;
}
