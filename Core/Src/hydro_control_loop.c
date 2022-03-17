/*
 * hydro_control_loop.c
 *
 *  Created on: Feb 9, 2022
 *      Author: Alec
 */

#include "hydro_control_loop.h"
#include "File_Handling.h"
#include "fatfs.h"
#include "usb_host.h"
#include "stm32f2xx_hal.h"
#include "heater_driver.h"
#include "sensors.h"
#include "main.h"
double max_pH_up_dose = 5.0, max_pH_down_dose = 5.0, pH_up_dose = 0, pH_down_dose = 0, max_nutrient_dose = 100, nutrient_dose = 0, total_nutrient_ml = 0, total_pH_up_ml = 0, total_pH_down_ml = 0, total_nutrient_ml_per_file = 0, total_pH_up_ml_per_file = 0, total_pH_down_ml_per_file = 0,
		nutrient_set_point = 580.0, pH_set_point = 6.2, water_temp_set_point = 21.5, water_temp_bounds_set = 0.5,  water_temp_bounds_check = 1.0, pH_bounds_check = 0.08, pH_bounds_set = 0.03, nutrient_bounds_check = 5.0, nutrient_bounds_set = 1.5, TDS = 0, pH = 0, water_temp = 0,
		start_TDS = 0, start_pH = 0, prev_smallest_ph = 0, prev_smallest_TDS = 0, historic_largest_pH[200] = {0}, historic_smallest_pH[200] = {0}, historic_largest_TDS[200] = {0}, historic_smallest_TDS[200] = {0}, historic_average_pH[200] = {0}, historic_average_TDS[200] = {0},
		historic_average_pH_range = 0, historic_average_TDS_range = 0, historic_range_pH  = 0, historic_range_TDS = 0, average_pH = 0, average_TDS = 0,  historic_average_pH_min = 1000, historic_average_pH_max = 0, historic_average_TDS_min = 100000,  historic_average_TDS_max = 0,
		historic_TDS_max = 0, historic_pH_max  = 0, historic_pH_min  = 0, historic_TDS_min  = 0, slope_factor_average_TDS = 0, slope_factor_average_ph = 0, sample_array_TDS[30] = {0}, sample_array_pH[30] = {0}, smallest_value_TDS = 100000, largest_value_TDS = 0, smallest_value_pH = 100,
		largest_value_pH = 0, prev_largest_pH = 0, prev_largest_TDS = 0, TDS_range = 0,	pH_range = 0, TDS_avg_check = 0, pH_avg_check = 0, fpnumber;

int balance_index = 0, i = 0, time_to_bal_nutrient = 0, time_to_bal_pH = 0, file_number = 0, file_index = 0, num_of_stable_runs = 0, num_sensor_samples = 15, data_array_length = 10000, write_times = 0, number_of_files = 1, total_runs = 0, eq_end_time_sec = 0, eq_start_time_sec = 0,
		stability_value = 1, start_time_seconds = 0, end_time_seconds = 0, total_time_seconds = 0, sample_index1 = 0, recheck_count = 0, valid = 0, historic_sample_index = 0, number_historic_samples = 200, run_again = 1;

char file_name[25] = "data_", extention[5] = ".csv", buffer[25] = {0}, convertedString[10] = {0}, waiting_to_write = 'n', balance_data[5000] = {0}, get_init_conditions = 'n', pH_init_title[15] = "Initial pH:", pH_set_point_title[15] = "pH set point:", TDS_init_title[15] = "Initial TDS:",
		TDS_set_point_title[15] = "TDS set point:",	balance_header[]  = "pH_after_dose, total_pH_up_ml, pH_Up_dose_ml, total_pH_down_ml, pH_down_dose_ml, time_to_bal_pH, TDS_after_dose, total_nutrient_ml, TDS_dose_ml, time_to_bal_nutrient, error, water_temp\n", setting_pH = 'n',
		setting_nutrient = 'n', heat_on = 'n', cool_on = 'n', setting_water_temp = 'n', run_once = 'n', error = 'n', pH_up = 'n', pH_down = 'n', nutrient_up = 'n', write_header = 'n', temp_up = 'n', temp_down = 0;

