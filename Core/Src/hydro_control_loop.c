/*
 * hydro_control_loop.c
 *
 *  Created on: Feb 9, 2022
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
double max_pH_up_dose = 5.0, max_pH_down_dose = 5.0, pH_up_dose = 0, pH_down_dose = 0, max_nutrient_dose = 80, nutrient_dose = 0, total_nutrient_ml = 0, total_pH_up_ml = 0, total_pH_down_ml = 0, total_nutrient_ml_per_file = 0, total_pH_up_ml_per_file = 0, total_pH_down_ml_per_file = 0;
float nutrient_set_point = 185.0, pH_set_point = 6.0, water_temp_set_point = 20.0, pH_bounds_check = 0.10, pH_bounds_set = 0.05, nutrient_bounds_check = 15, nutrient_bounds_set = 7.5, TDS = 0, pH = 0, water_temp = 0;
char balance_data[50000] = {0};
int run_once = 1, error = 0, balance_index = 0, i = 0;
int time_to_bal_nutrient = 0;
int time_to_bal_pH = 0;
int pH_up = 0;
int pH_down = 0;
int nutrient_up = 0;
char char_of_number[7] = {0};
int num_of_stable_runs = 0;
char waiting_to_write = 'n';
extern void calibrateSys();
extern int  waitForWaterToStabilize();
char get_init_conditions = 'n';

double total_prev_pH_and_nutrient_ml = 0;
int file_number = 0;
int file_index = 0;
char file_name[25] = "balance_data_";
char extention[5] = ".csv";
char buffer[25] = {0};

void write_file()
{
	for(file_index = 0; file_index<16 && file_name[file_index] != '\0'; file_index++)
	{
		buffer[file_index] = file_name[file_index];
	}
	sprintf(char_of_number, "%d", (int)file_number);
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++,file_index++)
	{
		buffer[file_index] = char_of_number[i];
	}


	//buffer[file_index] = char_of_number[file_index];
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
	sprintf(char_of_number, "%.2f", (double)pH);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	sprintf(char_of_number, "%.2f", (double)total_pH_up_ml);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	sprintf(char_of_number, "%.2f", (double)pH_up_dose);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;


	sprintf(char_of_number, "%.2f", (double)total_pH_down_ml);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	sprintf(char_of_number, "%.2f", (double)pH_down_dose);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;


	sprintf(char_of_number, "%d", time_to_bal_pH);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	sprintf(char_of_number, "%.2f", (double)TDS);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	sprintf(char_of_number, "%.2f", (double)total_nutrient_ml);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;


	sprintf(char_of_number, "%.2f", (double)nutrient_dose);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;



	sprintf(char_of_number, "%d", time_to_bal_nutrient);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;

	sprintf(char_of_number, "%d", error);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = ',';
	balance_index++;
	sprintf(char_of_number, "%.2f", (double)water_temp);	// convert water temp to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)
	{
		balance_data[balance_index] = char_of_number[i];
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
	sprintf(char_of_number, "%.2f", pH);									// convert pH to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)	// append pH to our data array
	{
		balance_data[balance_index] = char_of_number[i];
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
	sprintf(char_of_number, "%.2f", pH_set_point);									// convert pH set point to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)			// append pH set point char to the data array
	{
		balance_data[balance_index] = char_of_number[i];
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
	sprintf(char_of_number, "%.2f", TDS);									// convert pH to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)	// append pH to our data array
	{
		balance_data[balance_index] = char_of_number[i];
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
	sprintf(char_of_number, "%.2f", nutrient_set_point);									// convert pH set point to char array and write it to the data buffer
	for(i = 0;i < (sizeof char_of_number) && char_of_number[i] != '\0';i++)			// append pH set point char to the data array
	{
		balance_data[balance_index] = char_of_number[i];
		balance_index++;
	}
	balance_data[balance_index] = '\n';
	balance_index++;
    get_init_conditions = 'y';
}


char setting_pH = 'n';
char setting_nutrient = 'n';

void balancePhAndNutrient()
{

}

//int testRuns = 0;
void systemControl()
{

	/*if(waiting_to_write != 'y')testRuns++;
	if(testRuns > 5) pH_set_point = 6.4;
	if(testRuns > 15)
	{
		testRuns = 0;
		pH_set_point = 6.9;
	}*/

	if(run_once == 1)// && write_times == 2)
	{
		//doseWater(30,30,50);
		run_once = 0;
		fanOn();
		setTimeDate(0x01, 0x08, 0x22, 0x19, 0x09, 0x00); // MUST BE HEX BUT NOT CONVERTED i,e,(the 22 day of the month is represented as 0x22 NOT 0x16) (month, day, year, hours, min, sec)
		setLightCyle(19, 9, 19, 10); 			   		 // MUST BE INT (start hour, start min, start sec, end hour, end min)
	}






//	if((TDS<nutrient_max && what_to_save == 0) || (write_times%2 != 0 && what_to_save == 0))calibrateSys(); // if the TDS is above range or we still need to write a file




}

