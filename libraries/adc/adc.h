/*
 * adc.h
 *
 */

#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <stdint.h>

/* Define auto-triggering sources */

#define FREE_RUNNING	(0x00)
#define AN_COMPARATOR	(0x01)
#define EXT_IRQ			  (0x02)
#define TMR0_COMPA		(0x03)
#define	TMR0_OVF		  (0x04)
#define TMR1_COMPB		(0x05)
#define TMR1_OVF		  (0x06)
#define TMR1_CAP		  (0x07)


/* ADC Channel Selection */
#define CHANNEL_0		(0x00)
#define CHANNEL_1		(0x01)
#define CHANNEL_2		(0x02)
#define CHANNEL_3		(0x03)
#define CHANNEL_4		(0x04)
#define CHANNEL_5		(0x05)

/* Start Channel Selection */
#define ADC_CHANNEL    CHANNEL_0

/* Reference voltage settings */
#define	AREF			  (0x00)
#define AVCC			  (0x01)
#define BAND_REF		  (0x03)

/* ADC Clock settings */
#define PRESCALAR_2		(0x01)		// ADC_CLK = CLK/2
#define PRESCALAR_4		(0x02)		// ADC_CLK = CLK/4
#define PRESCALAR_8		(0x03)		// ADC_CLK = CLK/8
#define PRESCALAR_16	(0x04)		// ADC_CLK = CLK/16
#define PRESCALAR_32	(0x05)		// ADC_CLK = CLK/32
#define PRESCALAR_64	(0x06)		// ADC_CLK = CLK/64
#define PRESCALAR_128	(0x07)		// ADC_CLK = CLK/128

/* Useful Macros */
#define POWER_DOWN()  (PRR |= _BV(PRADC))
#define ADC_ENABLE()  (ADCSRA |= _BV(ADEN))
#define ADC_DISABLE()	(ADCSRA &= ~_BV(ADEN))
#define ADC_START()	(ADCSRA |= _BV(ADSC))
#define ADC_DONE()	(ADCSRA & _BV(ADIF))
#define ADC_INTEN()	(ADCSRA |= _BV(ADIE))
#define AUTO_EN()		 (ADCSRA |= (AUTO_TRIG << ADATE))


/* Prototypes */
void initADC(void);
uint16_t getConvData(void);


/*! Sets up ADC reference voltage, channel and output format */
void initADC(void)
{

  // AVCC reference; right-adjusted; channel selection
  ADMUX |= (1 << REFS0) | (ADC_CHANNEL);

    // Prescalar by 2
  ADCSRA = (1 << PRESCALAR_2);
  ADC_ENABLE();
}

/*! \brief Retrieves converted data; blocking function */
uint16_t getConvData(void)
{
  /*
   * There are a couple of ways of checking if the conversion is over.
   * 1. In a single-ended conversion, the ADSC is cleared as soon as the
   *    conversion is over otherwise it stays high.
   * 2. The ADIF bit is set on conversion complete.
   * 3. The ADIF bit could set off an interrupt to indicate end of conversion.
   *
   * Note that the first method wouldn't be useful if ADC is free-running, in
   * which case methods 2 or 3 would be appropriate.
   *
   */

  uint16_t result;

  ADC_START();

  // while (ADCSRA & _BV(ADSC));
  while(!ADC_DONE());           // Stay here while conversion completes

  // ADC data update is blocked on reading ADCL, keeps read atomic
  // result = (uint16_t) (ADCL + (ADCH << 8));
  // Otherwise, just read the 16-bit register, ADCW
  result = (uint16_t) (ADCW);

  return result;
}

#endif