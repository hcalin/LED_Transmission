#include "adc.h"

// Select the MUX channel of your choice
#define CLEAR_MUX(A)  ADMUX &= ~(0X0F)
#define MUX(A)        ADMUX |= (A)



void initPins(void)
{
  /*  Set all pins as OUTPUT LOW
   *   We use just 
   *   A0 for LED input and 
   *   A1 for C/H reference voltage 
   */
  DDRB = 0xFF;
  DDRC = 0xFF;
  DDRD = 0xFF;
  PORTB = 0x00;
  PORTC = 0x00;
  PORTD = 0x00;
}

int main(void)
{
  uint16_t result;
  initADC();
  initPins();
  Serial.begin (9600);

  while(1)
  {

    result = getConvData();
    Serial.println(result);
  }
    return 0;

}
