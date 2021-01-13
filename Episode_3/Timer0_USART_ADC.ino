#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC 16000000UL // Defining the CPU Frequency 

volatile uint8_t counter = 0; // this counts the number of timer overflows

uint8_t myString[11] = {'A', 'T', 'm', 'e', 'g', 'a', '3', '2', '8', 'P', '\0'};
uint8_t *ptr = &myString[0];

// variables to break up the read ADC value
uint8_t ADC0_Result_Thousands = 0;
uint8_t ADC0_Result_Hundreds = 0;
uint8_t ADC0_Result_Tens = 0;
uint8_t ADC0_Result_Ones = 0;

uint16_t temp = 0;  // used to hold ADC0_Result to perform operations on

// function prototypes
void Timer0_Init(void);
void USART_Init(uint16_t, char, uint8_t, uint8_t);
void USART_TransmitByte(uint8_t*);
void ADC_Init(void);
uint16_t ADC_Read(char);

int main(void)  {
  USART_Init(4800, 'o', 7, 2);
  Timer0_Init();
  ADC_Init();
  
  while (1) {
    if (counter >= 125) {   // 4ms * 125 = 500ms
      temp = ADC_Read(0); // read ADC channel 0 (
      
      ADC0_Result_Ones = temp % 10;
      temp = temp / 10;
      ADC0_Result_Tens = temp % 10;
      temp = temp / 10;
      ADC0_Result_Hundreds = temp % 10;
      ADC0_Result_Thousands = temp / 10;

      // Convert to ASCII values, adds 0x30 to each
      ADC0_Result_Thousands += 0x30;
      ADC0_Result_Hundreds += 0x30;
      ADC0_Result_Tens += 0x30;
      ADC0_Result_Ones += 0x30;

      // write the ADC reading to the USART 
      USART_TransmitByte(ADC0_Result_Thousands);
      USART_TransmitByte(ADC0_Result_Hundreds);
      USART_TransmitByte(ADC0_Result_Tens);
      USART_TransmitByte(ADC0_Result_Ones);
      USART_TransmitByte('\n');
      // reset the counter
      counter = 0;        
    }
  }
}


ISR (TIMER0_COMPA_vect)  // timer0 overflow interrupt
{
  counter++;  // every 4ms increment counter
}

ISR (ADC_vect)  // ADC interrupt 
{
}

uint16_t ADC_Read(char channel)  {
  ADMUX = (1<<REFS0) | (channel & 0x0f); // select channel requested
  ADCSRA |= (1<<ADSC);                    // start conversion
  while (ADCSRA & (1<<ADSC));             // wait for result
  return ADCW;
}

void ADC_Init()  {
  ADMUX = (1 << REFS0);   // uses AVCC which is connected to +5V on UNO 
  ADMUX &= 0xF0;          // clear 4 LSB to set ADC0
  ADCSRA |= (1<<ADEN) | (7<<ADPS0); // set ADC clock to 125kHz and enable ADC
}

void Port_Init()  {
  DDRB |= (1 << 5);         // set Arduino pin 13 (PORTB bit 5) as output
}

void Timer0_Init()  {
  TCCR0A |= (1 << WGM01);     // Set the Timer Mode to CTC
  OCR0A = 0xF9;               // Set the value that we want to count to (249)
  TIMSK0 |= (1 << OCIE0A);    //Set the ISR COMPA vect
  sei();                      //enable global interrupts
  TCCR0B = (1 << CS02);       // set prescaler to 256 and start the timer
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
  // Wait for data to be received 
  while ( !(UCSR0A & (1 << RXC0)) )
    ;
  // Get and return received data from buffer 
  return UDR0;
}
