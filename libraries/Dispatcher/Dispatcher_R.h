#ifndef GUARD_DISPATCHER_R_H
#define	GUARD_DISPATCHER_R_H

#include <Settings_R.h>
//==================================================
//===            Local function					 ===
//==================================================

//dummy function for the initialization of the callback member of the struct
//this is use is case of calling a wrong task not to restart the uC and call this function insted
uint8_t dummy_function(void)
{
    Serial.println("Dummy_function");
}


//Initializate with 0 all the task array
void init_arraytasks(void)
{

    for (int i = 0; i != Last_Task; ++i)
    {
        arrayTask[i].callback_function = dummy_function;
        arrayTask[i].runningflag = 0;
        arrayTask[i].timeExpected = 0;
        arrayTask[i].tempflag = 0;
    }
}

//Reset the timer 50ms //10 ms
inline void reset_timer(void)
{
    TCNT1 = 63086; // 53060
}

//Settings for the timer 
//Activate the OVF interrupt every 10ms
inline void set_timer(void) {

    TCCR1A = 0x00;
    TCCR1C = 0x00;

    //Set timer for prescale 64
    TCCR1B = (1 << CS11) | (1 << CS10);
    //Enable Overflow interrupt
    TIMSK1 = (1 << TOIE1);

    //Enable global interrupts
    sei();

    //set the timer 
    reset_timer();
}

inline void stop_timer()
{
    //dezactivate timer OVF interrupt
    TIMSK1 = 0;

    TCCR1B = 0;

    //set the timer 
    reset_timer();
}
//==================================================
//===            API's							 ===
//==================================================

//Set the running task to read_state
inline void set_read_task(uint16_t timeExpected)
{
    RUNNING_TASK = Read_Task;
    arrayTask[Read_Task].runningflag = 1;
    arrayTask[Read_Task].timeExpected = timeExpected;
    arrayTask[Read_Task].tempflag = 0;
    timeCounterRunningFlag = 0;

    stop_timer();
}

//Set the running task to write_1_state
inline void set_write_1_task(uint16_t timeExpected)
{
    RUNNING_TASK = Write_1_Task;
    arrayTask[Write_1_Task].runningflag = 1;
    arrayTask[Write_1_Task].timeExpected = timeExpected;
    arrayTask[Write_1_Task].tempflag = 0;
    timeCounterRunningFlag = 0;

    set_timer();

}

//Set the running task to write_0_state
inline void set_write_0_task(uint16_t timeExpected)
{
    RUNNING_TASK = Write_0_Task;
    arrayTask[Write_0_Task].runningflag = 1;
    arrayTask[Write_0_Task].timeExpected = timeExpected;
    arrayTask[Write_0_Task].tempflag = 0;
    timeCounterRunningFlag = 0;

    set_timer();
}

//Set the running task to delay_state
inline void set_Delay_task(uint16_t timeExpected)
{
    RUNNING_TASK = Delay_Task;
    arrayTask[Delay_Task].runningflag = 1;
    arrayTask[Delay_Task].timeExpected = timeExpected;
    arrayTask[Delay_Task].tempflag = 0;

    set_timer();
}


//Search what task you need to run and run it
void dispatch(void)
{
    static uint8_t tempResult = 0;

    switch (RUNNING_STATE)
    {
    case Default_State:

        tempResult = default_state();
        if (tempResult == 1)
        {
            RUNNING_STATE = Read_Start_Signal_State;
        }
        else if (tempResult == 0)
        {
            RUNNING_STATE = Default_State;
        }
        else
        {
            /*Nothing to do*/
        }

        break;

    case Read_Start_Signal_State:

        tempResult = read_start_signal();
        if (tempResult == 0)
        {
            RUNNING_STATE = Default_State;
        }
        else if (tempResult == 1)
        {
            RUNNING_STATE = Write_Ack_State;
        }
        else
        {
            /*Nothing to do*/
        }

        break;

    case Write_Ack_State:

        tempResult = write_ack_state();
        if (tempResult == 1)
        {
            RUNNING_STATE = Read_Byte_State;
        }
        else
        {
            /*Nothing to do*/
        }

        break;

    case Read_Byte_State:

        tempResult = read_byte_state();
        if (tempResult == 1)
        {
            RUNNING_STATE = Default_State;
        }
        else
        {
            /*Nothing to do*/
        }

        break;

    default:

        current = millis();
        Serial.println(millis() - current);
        Serial.print("Read  = ");
        RUNNING_STATE = Default_State;
        break;
    }

    result_after_running_the_task = arrayTask[RUNNING_TASK].callback_function();
}

//==================================================
//===				INTERRUPT's					 ===
//==================================================

//Enter in this interrupt every 50ms
ISR(TIMER1_OVF_vect)
{
    timeCounterRunningFlag++;									//Count how many time the interrupt was called
    if (timeCounterRunningFlag >= arrayTask[RUNNING_TASK].timeExpected)
    {

        arrayTask[RUNNING_TASK].runningflag = 0;				//Stop this procces and go to the next one by setting the runningflag to 0
        timeCounterRunningFlag = 0;								//Reset the timer index
    }
    else
    {
        /*Nothing to do*/
    }

    reset_timer();												//Reset the timer
}

#endif //GUARD_DISPATCHER_H
