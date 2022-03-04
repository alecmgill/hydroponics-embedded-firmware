/*
 * calibrate_system.c
 *
 *  Created on: Feb 18, 2022
 *      Author: Alec
 */
#include "hydro_control_loop.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "File_Handling.h"
#include "fatfs.h"
#include "usb_host.h"
#include "stm32f2xx_hal.h"
#include "calibrate_system.h"
#include "main.h"

// NOTES for the following chars: line write buffers are used to append file data/names without effecting out header arrays
char initial_TDS[6] = {0}, post_nutrient_TDS[6] = {0}, initial_PH[6] = {0},post_nutrient_PH[6] = {0}, time_to_equalize[6] = {0}, calibration_data_array[10000] = {0},initial_nutrient_buffer[10000] = {0}, data_buffer[10000] = {0}, converted_char[7] = {0},
		first_line_write_buf[45] = {0}, second_line_write_buf[45]  = {0}, third_line_write_buf[45] = {0}, fourth_line_write_buf[45] =  {0}, fifth_line_write_buf[45] =  {0};

// NOTES for the following ints: if been_written = 1 pH-Up data file written flash drive, 2 pH-down file written to flash drive.
// If what_to_save value  = 0 no file selected to save, 1 save first file, 2 save second file
// minimum_sec_duration holds the number of seconds just to get the data.
// if up_down_total has a value of 3 it signifies both ph-up/ph-down cycle has completed
int data_array_length = 10000, write_times = 0, been_written = 0, number_of_files = 1, what_to_save = 0, total_nutrient_ml_cal = 0, write_header = 0, total_runs = 0, eq_end_time_sec = 0, eq_start_time_sec = 0,
		stability_value = 1, initial_run = 1, start_time_seconds = 0, end_time_seconds = 0, total_time_seconds = 0, max_runs = 0, previous_written = 0, up_down_total = 0;

// Notes for below float declarations: smallest values TDS and pH set them to values much higher than expected and set largestTFS/pH to the smallest possible value to catch edge cases
// minimum_sec_duration holds the number of seconds just to get the data.
float sample_array_TDS[30] = {0}, sample_array_pH[30] = {0}, smallest_value_TDS = 100000, largest_value_TDS = 0, smallest_value_pH = 100, largest_value_pH = 0, prev_largest_pH = 0, prev_largest_TDS = 0, TDS_range = 0,
		pH_range = 0, minimum_sec_duration = 36;

double min_ph = 5.0, max_ph = 6.5, nutrient_max = 950, nutrient_min = 450, ph_calibration_dose = 0.5, nutrient_calibration_dose = 40.0;

// following char and int's are used to format the output file
char header[] = "\"nurient vs pH-down characteristics. Fresh R/O water volume of 5 gal, nutrient solution appx 23g in 1L\",,,,\nTemp,pH post pH-up/down ,TDS post ph-up/down,time to equalize pH,pH Dose, Notes\n",
		first_line[45] = "TDS before nutrient addition: ,", second_line[45] = "TDS after 10ml nutrient addition:,", third_line[45] = "pH before nutrient addition: ,", fourth_line[45] = "pH after 40ml nutrient addition: ,",
		fifth_line[45] =  "Time to stabalize:,", ph_up_file_name[50] = "/PH_UP_Calibration_data_total_nutrient_", ph_down_file_name[50] = "/PH_Down_Calibration_data_total_nutrient_", file_name_buffer[50] = {0}, file_extention[] = ".csv",
		total_nutrient_string[7] = {0}, single_char[1] = {0}, new_line[] = "\n";

int last_val_one = 0,last_val_two = 0,last_val_three = 0,last_val_four = 0,last_val_five = 0, line_number = 0, data_value_ready = 0, been_appended = 0;

void getFiveSamples(int delay_and_temp_enable)
{
	water_temp = 0;
	TDS = 0;
	pH = 0;
	for(int fiveSamples = 0; fiveSamples<5;fiveSamples++)
	{
								// get sensor data
		TDS 		  += readWaterTDS();
		pH 		 	  += readPH();
		if(delay_and_temp_enable == 1)
		{
			water_temp	  += readWaterTemp();
			HAL_Delay(100);
		}
	}
	water_temp	  = water_temp/5;							    // get sensor data so we can tell if its time to do pH up or pH down
	TDS 		  = TDS/5;
	pH 		 	  = pH/5;
}

