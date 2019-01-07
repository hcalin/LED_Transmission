#ifndef GUARD_STATES_R_H
#define GUARD_STATES_R_H

//==================================================
//===				 Tasks						 ===
//==================================================

//Set the led as an sensor
uint8_t read_task(void)
{

	//Variable to check the result
	static uint16_t compareVariableWithSenTime = 930;

	//Buffer to store the senzore value
	static uint16_t sen_timeBuffer = 0;

	//Store the current senzor Value
	sen_timeBuffer += turn_led_sensor();

	//Count how many values reads
	arrayTask[Read_Task].tempflag++;

	//Stop when count equels to time expected value
	if (arrayTask[Read_Task].tempflag >= arrayTask[Read_Task].timeExpected)
	{
		//make an average 
		uint16_t sen_time = sen_timeBuffer / arrayTask[Read_Task].tempflag;

		//reset the values
		sen_timeBuffer = 0;
		arrayTask[Read_Task].tempflag = 0;

		//Stop this task
		arrayTask[Read_Task].runningflag = 0;

		//recived an 0
		if (sen_time > compareVariableWithSenTime)
		{
			return 0;
		}
		//Recived an 1
		else
		{
			return 1;
		}
	}
	else
	{
		return 255;
	}

}

//Turn the led ON
uint8_t write_1_task(void)
{

	if (arrayTask[Write_1_Task].tempflag == 0)
	{
		turn_led_on();
		arrayTask[Write_1_Task].tempflag = 1;

		return 255;
	}

	if (arrayTask[Write_1_Task].runningflag == 0)
	{
		turn_led_off();
		arrayTask[Write_1_Task].tempflag = 0;

		return 1;
	}
	else
	{
		return 255;
	}


}

//Turn the led OFF
uint8_t write_0_task(void)
{

	if (arrayTask[Write_0_Task].tempflag == 0)
	{
		turn_led_off();
		arrayTask[Write_0_Task].tempflag = 1;

		return 255;
	}

	if (arrayTask[Write_0_Task].runningflag == 0)
	{
		arrayTask[Write_0_Task].tempflag = 0;

		return 1;
	}
	else
	{
		return 255;
	}
}

//Delay the program
uint8_t delay_task(void)
{

	if (arrayTask[Delay_Task].runningflag == 0)
	{
		return 1;
	}
	else
	{
		return 255;
	}
}

//==================================================
//===				 States						 ===
//==================================================

//default state
uint8_t default_state(void)
{
	static uint8_t internal_flag = 0;

	if (internal_flag == 0)
	{
		set_read_task(4);
		internal_flag = 1;
		return 255;
	}
	else if (result_after_running_the_task == 1)
	{
		internal_flag = 0;
		set_Delay_task(1);
		return 1;
	}
	else if (result_after_running_the_task == 0)
	{
		internal_flag = 0;
		set_Delay_task(1);
		return 0;
	}
	else
	{
		return 255;
	}
}

//read start signal
uint8_t read_start_signal(void)
{
	//variables to count the 1 or 0 bites recived in read_start_signal
	static uint8_t recived_1 = 0;
	static uint8_t recived_0 = 0;
	static uint8_t internal_flag = 0;

	//Read the values once evry 40 ms
	if (internal_flag == 0)
	{
		set_read_task(4);
		internal_flag = 1;
		return 255;
	}

	//Count the recived values
	if (result_after_running_the_task == 1)
	{

		recived_1++;
		set_read_task(4);
	}
	else if (result_after_running_the_task == 0)
	{
		recived_0++;
		set_read_task(4);
	}
	//Check the results
	if (recived_0 > 2)
	{

		recived_0 = 0;
		recived_1 = 0;
		internal_flag = 0;

		set_Delay_task(1);

		return 0;
	}
	else if ((recived_1 + recived_0) >= 9 && recived_1 >= 7)
	{
		recived_0 = 0;
		recived_1 = 0;
		internal_flag = 0;

		set_Delay_task(1);
		return 1;
	}
	else
	{
		return 255;
	}


}

//write ack 
uint8_t write_ack_state(void) 
{
	static uint8_t internal_flag = 0;

	if (internal_flag == 0)
	{
		set_write_1_task(10);
		internal_flag = 1;

		return 255;
	}

	if (result_after_running_the_task == 1)
	{
		internal_flag = 0;
		set_Delay_task(1);
		return 1;
	}
	else
	{
		return 255;
	}

}

//Read 8 bits and print them
uint8_t read_byte_state(void)
{
	static uint8_t internal_flag = 0;
	static uint8_t internal_flag_tasks = 0;
	if (internal_flag == 0)
	{

		set_Delay_task(5);
		internal_flag = 1;
		return 255;
	}

	switch (internal_flag_tasks)
	{
	case 0:
		if (result_after_running_the_task == 1)
		{
			set_read_task(10);
			internal_flag_tasks = 1;
		}
		break;

	case 1:
		if (result_after_running_the_task == 0)
		{
			Serial.print(0);
			set_Delay_task(5);
			internal_flag_tasks = 2;
		}
		else if (result_after_running_the_task == 1)
		{

			Serial.print(1);
			set_Delay_task(5);
			internal_flag_tasks = 2;
		}
		break;

	case 2:

		if (result_after_running_the_task == 1)
		{
			static uint8_t index = 0;
			index++;
			if (index >= 8)
			{
				Serial.println("");
				index = 0;
				internal_flag = 0;
				internal_flag_tasks = 0;
				set_Delay_task(1);
				return 1;
			}
			internal_flag = 0;
			internal_flag_tasks = 0;


		}
		break;
	}
}

#endif