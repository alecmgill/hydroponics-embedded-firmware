/*
 * hydro_control_loop.c
 *
 *  Created on: Feb 9, 2022
 *      Author: Alec
 */

#include "hydro_control_loop.h"

#include "stdio.h"
#include "stdlib.h"
#include "File_Handling.h"
#include "fatfs.h"
#include "usb_host.h"
#include "stm32f2xx_hal.h"

double max_pH_up_dose = 5.0, max_pH_down_dose = 5.0, pH_up_dose = 0, pH_down_dose = 0, max_nutrient_dose = 80, nutrient_dose = 0, total_nutrient_ml = 0, total_pH_up_ml = 0, total_pH_down_ml = 0, total_nutrient_ml_per_file = 0, total_pH_up_ml_per_file = 0, total_pH_down_ml_per_file = 0;
float nutrient_set_point = 185.0, pH_set_point = 6.0, water_temp_set_point = 20.0, pH_bounds_check = 0.10, pH_bounds_set = 0.05, nutrient_bounds_check = 15, nutrient_bounds_set = 7.5, TDS = 0, pH = 0, water_temp = 0;
char balance_data[50000] = {0};
int run_once = 1, error = 0, balance_index = 0, i = 0;
int time_to_bal_nutrient = 0;
int time_to_bal_pH = 0;
int pH_up = 0;
int pH_down = 0;
int nutrient_up = 0;
//char convertedString[7] = {0};
int num_of_stable_runs = 0;
char waiting_to_write = 'n';
extern void calibrateSys();
extern int  waitForWaterToStabilize();
char get_init_conditions = 'n';

double total_prev_pH_and_nutrient_ml = 0;
int file_number = 0;
int file_index = 0;
char file_name[25] = "data_";
char extention[5] = ".csv";
char buffer[25] = {0};
char convertedString[10] = {0};

