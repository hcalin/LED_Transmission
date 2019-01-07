#include <adc.h>
#include <Dispatcher_R.h>
#include <States_R.h>

long int var = 0;
void setup()
{

  Serial.begin(2000000);
  Serial.println("Start!");
  initPins();
  //turn the led on for 1 sec
  turn_led_on();
  delay(1000);
  turn_led_off();
  
  //add all tasks to the array
  add_all_tasks();
  
}

void loop()
{
  dispatch();
}
