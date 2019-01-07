#ifndef GUARD_Settings_R_H
#define GUARD_Settings_R_H

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

//==================================================
//===       Structure Declaration 				 ===
//==================================================

//Enum with the names of the tasks
typedef enum taskName { Read_Task, Write_1_Task, Write_0_Task, Delay_Task, Last_Task }taskName;

//Enum with the name of the states
typedef enum { Default_State, Read_Start_Signal_State, Write_Ack_State, Read_Byte_State }stateName;

//tasks structure
typedef struct {

    uint8_t(*callback_function)(void);
    volatile uint8_t  runningflag;
    volatile uint16_t timeExpected;
    uint8_t tempflag;

}Tasks;


//==================================================
//===            Global Declaration				 ===
//==================================================

//Count how many times the interrupt was enabled
volatile  uint16_t timeCounterRunningFlag = 0;

//Global variables (array of tasks,array of alarms and the deque of tasks)
Tasks arrayTask[Last_Task];

//Current running task
taskName RUNNING_TASK = Read_Task;

//Current running state
stateName RUNNING_STATE = Default_State;

//Variable to store the result after running the task
uint8_t result_after_running_the_task = 1;

int current = 0;
//==================================================
//===       Forward Declaration 				 ===
//==================================================


//Initializate with 0 all the task array
void init_arraytasks(void);

//Set the led as an sensor
uint8_t read_task(void);

//Turn the led ON
uint8_t write_1_task(void);

//Turn the led OFF
uint8_t write_0_task(void);

//Delay the program
uint8_t delay_task(void);

//default state
uint8_t default_state(void);

//Read start signal state
uint8_t read_start_signal(void);

//Write back ack
uint8_t write_ack_state(void);

//Read data and print it
uint8_t read_byte_state(void);


//==================================================
//===				  API						 ===
//==================================================

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


//Create a new task and store it in the taskarray
void vAddNewTask(unsigned char(*vCallbackFunct)(), unsigned char this_task, int timeExpected)
{
    Tasks *tempTask = &arrayTask[this_task];

    tempTask->callback_function = vCallbackFunct;
    tempTask->runningflag = 1;
    tempTask->timeExpected = timeExpected;


    if (this_task > Last_Task)
    {
        Serial.println("You have reached your limit of tasks");
    }

}


//add all tasks to the arrayTasks
void add_all_tasks(void)
{

    //Initialize all the arrayTask with 0
    init_arraytasks();

    //Add all tasks in the array
    //Set the led as a sensor 
    vAddNewTask(read_task, Read_Task, 0);

    //Turn the led ON
    vAddNewTask(write_1_task, Write_1_Task, 5);

    //Turn the led OFF
    vAddNewTask(write_0_task, Write_0_Task, 5);

    //Delay the program
    vAddNewTask(delay_task, Delay_Task, 10);

}

//Set the led ON
inline void turn_led_on(void)
{
    //Set the led Anode to OUTPUT and High
    DDRB |= (1 << PB0);
    PORTB |= (1 << PB0);

    //Set the led Cathode to OUTPUT and Low
    DDRC |= (1 << PC0);
    PORTC &= ~(1 << PC0);
}

//Set tge led OFF
inline void turn_led_off(void)
{
    //turn led off
    //Set the led Anode to OUTPUT and High
    DDRB |= (1 << PB0);
    PORTB &= ~(1 << PB0);

    //Set the led Cathode to OUTPUT and Low
    DDRC |= (1 << PC0);
    PORTC &= ~(1 << PC0);
}

void debugging()
{
    Serial.print("RUNNING_STATE --> ");
    Serial.print(RUNNING_STATE);
    Serial.print("-->");
    Serial.print(arrayTask[RUNNING_TASK].runningflag);

}
//Set the led as sensor
inline uint16_t turn_led_sensor(void)
{

    uint16_t sen_time = 0;

    //debugging();
    //Serial.print("--> sen_time = ");
    //Serial.println(sen_time);

    return sen_time;

}

#endif // GUARD_Settings_H