void saveInitialState()
{
	getFiveSamples(1);											// get the current TDS and pH

	sprintf(converted_char, "%.2f", TDS);						// convert TDS to a 6 char array
	for(int e = 0; e<6;e++) initial_TDS[e] = converted_char[e]; // transfer the 6 chars to the initial TDS and pH variables

	sprintf(converted_char, "%.2f", pH);
	for(int i = 0; i<6;i++) initial_PH[i] = converted_char[i];  // transfer the 6 chars to the initial TDS and pH variables
}

void saveStateAfterNutrient(float totalSeconds)
{
	TDS = 0;															// reset TDS and pH vars
	pH = 0;
	for(int d = 0; d<5;d++) 											// get 5 samples of the initial sensor data
	{
		TDS  = TDS+readWaterTDS(); 										// get sensor data
		pH 	= pH+readPH();
	}
	TDS = TDS/5;														// get the average of 5 samples
	pH = pH/5;

	sprintf(converted_char, "%.2f", TDS);
	for(int e = 0; e<6;e++) post_nutrient_TDS[e] = converted_char[e];	// transfer the 6 chars to the initial TDS and pH variables

	sprintf(converted_char, "%.2f", pH);
	for(int i = 0; i<6;i++) post_nutrient_PH[i] = converted_char[i];	// transfer the 6 chars to the initial TDS and pH variables

	sprintf(converted_char, "%.2f", totalSeconds);
	for(int f = 0; f<6;f++) time_to_equalize[f] = converted_char[f];	// transfer the 6 chars to the initial TDS and pH variables
}



float prev_smallest_ph = 0;
float prev_smallest_TDS = 0;
int sample_time = 0;

int valid = 0;
float historic_largest_pH[30] = {0};
float historic_smallest_pH[30] = {0};
float historic_largest_TDS[30] = {0};
float historic_smallest_TDS[30] = {0};
float historic_average_pH[30] = {0};
float historic_average_TDS[30] = {0};
int	historic_sample_index = 0;
float historic_average_pH_range = 0;
float historic_average_TDS_range = 0;
float historic_range_pH  = 0;
float historic_range_TDS = 0;
float average_pH = 0;
float average_TDS = 0;
float historic_average_pH_min = 1000;
float historic_average_pH_max = 0;
float historic_average_TDS_min = 100000;
float historic_average_TDS_max = 0;
float historic_TDS_max = 0;
float historic_pH_max  = 0;
float historic_pH_min  = 0;
float historic_TDS_min  = 0;
float slope_factor_average_TDS = 0;
float slope_factor_average_ph = 0;
int recheck_count = 0;
int run_again = 1;
void resetStabilityVars()
{
	//total_time_seconds = end_time_seconds-start_time_seconds;
		average_pH = 0;
		average_TDS = 0;
		smallest_value_TDS = 100000;							   // set smallest values to value much higher than expected
		largest_value_TDS = 0;	  							   // set largest to the smallest possible value these steps ensure we catch error cases
		smallest_value_pH = 100;
		largest_value_pH = 0;
		run_again = 1; // set to one its the first run 2 is multiple 0 is do not run again

		valid = 0;
		historic_sample_index = 0;
		slope_factor_average_TDS = 0;
		slope_factor_average_ph = 0;

		 historic_average_pH_min = 1000;
		 historic_average_pH_max = 0;
		 historic_average_TDS_min = 100000;
		 historic_average_TDS_max = 0;
		//slope_factor_TDS = 0;
		//slope_factor_ph = 0;
		for(int h = 0; h<30; h++)
		{
			historic_largest_pH[h] = 0;
			historic_smallest_pH[h] = 0;
			historic_largest_TDS[h] = 0;
			historic_smallest_TDS[h] = 0;
		}
}