long int befdec, aftdec;

void floatToString(double FP_NUM)
{
	fpnumber = FP_NUM;					// Fractional part is truncated
	befdec = fpnumber;                  // 12.163456 becomes 12
	aftdec = fpnumber * 100;            // 12.163456 becomes 1216
	aftdec = aftdec - (befdec * 100);   // 1216 - 1200 = 16
	if (fpnumber < 1)
	{
		convertedString[0] = '0';
		convertedString[1] = '.';
		convertedString[2] = (aftdec/10) + 48;
		convertedString[3] = (aftdec/1)%10 + 48;
		convertedString[4] = ' ';
		convertedString[5] = ' ';
		convertedString[6] = ' ';
		convertedString[7] = '\0';
	}
	else if ((fpnumber >= 1) && (fpnumber < 10))
	{
		convertedString[0] = (befdec/1)%10 + 48;
		convertedString[1] = '.';
		convertedString[2] = (aftdec/10) + 48;
		convertedString[3] = (aftdec/1)%10 + 48;
		convertedString[4] = ' ';
		convertedString[5] = ' ';
		convertedString[6] = ' ';
		convertedString[7] = '\0';
	}
	else if ((fpnumber >= 10) && (fpnumber < 100))
	{
		convertedString[0] = (befdec/10) + 48;
		convertedString[1] = (befdec/1)%10 + 48;
		convertedString[2] = '.';
		convertedString[3] = (aftdec/10) + 48;
		convertedString[4] = (aftdec/1)%10 + 48;
		convertedString[5] = ' ';
		convertedString[6] = ' ';
		convertedString[7] = '\0';
	}
	else if ((fpnumber >= 100) && (fpnumber < 1000))
	{
		convertedString[0] = (befdec/100) + 48;
		convertedString[1] = (befdec/10)%10 + 48;
		convertedString[2] = (befdec/1)%10 + 48;
		convertedString[3] = '.';
		convertedString[4] = (aftdec/10) + 48;
		convertedString[5] = ' ';
		convertedString[6] = ' ';
		convertedString[7] = '\0';
	}
	else if ((fpnumber >= 1000) && (fpnumber < 10000))
	{
		convertedString[0] = (befdec/1000) + 48;
		convertedString[1] = (befdec/100)%10 + 48;
		convertedString[2] = (befdec/10)%10 + 48;
		convertedString[3] = (befdec/1)%10 + 48;
		convertedString[4] = '.';
		convertedString[5] = (aftdec/10) + 48;
		convertedString[6] = ' ';
		convertedString[7] = '\0';
	}
	else if ((fpnumber >= 10000) && (fpnumber < 100000))
	{
		convertedString[0] = (befdec/10000) + 48;
		convertedString[1] = (befdec/1000)%10 + 48;
		convertedString[2] = (befdec/100)%10 + 48;
		convertedString[3] = (befdec/10)%10 + 48;
		convertedString[4] = (befdec/1)%10 + 48;
		convertedString[5] = '.';
		convertedString[6] = (aftdec/10) + 48;
		convertedString[7] = '\0';
	}
}

void write_balance_data_file(char buffer_full)
{

	for(file_index = 0; file_index<16 && file_name[file_index] != '\0'; file_index++) buffer[file_index] = file_name[file_index];

	floatToString((int)file_number);

	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0' && convertedString[i] != '.';i++,file_index++)buffer[file_index] = convertedString[i];

	if(buffer_full == 'y')
	{
		buffer[file_index] = 'B';
		file_index++;
	}

	for(i=0;i<4 && extention[i]!='\0';i++,file_index++) buffer[file_index]=extention[i];

	Create_File(buffer);								  // create the file based on the created file name
	Write_File(buffer,balance_data);	      // write the data to the file.
	balance_index = 0;
	write_header  = 'n';
	get_init_conditions = 0;
	file_number++;

	for(i=0;i<10000 && balance_data[i]!='\0';i++)balance_data[i] = '\0';
	for(i=0;i<11 && buffer[i]!='\0';i++)buffer[i] = '\0';
}

