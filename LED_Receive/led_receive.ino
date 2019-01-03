#include "adc.h"
#include <util/delay.h>

// Select the MUX channel of your choice
#define  CLEAR_MUX()   ADMUX &= ~(0X0F)
#define  MUX(A)         ADMUX |= (A - 1)
#define  SET_INPUT(A)   DDRC &= ~(1 << A)
#define  SET_OUTPUT(A)  DDRC |= (1 << A) 
#define  SET_LOW(A)     PORTC &= ~(1 << A) 
#define  SET_HIGH(A)    PORTC |= (1 << A) 
#define  ANODE_LOW()    PORTD &= (1 << 3)

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

int16_t readSample(){
   uint16_t sample1 = 0;
   uint16_t sample2 = 0;
   CLEAR_MUX();
      
   // charge C/H internal capacitance
   SET_OUTPUT(1);
   SET_HIGH(1);
   ADMUX = 0x11; 
   _delay_us(10);

   // discharge external capacitance
   SET_OUTPUT(0);
   SET_LOW(0);
   _delay_ms(10); // could be optimized

   //Connect the internal capacitor with the external one
   ADMUX = 0x10; 
   _delay_us(10);
   
   // get first sample
   SET_INPUT(0);
   sample1 = getConvData();

   // discharge C/H internal capacitance
   SET_OUTPUT(1);
   SET_LOW(1);
   ADMUX = 0x11;
   _delay_us(10);

   // charge external capacitance
   SET_OUTPUT(0);
   SET_HIGH(0);
   _delay_us(10);

   //Connect the internal capacitor with the external one
   ADMUX = 0x10;
   _delay_ms(10); // could be optimized
   
   // get second sample
   SET_INPUT(0);
   sample2 = getConvData();
   
   return (sample2 - sample1);
}

int main(void)
{
  init();
  uint16_t result;
  initADC();
  initPins();
  Serial.begin (2000000);

  while(1)
  {
    Serial.println(readSample());
  }
    return 0;
}