void floatToString(double FP_NUM) {
        double fpnumber;
        long int befdec, aftdec;

        fpnumber = FP_NUM;

        befdec = fpnumber;                      // Fractional part is truncated
                                                // 12.163456 becomes 12
        aftdec = fpnumber * 100;            // 12.163456 becomes 1216
        aftdec = aftdec - (befdec * 100);   // 1216 - 1200 = 16


        if (fpnumber < 1) {
                convertedString[0] = '0';
                convertedString[1] = '.';
                convertedString[2] = (aftdec/10) + 48;
                convertedString[3] = (aftdec/1)%10 + 48;
                convertedString[4] = ' ';
                convertedString[5] = ' ';
                convertedString[6] = ' ';
                convertedString[7] = '\0';

        }

        else if ((fpnumber >= 1) && (fpnumber < 10)) {
                convertedString[0] = (befdec/1)%10 + 48;
                convertedString[1] = '.';
                convertedString[2] = (aftdec/10) + 48;
                convertedString[3] = (aftdec/1)%10 + 48;
                convertedString[4] = ' ';
                convertedString[5] = ' ';
                convertedString[6] = ' ';
                convertedString[7] = '\0';

        }

        else if ((fpnumber >= 10) && (fpnumber < 100)) {
                convertedString[0] = (befdec/10) + 48;
                convertedString[1] = (befdec/1)%10 + 48;
                convertedString[2] = '.';
                convertedString[3] = (aftdec/10) + 48;
                convertedString[4] = (aftdec/1)%10 + 48;
                convertedString[5] = ' ';
                convertedString[6] = ' ';
                convertedString[7] = '\0';

        }

        else if ((fpnumber >= 100) && (fpnumber < 1000)) {
                convertedString[0] = (befdec/100) + 48;
                convertedString[1] = (befdec/10)%10 + 48;
                convertedString[2] = (befdec/1)%10 + 48;
                convertedString[3] = '.';
                convertedString[4] = (aftdec/10) + 48;
                convertedString[5] = ' ';
                convertedString[6] = ' ';
                convertedString[7] = '\0';

        }

        else if ((fpnumber >= 1000) && (fpnumber < 10000)) {
                convertedString[0] = (befdec/1000) + 48;
                convertedString[1] = (befdec/100)%10 + 48;
                convertedString[2] = (befdec/10)%10 + 48;
                convertedString[3] = (befdec/1)%10 + 48;
                convertedString[4] = '.';
                convertedString[5] = (aftdec/10) + 48;
                convertedString[6] = ' ';
                convertedString[7] = '\0';

        }

        else if ((fpnumber >= 10000) && (fpnumber < 100000)) {
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

void write_file()
{
	for(file_index = 0; file_index<16 && file_name[file_index] != '\0'; file_index++)
	{
		buffer[file_index] = file_name[file_index];
	}
	floatToString((int)file_number);
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0' && convertedString[i] != '.';i++,file_index++)
	{
		buffer[file_index] = convertedString[i];
	}


	//buffer[file_index] = convertedString[file_index];
	//file_index++;

	for(i=0;i<4 && extention[i]!='\0';i++,file_index++)buffer[file_index]=extention[i];


	Create_File(buffer);								  // create the file based on the created file name
	Write_File(buffer,balance_data);	      // write the data to the file.
	file_number++;

}

int one_run = 0;
double start_TDS = 0;
double start_pH = 0;

char balance_header[] = "pH_after_dose, total_pH_up_ml, pH_Up_dose_ml, total_pH_down_ml, pH_down_dose_ml, time_to_bal_pH, TDS_after_dose, total_nutrient_ml, TDS_dose_ml, time_to_bal_nutrient, error, water_temp\n";
void add_data_to_array()
{

	if(one_run == 0)
	{
		for(i = 0;i < (sizeof balance_header) && balance_header[i] != '\0';i++)
		{
			balance_data[balance_index] = balance_header[i];
			balance_index++;
		}
		one_run = 1;
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

	floatToString( error);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof convertedString) && convertedString[i] != '\0';i++)
	{
		balance_data[balance_index] = convertedString[i];
		balance_index++;
	}
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


char pH_init_title[17] = "Initial pH:";
char pH_set_point_title[17] = "pH set point:";
char TDS_init_title[17] = "Initial TDS:";
char TDS_set_point_title[17] = "TDS set point:";

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

char setting_pH = 'n';
char setting_nutrient = 'n';

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
			 osDelay(100);
		}
	}
	water_temp	  = water_temp/5;							    // get sensor data so we can tell if its time to do pH up or pH down
	TDS 		  = TDS/5;
	pH 		 	  = pH/5;
}
void balancePhAndNutrient()
{
	if(waiting_to_write == 'n') getFiveSamples(1);
	pH_up = 0;
	pH_down = 0;
	nutrient_up = 0;

	if(setting_pH == 'n')	// if we are not changing the pH or nutrient level, check to see if we are out of bounds
	{
		if(     pH  > pH_set_point     &&     (pH - pH_bounds_check) > pH_set_point)   				pH_down = 1; 			// if we are over our set point dose the water with pH-down
		else if(pH  < pH_set_point 	   &&     (pH + pH_bounds_check) < pH_set_point)  				pH_up = 1; 				// if we are under our set point dose the water with pH-up
	}
	else	// else we are setting the pH so reduce the pH bounds to accurately set the value
	{
		if(     pH  > pH_set_point     &&     (pH - pH_bounds_set) > pH_set_point)   				pH_down = 1; 			// if we are over our set point dose the water with pH-down
		else if(pH  < pH_set_point 	   &&     (pH + pH_bounds_set) < pH_set_point)  				pH_up = 1; 				// if we are under our set point dose the water with pH-up
	}
	if(setting_nutrient == 'n')	// if we are not changing the pH or nutrient level, check to see if we are out of bounds
	{
		if(     TDS > nutrient_set_point && (TDS - nutrient_bounds_check) > nutrient_set_point) 	error = 1; 				 // if we are over our TDS set point ERROR
		else if(TDS < nutrient_set_point && (TDS + nutrient_bounds_check) < nutrient_set_point ) 	nutrient_up = 1;		 // if we are under our set point dose the water with pH-down
	}
	else if(TDS < nutrient_set_point && (TDS + nutrient_bounds_set) < nutrient_set_point) nutrient_up = 1;		 // if we are under our set point dose the water with pH-down

	if(get_init_conditions == 'n' && (pH_down == 1 || pH_up == 1 || nutrient_up == 1)) appendInitialConditions();

	if((pH_down == 1 || pH_up == 1 || nutrient_up == 1))				// if we are adding pH-up/down or nutrient, signify what we are setting so we can change the accuracy range
	{
		if(nutrient_up == 1) 			setting_nutrient = 'y';
		else setting_nutrient = 'n';

		if(pH_down == 1 || pH_up == 1)  setting_pH = 'y';
		else setting_pH = 'n';
		num_of_stable_runs = 0;
	}
	else if(pH_down == 0 && pH_up == 0 && nutrient_up == 0)				// else if we are not setting anything, signify that
	{
		setting_nutrient = 'n';
		setting_pH = 'n';
	}

	if(num_of_stable_runs >= 0 && num_of_stable_runs <= 5) // consider changing to 10
	{
		time_to_bal_pH = 0;
		time_to_bal_nutrient = 0;

		if(nutrient_up == 1)	// if we need to add nutrients and we also need to adjust the pH, add the nutrients first and wait to stabilize then add pH-up/down
		{
			nutrient_dose = calcNutrientDose(pH_set_point, nutrient_set_point);

			doseWater(0,0,nutrient_dose);
			total_nutrient_ml += nutrient_dose;
			total_nutrient_ml_per_file += nutrient_dose;
			time_to_bal_nutrient = waitForWaterToStabilize();
			nutrient_up = 0;

			getFiveSamples(1);	// after adding nutrient check the pH again to see if it needs to be adjusted
			pH_down = 0;
			pH_up 	= 0;

			if(     pH  > pH_set_point     &&     (pH - pH_bounds_check) > pH_set_point)   		pH_down = 1;  // if we are over our set point dose the water with pH-down
			else if(pH  < pH_set_point 	   &&     (pH + pH_bounds_check) < pH_set_point)  		pH_up   = 1;  // if we are under our set point dose the water with pH-up

			if(pH_down == 1)
			{
				pH_down_dose = calcPhDownDose(pH_set_point, nutrient_set_point);
				doseWater(pH_down_dose,0,0);
				total_pH_down_ml += pH_down_dose;
				total_pH_down_ml_per_file += pH_down_dose;
				pH_down = 0;
				time_to_bal_pH = waitForWaterToStabilize();
			}
			else if(pH_up == 1)
			{
				pH_up_dose = calcPhUpDose(pH_set_point, nutrient_set_point);
				doseWater(0,pH_up_dose,0);
				total_pH_up_ml += pH_up_dose;
				total_pH_up_ml_per_file+= pH_up_dose;
				pH_up = 0;
				time_to_bal_pH = waitForWaterToStabilize();
			}
		}
		else if(pH_down == 1)
		{
			pH_down_dose = calcPhDownDose(pH_set_point, nutrient_set_point);
			doseWater(pH_down_dose,0,0);
			total_pH_down_ml += pH_down_dose;
			total_pH_down_ml_per_file += pH_down_dose;
			pH_down = 0;
			time_to_bal_pH = waitForWaterToStabilize();
		}
		else if(pH_up == 1)
		{
			pH_up_dose = calcPhUpDose(pH_set_point, nutrient_set_point);
			doseWater(0,pH_up_dose,0);
			total_pH_up_ml += pH_up_dose;
			total_pH_up_ml_per_file += pH_up_dose;
			pH_up = 0;
			time_to_bal_pH = waitForWaterToStabilize();
		}
		 osDelay(500);
		num_of_stable_runs++;
		add_data_to_array();
		pH_up_dose = 0;
		pH_down_dose = 0;
		nutrient_dose = 0;
	}
	else if(num_of_stable_runs>=6 && waiting_to_write == 'n' && (total_pH_up_ml_per_file > 0 || total_pH_down_ml_per_file > 0 || total_nutrient_ml_per_file > 0) && setting_nutrient =='n' && setting_pH == 'n')
	{
		waiting_to_write = 'y';
		total_pH_down_ml_per_file = 0;
		total_pH_up_ml_per_file = 0;
		total_nutrient_ml_per_file = 0;
	}
	if(usb_good == 1 && num_of_stable_runs >= 6 && waiting_to_write == 'y')//if(usb_good == 1 && (what_to_save < 3) && (what_to_save != 0))
	{
		write_file();
		num_of_stable_runs = 0;
		balance_index = 0;
		get_init_conditions  = 'n';
		waiting_to_write = 'n';
		for(int a = 0; a < (sizeof balance_data); a++)	// reset the data buffer since we just wrote the data
		{
			balance_data[a] = '\0';
			if(a<25)buffer[a] = '\0';
		}
	}
}

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
int valid = 0;
float sample_array_TDS[30] = {0}, sample_array_pH[30] = {0}, smallest_value_TDS = 100000, largest_value_TDS = 0, smallest_value_pH = 100, largest_value_pH = 0, prev_largest_pH = 0, prev_largest_TDS = 0, TDS_range = 0,
		pH_range = 0, minimum_sec_duration = 36;

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
int data_array_length = 10000, write_times = 0, been_written = 0, number_of_files = 1, what_to_save = 0, total_nutrient_ml_cal = 0, write_header = 0, total_runs = 0, eq_end_time_sec = 0, eq_start_time_sec = 0,
		stability_value = 1, initial_run = 1, start_time_seconds = 0, end_time_seconds = 0, total_time_seconds = 0, max_runs = 0, previous_written = 0, up_down_total = 0;