void isStabalized()  // will take a few samples of the waters pH and TDS to determine if the solution has stabilized. If stabilized returns one else 0
{		// get the current system time sets global variables sTime and sDate in RTC_driver.c


	end_time_seconds = 0;
	prev_smallest_ph = smallest_value_pH;
	prev_smallest_TDS = smallest_value_TDS;
	prev_largest_TDS = largest_value_TDS;
	prev_largest_pH = largest_value_pH;

	HAL_Delay(5000);
	for(int samples = 0; samples<30; samples++)			   // sample TDS and PH every half second for 30 times
	{

		sample_array_TDS[samples] = readWaterTDS();
		sample_array_pH[samples] = readPH();
		HAL_Delay(10);
	}

	smallest_value_TDS = 10000;							   // set smallest values to value much higher than expected
	largest_value_TDS = 0;	  							   // set largest to the smallest possible value these steps ensure we catch error cases
	smallest_value_pH = 100;

    largest_value_pH = 0;

    float slope_factor_ph = 0;							   // if positive we are increasing, negative decreasing
    float slope_factor_TDS = 0;

	for(int i = 0; i<30;i++)
	{

		if(sample_array_TDS[i] > largest_value_TDS)  largest_value_TDS  = sample_array_TDS[i];	// find largest and smallest values in our data
		if(sample_array_TDS[i] < smallest_value_TDS) smallest_value_TDS = sample_array_TDS[i];
		if(sample_array_pH[i]  > largest_value_pH)	 largest_value_pH   = sample_array_pH[i];
		if(sample_array_pH[i]  < smallest_value_pH)  smallest_value_pH  = sample_array_pH[i];

		average_pH  += sample_array_pH[i];
		average_TDS += sample_array_TDS[i];
		// find the slope of the data
		if(i>1)
		{
			slope_factor_TDS += (sample_array_TDS[i] - sample_array_TDS[i-1]);
			slope_factor_ph  += (sample_array_pH[i]  - sample_array_pH[i-1]);
		}
	}

	TDS_range = largest_value_TDS - smallest_value_TDS; // calculate the range in the samples i.e. (largest valve)-(smallest value)
	pH_range  = largest_value_pH  - smallest_value_pH;

	historic_TDS_max = 0;
	historic_TDS_min = 100000;
	historic_pH_max  = 0;
	historic_pH_min  = 100000;

	average_pH  = average_pH/30;
	average_TDS = average_TDS/30;
	if(historic_sample_index < 30)
	{
		historic_largest_pH[historic_sample_index]   = largest_value_pH;
		historic_smallest_pH[historic_sample_index]  = smallest_value_pH;
		historic_largest_TDS[historic_sample_index]  = largest_value_TDS;
		historic_smallest_TDS[historic_sample_index] = smallest_value_TDS;
		historic_average_pH[historic_sample_index]   = average_pH;
		historic_average_TDS[historic_sample_index]  = average_TDS;
		historic_sample_index++;

		if(historic_sample_index > 7)
		{
			for(int k = historic_sample_index-7; k<historic_sample_index; k++) 								// if we have at-least 5 samples find the max and min to calculate range of the last 5 runs
			{
				if(historic_largest_TDS[k]  > historic_TDS_max)   		historic_TDS_max 		  = historic_largest_TDS[k];	// find largest and smallest valuesin our data
				if(historic_smallest_TDS[k] < historic_TDS_min)  		historic_TDS_min		  = historic_smallest_TDS[k];
				if(historic_largest_pH[k]   > historic_pH_max)     		historic_pH_max  		  = historic_largest_pH[k];
				if(historic_smallest_pH[k]  < historic_pH_min)    		historic_pH_min  		  = historic_smallest_pH[k];
				if(historic_average_pH[k]   < historic_average_pH_min)  historic_average_pH_min   = historic_average_pH[k];
				if(historic_average_pH[k]   > historic_average_pH_max)  historic_average_pH_max   = historic_average_pH[k];
				if(historic_average_TDS[k]  < historic_average_TDS_min) historic_average_TDS_min  = historic_average_TDS[k];
				if(historic_average_TDS[k]  > historic_average_TDS_max) historic_average_TDS_max  = historic_average_TDS[k];

				slope_factor_average_TDS += (historic_average_TDS[historic_sample_index]-historic_average_TDS[historic_sample_index-1]);
				slope_factor_average_ph  += (historic_average_TDS[historic_sample_index]-historic_average_TDS[historic_sample_index-1]);
			}
			historic_range_pH  = historic_pH_max  - historic_pH_min;
			historic_range_TDS = historic_TDS_max - historic_TDS_min;
			historic_average_pH_range  = historic_average_pH_max  - historic_average_pH_min;
		    historic_average_TDS_range = historic_average_TDS_max - historic_average_TDS_min;
		}
	}
	else
	{
		resetStabilityVars();
		run_again = 2;
	}
	if(run_again != 2 && (historic_sample_index <= 7 || TDS_range > 10 || pH_range > 0.06 || historic_average_pH_range > 0.02 || historic_average_pH_range < -0.02  || historic_range_pH > 0.07 || historic_range_pH < -0.07 || historic_range_TDS > 12.8 || historic_range_TDS < -12.8 || historic_average_TDS_range > 2.6 || historic_average_TDS_range < -2.6))
	{
 		valid = 0;
 		recheck_count++;
 		run_again = 2; 				// if the ph or TDS values are still changing check again, or the smallest value is at the beginning of the array, or the largest value is at the end of the array
	}
	else if(run_again == 1)		    // we are stabilized however, the system could still be changing so we need to check how many times we re-sampled the water if its > 1 check until its 1
	{
		run_again = 0; 				// set run again to 0(no) however, if our validity is not high enough we will set run_again to 2(yes)
		valid++;
		if(valid < 4)				// if we have not completed three valid runs in a row re-run
		{

			HAL_Delay(5000);
			run_again = 2;
		}
	}
	if(run_again == 1) run_again = 0;
	else if (run_again == 2) run_again = 1;
}


