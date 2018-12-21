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
   
   // charge C/H internal capacitance
   SET_HIGH(1);
   MUX(1);
   _delay_us(10);
   CLEAR_MUX();
   
   // discharge external capacitance
   SET_LOW(0);
   SET_INPUT(0);
   _delay_ms(5); // could be optimized

   // get first sample
   sample1 = getConvData();

   // discharge C/H internal capacitance
   SET_LOW(1);
   MUX(1);
   _delay_us(20);

   // charge external capacitance
   SET_OUTPUT(0);
   SET_HIGH(0);
   ANODE_LOW();
   for(volatile uint8_t i = 0; i < 3 ; i++);
   CLEAR_MUX();
   _delay_ms(5); // could be optimized
   
   // get second sample
   sample2 = getConvData();
   
   return (sample2 - sample1);
}

int main(void)
{
  uint16_t result;
  initADC();
  initPins();
  Serial.begin (9600);

  while(1)
  {
    Serial.println(readSample());
  }
    return 0;
}