float prev_smallest_ph = 0;
float prev_smallest_TDS = 0;
void isStabalized()  // will take a few samples of the waters pH and TDS to determine if the solution has stabilized. If stabilized returns one else 0
{		// get the current system time sets global variables sTime and sDate in RTC_driver.c


	end_time_seconds = 0;
	prev_smallest_ph = smallest_value_pH;
	prev_smallest_TDS = smallest_value_TDS;
	prev_largest_TDS = largest_value_TDS;
	prev_largest_pH = largest_value_pH;

	 osDelay(5000);
	for(int samples = 0; samples<30; samples++)			   // sample TDS and PH every half second for 30 times
	{

		sample_array_TDS[samples] = readWaterTDS();
		sample_array_pH[samples] = readPH();
		 osDelay(10);
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

			 osDelay(5000);
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



void waterTempControl()
{

}
void systemControl()
{


	if(run_once == 1)// && write_times == 2)
	{
		//doseWater(30,30,50);
		run_once = 0;
		fanOn();
		setTimeDate(0x01, 0x08, 0x22, 0x19, 0x09, 0x00); // MUST BE HEX BUT NOT CONVERTED i,e,(the 22 day of the month is represented as 0x22 NOT 0x16) (month, day, year, hours, min, sec)
		setLightCyle(19, 9, 19, 10); 			   		 // MUST BE INT (start hour, start min, start sec, end hour, end min)
	}


	//float_to_string(10.21);
	//convertedString[1] = convertedString[1];

	getFiveSamples(1);
	//balancePhAndNutrient();

	// Water Temp control



}