int waitForWaterToStabilize() // Returns the total time in seconds
{
	getTime();
	eq_start_time_sec = sTime.Seconds + sTime.Minutes*60;


	while(run_again > 0)	 isStabalized();// if we are not stabilized wait until we are

	getTime();
	eq_end_time_sec = sTime.Seconds + sTime.Minutes*60;
	if(eq_start_time_sec > eq_end_time_sec) // if seconds rolled over we cannot just subtract since we will get a negative
	{
		total_time_seconds = (3600-eq_start_time_sec)+eq_end_time_sec;
	}
	else total_time_seconds = eq_end_time_sec-eq_start_time_sec;	// else we did not roll over so total seconds is end time - start time

	stability_value = 0;
	recheck_count = 0;
	resetStabilityVars();
	return (total_time_seconds);
}

void set_init_water_ph()
{
	max_runs = 0;
	if((pH-min_ph)<(max_ph-pH) && (pH-min_ph) > 0.125) 			// if we are closer to minimum set point dose pH-Down until we reach the minimum set point
	{
		while (pH>min_ph && max_runs < 20)
		{
			doseWater(ph_calibration_dose, 0, 0);
			waitForWaterToStabilize();
			getFiveSamples(1);
			max_runs++;
		}
	}
	else if((pH-min_ph)>(max_ph-pH) && (max_ph-pH) > 0.125)
	{
		while (pH<max_ph && max_runs < 20)
		{
			doseWater(0, ph_calibration_dose, 0);				// else dose with pH-up
			waitForWaterToStabilize();//
			getFiveSamples(1);
			max_runs++;
		}
	}
}
char nutrient_addition_title[] = "nutrient ml, TDS, pH, time to stabilize \n";
int nutrient_index = 0;
void calibrateSys()
{
	if(what_to_save == 0) getFiveSamples(1);						// get initial sensor data

	if(initial_run == 1)				 						// if the first run
	{
		saveInitialState();
		nutrient_index = 0;
		int i = 0;
		//while(TDS<nutrient_min)									// while we are not at our minimum viable nutrient level dose the water
		for(i = 0;i < (sizeof nutrient_addition_title) && nutrient_addition_title[i] != '\0';i++) // add title to the initial nutrient file buffer
		{
			initial_nutrient_buffer[nutrient_index] = nutrient_addition_title[i];
			nutrient_index++;
		}

		for(int testCount = 0; testCount < 5; testCount++)
		{
			//doseWater(0, 0, nutrient_calibration_dose);			// add nutrient and wait for it to equalize
			total_nutrient_ml_cal += nutrient_calibration_dose;
			//	total_time_seconds = waitForWaterToStabilize();		// Returns the total time in seconds
			sprintf(converted_char, "%.2f", (double)total_nutrient_ml_cal);	// convert water temp to char array and write it to the data buffer
			for(i = 0;i < (sizeof converted_char) && converted_char[i] != '\0';i++)
			{
				initial_nutrient_buffer[nutrient_index] = converted_char[i];
				nutrient_index++;
			}
			initial_nutrient_buffer[nutrient_index] = ',';
			nutrient_index++;
			sprintf(converted_char, "%.2f", TDS);	// convert water temp to char array and write it to the data buffer
			for(i = 0;i < (sizeof converted_char) && converted_char[i] != '\0';i++)
			{
				initial_nutrient_buffer[nutrient_index] = converted_char[i];
				nutrient_index++;
			}
			initial_nutrient_buffer[nutrient_index] = ',';
			nutrient_index++;
			sprintf(converted_char, "%.2f", pH);	// convert water temp to char array and write it to the data buffer
			for(i = 0;i < (sizeof converted_char) && converted_char[i] != '\0';i++)
			{
				initial_nutrient_buffer[nutrient_index] = converted_char[i];
				nutrient_index++;
			}
			initial_nutrient_buffer[nutrient_index] = ',';
			nutrient_index++;
			sprintf(converted_char, "%.2f", (double)total_time_seconds);	// convert water temp to char array and write it to the data buffer
			for(i = 0;i < (sizeof converted_char) && converted_char[i] != '\0';i++)
			{
				initial_nutrient_buffer[nutrient_index] = converted_char[i];
				nutrient_index++;
			}
			initial_nutrient_buffer[nutrient_index] = ',';
			nutrient_index++;
			initial_nutrient_buffer[nutrient_index] = '\n';
			nutrient_index++;
			getFiveSamples(1);


		}
		set_init_water_ph();		 							// add ph-up/ph-down if the water pH is out of our calibration bounds
		saveStateAfterNutrient(total_time_seconds);
		initial_run = 0;
	}

	if(abs(previous_written-been_written)>0 && write_times < number_of_files) // if we just wrote a file and have not completed collecting data for all files reset buffers/vars
	{
		for(int i = 0; i<10000; i++) 							// if we wrote a file reset the data_buffer and data array to allow multiple files to be written
		{
			initial_nutrient_buffer[i] = '\0';
			data_buffer[i-1]				   = '\0';
			calibration_data_array[i] 		   = '\0';
			if(i<44)
			{
				first_line_write_buf[i]  = '\0';  			// append to beginning time to equilize, ph after 5ml ph up, TDS adter 5ml ph-up
				second_line_write_buf[i] = '\0';
				third_line_write_buf[i]  = '\0';
				fourth_line_write_buf[i] = '\0';
				fifth_line_write_buf[i]  = '\0';
			}
			if(i == 1)
			{
				write_header = 0;								// reset the write header var so we can output multiple files
				what_to_save = 0;
				initial_run = 0;
			}
		}
		previous_written = been_written;
		up_down_total += been_written;
	}

	if(up_down_total == 3) 										// if we wrote both ph-up and ph-down files dose with nutrients
	{
		saveInitialState();
		doseWater(0, 0, nutrient_calibration_dose);				// add nutrient here and wait for it to equalize step the pump motors (pHDown,pHup,nutrient_dose)
		total_nutrient_ml_cal += nutrient_calibration_dose;
		if(total_nutrient_ml_cal >= 3*nutrient_calibration_dose)ph_calibration_dose++; // if we have added 2 nutrient doses increase the pH-up/down dose since it takes more to equalize
		total_time_seconds = waitForWaterToStabilize();			// Returns the total time in seconds
		saveStateAfterNutrient(total_time_seconds);
		set_init_water_ph();
		up_down_total = 0;
	}

	int end = 0;
	for(int j = 0; j<data_array_length && what_to_save == 0;j++)// find the end of the data_buffer so we can append
	{
		if(data_buffer[j] == '\0')
		{
			end = j;
			break;
		}
	}

	int a = 0;
	if(end<data_array_length && pH<=(min_ph+.125) && (been_written == 0 || been_written == 2) && what_to_save == 0) // do pH-up calibration if the pH is less than or equal to 4.0 and we haven't already saved pH-up dose until we reached a pH of 9.0
	{
		for(int k = end;(k < (data_array_length-27)) && (what_to_save == 0) && (pH<max_ph) ;k++)
		{
			doseWater(0, ph_calibration_dose, 0);			// add pre-defined pH-up solution (pHDown,pHup,nutrient_dose)
			float timeSeconds = 0;//waitForWaterToStabilize();  // Returns the total time in seconds for the water to stabilize
			getFiveSamples(1);								// get sensor data

			sprintf(converted_char, "%.2f", water_temp);	// convert water temp to char array and write it to the data buffer
			for(a = 0; a < (sizeof converted_char) && converted_char[a] != '\0';a++)
			{
				data_buffer[k] = converted_char[a];
				k++;
			}
			data_buffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", pH);			// convert pH to char array and write it to the data buffer
			for(a = 0; a < (sizeof converted_char)&& converted_char[a] != '\0';a++)
			{
				data_buffer[k] = converted_char[a];
				k++;
			}
			data_buffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", TDS);			// convert TDS to char array and write it to the data buffer
			for(a = 0; a < (sizeof converted_char) && converted_char[a] != '\0';a++)
			{
				data_buffer[k] = converted_char[a];
				k++;
			}
			data_buffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", timeSeconds);	// convert time in seconds to char array and write it to the data buffer
			for(a = 0; a < (sizeof converted_char) && converted_char[a] != '\0';a++)
			{
				data_buffer[k] = converted_char[a];
				k++;
			}
			data_buffer[k] = ',';
			k++;
			sprintf(converted_char, "%.2f", ph_calibration_dose);			// convert pH to char array and write it to the data buffer
			for(a = 0; a < (sizeof converted_char)&& converted_char[a] != '\0';a++)
			{
				data_buffer[k] = converted_char[a];
				k++;
			}
			data_buffer[k] = ',';
			k++;
			if(pH>=max_ph)								    // if we reached our max pH set point signify we are done and need to write a file
			{
				what_to_save = 1;					        // signify we need to write pH up data
				number_of_files++;						    // increment the total number of files that will be created
			}
		}
	}
	else if(end<data_array_length && pH>=(max_ph-.125) && (been_written == 0 || been_written == 1) && what_to_save == 0 ) // do pH-down calibration if the pH is greater than 4.0 and end at pH 4.0 and we wrote the first file or we didnt write the first file
	{
		for(int k = end;(k < (data_array_length-27)) &&  what_to_save == 0 && (pH>(min_ph)) ;k++)
		{
 			doseWater(ph_calibration_dose, 0, 0);		    // add pre-defined pH-down solution (pHDown,pHup,nutrient_dose)
			float timeSeconds = 0;//waitForWaterToStabilize();  // Returns the total time in seconds for the water to stabilize

			getFiveSamples(1);							    // get sensor data

			sprintf(converted_char, "%.2f", water_temp);    // convert water temp to char array and write it to the data buffer
			for(a = 0; a < (sizeof converted_char) && converted_char[a] != '\0';a++)
			{
				data_buffer[k] = converted_char[a];
				k++;
			}
			data_buffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", pH);		    // convert pH to char array and write it to the data buffer
			for(a = 0; a < (sizeof converted_char) && converted_char[a] != '\0';a++)
			{
				data_buffer[k] = converted_char[a];
				k++;
			}
			data_buffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", TDS);		    // convert TDS to char array and write it to the data buffer
			for(a = 0; a < (sizeof converted_char) && converted_char[a] != '\0';a++)
			{
				data_buffer[k] = converted_char[a];
				k++;
			}
			data_buffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", timeSeconds);   // convert time in seconds to char array and write it to the data buffer
			for(a = 0; a < (sizeof converted_char) && converted_char[a] != '\0';a++)
			{
				data_buffer[k] = converted_char[a];
				k++;
			}
			data_buffer[k] = ',';
			k++;
			sprintf(converted_char, "%.2f", ph_calibration_dose);			// convert pH to char array and write it to the data buffer
						for(a = 0; a < (sizeof converted_char) && converted_char[a] != '\0';a++)
						{
							data_buffer[k] = converted_char[a];
							k++;
						}
						data_buffer[k] = ',';
						k++;
			if(pH<=min_ph)								    // if we reached our min pH set point signify we are done and need to write a file
			{
				what_to_save = 2; 						    // signify we need to write pH down data
				number_of_files++; 						    // increment the total number of files that will/need to be created
			}
		}
	}
}