void add_data_to_array()
{
	if(write_header == 'n')
	{
		for(i = 0;i < (sizeof balance_header) && balance_header[i] != '\0';i++)
		{
			balance_data[balance_index] = balance_header[i];
			balance_index++;
		}
		write_header = 'y';
	}
	// CSV file data format: pH, total_pH_up_ml, total_pH_down_ml, time_to_bal_pH, TDS, total_nutrient_ml, time_to_bal_nutrient, error, water_temp
	floatToString((double)pH);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	floatToString((double)total_pH_up_ml);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	floatToString((double)pH_up_dose);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;


	floatToString((double)total_pH_down_ml);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	floatToString((double)pH_down_dose);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;


	floatToString(time_to_bal_pH);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	floatToString( (double)TDS);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	floatToString((double)total_nutrient_ml);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	floatToString( (double)nutrient_dose);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	floatToString(time_to_bal_nutrient);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	balance_data[balance_index] = error;
	balance_index++;
	balance_data[balance_index] = ',';
	balance_index++;

	floatToString((double)water_temp);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;
	balance_data[balance_index] = '\n';
	balance_index++;
}

void appendInitialConditions()
{
	for(i = 0; i < 17 && pH_init_title[i] != '\0';i++)						// append init pH title to the data array
	{
		balance_data[balance_index] = pH_init_title[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;
	floatToString( pH);									// convert pH to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)	// append pH to our data array
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	for(i = 0; i < 17 && pH_set_point_title[i] != '\0';i++)							// append init pH set point title to the data array
	{
		balance_data[balance_index] = pH_set_point_title[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;
	floatToString( pH_set_point);									// convert pH set point to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)			// append pH set point char to the data array
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = '\n';
	balance_index++;

	for(i = 0; i < 17 && TDS_init_title[i] != '\0';i++)						// append init pH title to the data array
	{
		balance_data[balance_index] = TDS_init_title[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;
	floatToString( TDS);									// convert pH to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)	// append pH to our data array
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	for(i = 0; i < 17 && TDS_set_point_title[i] != '\0';i++)							// append init pH set point title to the data array
	{
		balance_data[balance_index] = TDS_set_point_title[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;
	floatToString( nutrient_set_point);									// convert pH set point to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)			// append pH set point char to the data array
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
	balance_data[balance_index] = '\n';
	balance_index++;
    get_init_conditions = 'y';
}

void balancePhAndNutrient()
{
	TDS_avg_check = 0;
	pH_avg_check = 0;
	if(waiting_to_write == 'n')	getSensorValues();

	pH_up = 'n';
	pH_down = 'n';
	nutrient_up = 'n';	// check twice to see if we need to add nutrient

	if(setting_pH == 'n')	// if we are not changing the pH or nutrient level, check to see if we are out of bounds
	{
		if(     pH  > pH_set_point     &&     (pH - pH_bounds_check) > pH_set_point)
		{
			pH_down = calibrateDosage('d', pH);		// if we are over our set point dose the water with pH-down
		}
		else if(pH  < pH_set_point 	   &&     (pH + pH_bounds_check) < pH_set_point)
		{
			pH_up 	= calibrateDosage('u',pH); 				// if we are under our set point dose the water with pH-up
		}
	}
	else	// else we are setting the pH so reduce the pH bounds to accurately set the value
	{
		if(     pH  > pH_set_point     &&     (pH - pH_bounds_set) > pH_set_point)   				pH_down = 'y'; 			// if we are over our set point dose the water with pH-down
		else if(pH  < pH_set_point 	   &&     (pH + pH_bounds_set) < pH_set_point)  				pH_up 	= 'y'; 				// if we are under our set point dose the water with pH-up
	}
	if(setting_nutrient == 'n')	// if we are not changing the pH or nutrient level, check to see if we are out of bounds
	{
		if(     TDS > nutrient_set_point && (TDS - nutrient_bounds_check) > nutrient_set_point) 	error = 'y'; 				 // if we are over our TDS set point ERROR
		else if(TDS < nutrient_set_point && (TDS + nutrient_bounds_check) < nutrient_set_point )
		{
			nutrient_up = calibrateDosage('n',pH);// if we checked twice and we still need to dose nutrients then go for it.
		}
	}
	else if(TDS < nutrient_set_point && (TDS + nutrient_bounds_set) < nutrient_set_point) nutrient_up = 'y';		 // if we are under our set point dose the water with pH-down

	if(get_init_conditions == 'n') appendInitialConditions();

	if((pH_down == 'y' || pH_up == 'y' || nutrient_up == 'y'))				// if we are adding pH-up/down or nutrient, signify what we are setting so we can change the accuracy range
	{
		if(nutrient_up == 'y') 			setting_nutrient = 'y';
		else setting_nutrient = 'n';

		if(pH_down == 'y' || pH_up == 'y')  setting_pH = 'y';
		else setting_pH = 'n';
		num_of_stable_runs = 0;
	}
	else if(pH_down == 'n' && pH_up == 'n' && nutrient_up == 'n')				// else if we are not setting anything, signify that
	{
		setting_nutrient = 'n';
		setting_pH = 'n';
	}

	if(num_of_stable_runs >= 0 && num_of_stable_runs < 7 && ((10000 - balance_index) >= 500) && waiting_to_write == 'n' ) // consider changing to 10
	{
		time_to_bal_pH = 0;
		time_to_bal_nutrient = 0;

		if(nutrient_up == 'y')	// if we need to add nutrients and we also need to adjust the pH, add the nutrients first and wait to stabilize then add pH-up/down
		{
			nutrient_dose = calcNutrientDose(pH_set_point, nutrient_set_point);
			doseWater(0,0,nutrient_dose);
			total_nutrient_ml += nutrient_dose;
			total_nutrient_ml_per_file += nutrient_dose;
			time_to_bal_nutrient = waitForWaterToStabilize();

			getSensorValues();	// after adding nutrient check the pH again to see if it needs to be adjustedgetSensorValues

			nutrient_up = 'n';
			pH_down = 'n';
			pH_up 	= 'n';
			if(     pH  > pH_set_point     &&     (pH - pH_bounds_check) > pH_set_point)
			{
				pH_down = calibrateDosage('d',pH);
			}
			else if(pH  < pH_set_point 	   &&     (pH + pH_bounds_check) < pH_set_point)
			{
				pH_up = calibrateDosage('u',pH);
			}

			if(pH_down == 'y')
			{
				pH_down_dose = calcPhDownDose(pH_set_point, nutrient_set_point);
				doseWater(pH_down_dose,0,0);
				total_pH_down_ml += pH_down_dose;
				total_pH_down_ml_per_file += pH_down_dose;
				pH_down = 'n';
				time_to_bal_pH = waitForWaterToStabilize();
			}
			else if(pH_up == 'y')
			{
				pH_up_dose = calcPhUpDose(pH_set_point, nutrient_set_point);
				doseWater(0,pH_up_dose,0);
				total_pH_up_ml += pH_up_dose;
				total_pH_up_ml_per_file+= pH_up_dose;
				pH_up = 'n';
				time_to_bal_pH = waitForWaterToStabilize();
			}
		}
		else if(pH_down == 'y')
		{
			pH_down_dose = calcPhDownDose(pH_set_point, nutrient_set_point);
			doseWater(pH_down_dose,0,0);
			total_pH_down_ml += pH_down_dose;
			total_pH_down_ml_per_file += pH_down_dose;
			pH_down = 'n';
			time_to_bal_pH = waitForWaterToStabilize();
		}
		else if(pH_up == 'y')
		{
			pH_up_dose = calcPhUpDose(pH_set_point, nutrient_set_point);
			doseWater(0,pH_up_dose,0);
			total_pH_up_ml += pH_up_dose;
			total_pH_up_ml_per_file += pH_up_dose;
			pH_up = 'n';
			time_to_bal_pH = waitForWaterToStabilize();
		}
		osDelay(500);
		num_of_stable_runs++;
		add_data_to_array();
		pH_up_dose = 0;
		pH_down_dose = 0;
		nutrient_dose = 0;
	}
	else if(num_of_stable_runs>=7 && waiting_to_write == 'n' && (total_pH_up_ml_per_file > 0 || total_pH_down_ml_per_file > 0 || total_nutrient_ml_per_file > 0) && setting_nutrient =='n' && setting_pH == 'n')
	{
		waiting_to_write = 'y';
		total_pH_down_ml_per_file = 0;
		total_pH_up_ml_per_file = 0;
		total_nutrient_ml_per_file = 0;
	}
	if(usb_good == 1 && num_of_stable_runs >= 7 && waiting_to_write == 'y')
	{
		write_balance_data_file('n');
		num_of_stable_runs = 0;
		balance_index = 0;
		get_init_conditions  = 'n';
		waiting_to_write = 'n';
		resetStabilityVars();
		for(int a = 0; a < (sizeof balance_data); a++)	// reset the data buffer since we just wrote the data
		{
			balance_data[a]   = '\0';
			if(a<25)buffer[a] = '\0';
		}
	}
	else if(usb_good == 1 && ((5000 - balance_index) < 500)) write_balance_data_file('y'); // if the buffer is full write the file
}

void resetStabilityVars()
{
	average_pH = 0;
	average_TDS = 0;
	run_again = 1; 		// set to one its the first run 2 is multiple 0 is do not run again
	valid = 0;
	historic_sample_index = 0;
	slope_factor_average_TDS = 0;
	slope_factor_average_ph = 0;

	for(int h = 0; h<200; h++)
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

	getSensorValues();

	smallest_value_TDS = 10000;							   // set smallest values to value much higher than expected
	largest_value_TDS = 0;	  							   // set largest to the smallest possible value these steps ensure we catch error cases
	smallest_value_pH = 100;
    largest_value_pH = 0;


    average_pH = 0;
    average_TDS = 0;
	for(int i = 0; i<num_sensor_samples;i++)
	{
		if(sample_array_TDS[i] > largest_value_TDS  && sample_array_TDS[i] != 0)  largest_value_TDS  = sample_array_TDS[i];	// find largest and smallest values in our data
		if(sample_array_TDS[i] < smallest_value_TDS && sample_array_TDS[i] != 0) smallest_value_TDS = sample_array_TDS[i];
		if(sample_array_pH[i]  > largest_value_pH && sample_array_pH[i] != 0) 	 largest_value_pH   = sample_array_pH[i];
		if(sample_array_pH[i]  < smallest_value_pH && sample_array_pH[i] != 0)   smallest_value_pH  = sample_array_pH[i];
		if( sample_array_TDS[i] != 0 && sample_array_pH[i] != 0)
		{
			average_pH  += sample_array_pH[i];
			average_TDS += sample_array_TDS[i];
			sample_index1++;
		}
	}

	TDS_range = largest_value_TDS - smallest_value_TDS; // calculate the range in the samples i.e. (largest valve)-(smallest value)
	pH_range  = largest_value_pH  - smallest_value_pH;

	historic_TDS_max = 0;
	historic_TDS_min = 100000;
	historic_pH_max  = 0;
	historic_pH_min  = 100000;

	average_pH  = average_pH/(sample_index1);
	average_TDS = average_TDS/(sample_index1);
	sample_index1 = 0;
	if(historic_sample_index < number_historic_samples)
	{
		historic_largest_pH[historic_sample_index]   = largest_value_pH;
		historic_smallest_pH[historic_sample_index]  = smallest_value_pH;
		historic_largest_TDS[historic_sample_index]  = largest_value_TDS;
		historic_smallest_TDS[historic_sample_index] = smallest_value_TDS;
		historic_average_pH[historic_sample_index]   = average_pH;
		historic_average_TDS[historic_sample_index]  = average_TDS;
		historic_sample_index++;
	}
	else
	{
		resetStabilityVars();
		run_again = 2;
	}

	if(historic_sample_index > 20)
	{
		historic_average_pH_min = 1000;
		historic_average_pH_max = 0;
		historic_average_TDS_min = 100000;
		historic_average_TDS_max = 0;
		smallest_value_TDS = 100000;							   // set smallest values to value much higher than expected
		largest_value_TDS = 0;	  							   // set largest to the smallest possible value these steps ensure we catch error cases
		smallest_value_pH = 100;
		largest_value_pH = 0;
		slope_factor_average_TDS = 0;
		slope_factor_average_ph = 0;
		for(int k = historic_sample_index-20; k<historic_sample_index; k++) 								// if we have at-least 5 samples find the max and min to calculate range of the last 5 runs
		{
			if(historic_largest_TDS[k]  > historic_TDS_max)   		historic_TDS_max 		  = historic_largest_TDS[k];	// find largest and smallest valuesin our data
			if(historic_smallest_TDS[k] < historic_TDS_min)  		historic_TDS_min		  = historic_smallest_TDS[k];
			if(historic_largest_pH[k]   > historic_pH_max )     	historic_pH_max  		  = historic_largest_pH[k];
			if(historic_smallest_pH[k]  < historic_pH_min )    		historic_pH_min  		  = historic_smallest_pH[k];
			if(historic_average_pH[k]   < historic_average_pH_min)  historic_average_pH_min   = historic_average_pH[k];
			if(historic_average_pH[k]   > historic_average_pH_max)  historic_average_pH_max   = historic_average_pH[k];
			if(historic_average_TDS[k]  < historic_average_TDS_min) historic_average_TDS_min  = historic_average_TDS[k];
			if(historic_average_TDS[k]  > historic_average_TDS_max) historic_average_TDS_max  = historic_average_TDS[k];

			slope_factor_average_TDS += (historic_average_TDS[k]-historic_average_TDS[k-1]);
			slope_factor_average_ph  += (historic_average_pH[k]-historic_average_pH[k-1]);
		}
		historic_range_pH  = historic_pH_max  - historic_pH_min;
		historic_range_TDS = historic_TDS_max - historic_TDS_min;
		historic_average_pH_range  = historic_average_pH_max  - historic_average_pH_min;
		historic_average_TDS_range = historic_average_TDS_max - historic_average_TDS_min;
	}

	if(run_again != 2 && ( slope_factor_average_TDS > 1.5 ||  slope_factor_average_TDS < -1.5 || slope_factor_average_ph > 0.04 || slope_factor_average_ph < -0.04 || historic_sample_index <= 20 || TDS_range > 10.5 || pH_range > 0.25 || historic_average_pH_range > 0.05 || historic_average_pH_range < -0.05  || historic_range_pH > 0.30 || historic_range_pH < -0.30 || historic_range_TDS > 10 || historic_range_TDS < -10 || historic_average_TDS_range > 4.0 || historic_average_TDS_range < -4.0))
	{
 		valid = 0;
 		recheck_count++;
 		run_again = 2; 				// if the ph or TDS values are still changing check again, or the smallest value is at the beginning of the array, or the largest value is at the end of the array
	}
	else if(run_again == 1)		    // we are stabilized however, the system could still be changing so we need to check how many times we re-sampled the water if its > 1 check until its 1
	{
		run_again = 0; 				// set run again to 0(no) however, if our validity is not high enough we will set run_again to 2(yes)
		valid++;
		if(valid < 10) run_again = 2;			// if we have not completed three valid runs in a row re-run
	}
	if(run_again == 1) run_again = 0;
	else if (run_again == 2) run_again = 1;
}

int waitForWaterToStabilize() // Returns the total time in seconds
{
	getTime();
	eq_start_time_sec = sTime.Seconds + sTime.Minutes*60;

	while(run_again > 0)
	{
		isStabalized();// if we are not stabilized wait until we are
		osDelay(2000);
	}
	getTime();
	eq_end_time_sec = sTime.Seconds + sTime.Minutes*60;
	if(eq_start_time_sec > eq_end_time_sec) total_time_seconds = (3600-eq_start_time_sec)+eq_end_time_sec;	// if seconds rolled over we cannot just subtract since we will get a negative
	else total_time_seconds = eq_end_time_sec-eq_start_time_sec;	// else we did not roll over so total seconds is end time - start time

	stability_value = 0;
	recheck_count = 0;
	resetStabilityVars();
	return (total_time_seconds);
}

void waterTempControl()
{
	temp_up = 'n';
	temp_down = 'n';

	if(setting_water_temp == 'n')	// if we are not changing the pH or nutrient level, check to see if we are out of bounds
	{
		if(     water_temp  > water_temp_set_point     &&     (water_temp - water_temp_bounds_check) > water_temp_set_point)   				temp_down = 'y'; 			// if we are over our set point dose the water with pH-down
		else if(water_temp  < water_temp_set_point 	   &&     (water_temp + water_temp_bounds_check) < water_temp_set_point)  				temp_up = 'y'; 				// if we are under our set point dose the water with pH-up
	}
	else	// else we are setting the pH so reduce the pH bounds to accurately set the value
	{
		if(     water_temp  > water_temp_set_point     &&     (water_temp - water_temp_bounds_set) > water_temp_set_point)   				temp_down = 'y'; 			// if we are over our set point dose the water with pH-down
		else if(water_temp  < water_temp_set_point 	   &&     (water_temp + water_temp_bounds_set) < water_temp_set_point)  				temp_up = 'y'; 				// if we are under our set point dose the water with pH-up
	}

	if((temp_up == 'y' || temp_down == 'y'))				// if we are adding pH-up/down or nutrient, signify what we are setting so we can change the accuracy range
	{
		setting_water_temp = 'y';
	}
	else setting_water_temp = 'n';

	if(temp_up == 'y' && heat_on == 'n')
	{
		heatOn();	// if we need heat the water turn on the heater
		setFanSpeed(3.5,3.5,3.5);
		heat_on = 'y';
		cool_on = 'n';
	}
	else if(temp_down == 'y' && cool_on == 'n')
	{
		coolOn();	// if we need cool the water turn on the cooler
		setFanSpeed(3.5,3.5,2.5);
		cool_on = 'y';
		heat_on = 'n';
	}
	else if(temp_up == 'n' && temp_down == 'n')
	{
		heat_on = 'n';
		cool_on = 'n';
		heatCoolOff();
		setFanSpeed(3.5,3.5,0);
	}
}

void systemControl()
{

	if(run_once == 'n')
	{
		//doseWater(100,100,100);
		run_once = 'y';
		fanOn();
		setFanSpeed(3.5,3.5,0);
		setTimeDate(0x01, 0x08, 0x22, 0x19, 0x09, 0x00); // MUST BE HEX BUT NOT CONVERTED i,e,(the 22 day of the month is represented as 0x22 NOT 0x16) (month, day, year, hours, min, sec)
		setLightCyle(19, 9, 19, 10); 			   		 // MUST BE INT (start hour, start min, start sec, end hour, end min)
		//doseWater(100,100,100);
	}
	getSensorValues();
	balancePhAndNutrient();
}