int write_once = 0;

void writeCalibrationData() // file refers to the document to be written (pH-down characteristcs=0, pH-up=1)
{

	if(write_once == 0)
	{
		Create_File("init_nutrient_addition.csv");								  // create the file based on the created file name
		Write_File("init_nutrient_addition.csv",initial_nutrient_buffer);	      // write the data to the file.
		write_once = 1;
	}

	int j = 0; // indexes for appending float to char array
	int k = 0;
	line_number = 0;
	if(write_times < number_of_files)
	{
		// NOTE: Following code was created because strcat() would somewhat randomly append to the beginning or end when appending short char arrays
		// We need to add the initial water parameters to their respective strings
		// find the end of the array we want to place our merge val
		// append the initial TDS to the end of the second line

		for(int i = 0; i<44; i++)							 		// copy over the text that will be appended so not to edit original and find the end
		{
			first_line_write_buf[i] = first_line[i];
			second_line_write_buf[i]  = second_line[i];
			third_line_write_buf[i] = third_line[i];
			fourth_line_write_buf[i] =  fourth_line[i];
			fifth_line_write_buf[i] =  fifth_line[i];
			if(first_line_write_buf[i]  != '\0')last_val_one   = i;// find the end of the char arrays so we can append to them
			if(second_line_write_buf[i] != '\0')last_val_two   = i;
			if(third_line_write_buf[i]  != '\0')last_val_three = i;
			if(fourth_line_write_buf[i] != '\0')last_val_four  = i;
			if(fifth_line_write_buf[i]  != '\0')last_val_five  = i;
		}

		if(line_number == 0 && write_header == 0)					// append the header to the data array(the array that will be written to the file)
		{
			strcat(calibration_data_array,header);
			write_header = 1;
		}

		for(k = 1; k<7;k++)											// append char array data to each repective line
		{
			first_line_write_buf[last_val_one+k]   = initial_TDS[k-1];
			second_line_write_buf[last_val_two+k]  = post_nutrient_TDS[k-1];
			third_line_write_buf[last_val_three+k] = initial_PH[k-1];
			fourth_line_write_buf[last_val_four+k] = post_nutrient_PH[k-1];
			fifth_line_write_buf[last_val_five+k]  = time_to_equalize[k-1];
		}

		for( j = 0; j<data_array_length; j++)					   // add the data to the data array
		{
			single_char[0] = data_buffer[j];
			strncat(calibration_data_array,single_char,1);		   // add the data one char at a time

			if(single_char[0] == ',') data_value_ready++;		   // every 4 commas is a new line i.e. we completed formatting one row of data

			if(data_value_ready == 5)							   // every 4 data points append to char array string
			{
				data_value_ready = 0;
				line_number++;
				been_appended = 0;
			}

			if(line_number == 1 && been_appended == 0)			  // append each respective line to the data array
			{
				strcat(calibration_data_array,first_line_write_buf);
				strcat(calibration_data_array,new_line);
				been_appended = 1;
			}
			else if(line_number == 2 && been_appended == 0)
			{
				strcat(calibration_data_array,second_line_write_buf);
				strcat(calibration_data_array,new_line);
				been_appended = 1;
			}
			else if(line_number == 3 && been_appended == 0)
			{
				strcat(calibration_data_array,third_line_write_buf);
				strcat(calibration_data_array,new_line);
				been_appended = 1;
			}
			else if(line_number == 4 && been_appended == 0)
			{
				//strcat(calibration_data_array,new_line);
				strcat(calibration_data_array,fourth_line_write_buf);
				strcat(calibration_data_array,new_line);
				been_appended = 1;
			}
			else if(line_number == 5 && been_appended == 0)
			{
				strcat(calibration_data_array,fifth_line_write_buf);
				strcat(calibration_data_array,new_line);
				been_appended = 1;
			}
			else if(line_number >5 && been_appended == 0)			   // if we have written the past 5 lines just add a newLine to each row of data
			{
				strcat(calibration_data_array,new_line);
				been_appended = 1;
			}
		}

		sprintf(total_nutrient_string, "%d", total_nutrient_ml_cal);	  // convert the total nutrient volume so we can create the file name

		for(int p = 0; p<50;p++)file_name_buffer[p] = '\0';			  // reset the file name buffer

		if(what_to_save == 1 && write_times < number_of_files) 		  // if we need to save the pH up data change the file name
		{
			strcat(file_name_buffer,ph_up_file_name);
			strcat(file_name_buffer,total_nutrient_string);
			strcat(file_name_buffer,file_extention);
		}
		else if(what_to_save == 2 && write_times < number_of_files)	  // if we need to save the pH down data change the file name
		{
			strcat(file_name_buffer,ph_down_file_name);
			strcat(file_name_buffer,total_nutrient_string);
			strcat(file_name_buffer,file_extention);
		}

		Create_File(file_name_buffer);								  // create the file based on the created file name
		Write_File(file_name_buffer,calibration_data_array);	      // write the data to the file.
		been_written = what_to_save;
		write_times++;
		what_to_save = 0;

		if(write_times > number_of_files) Unmount_USB();	          // if we wrote the last file unmount the USB
	}
		else return;
}

