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

char charFloatBuffer[6] = {0},initialTDS[6] = {0}, postNutrientTDS[6] = {0}, initialPH[6] = {0},postNutrientPH[6] = {0}, timeToEqualize[6] = {0};
int data_array_length = 10000;
char calibration_data_array[10000] = {0}; // max data points is 347
char dataBuffer[10000] = {0};

char converted_char[7] = {0};

int write_times = 0;
int beenWritten = 0;		// if value is 1 first file written flash drive, 2 second file written to flash drive


char secondlineWriteBuf[45] = {0};   // append to beginning time to equilize, ph after 5ml ph up, TDS adter 5ml ph-up
char thirdlineWriteBuf[45]  = {0};
char fourthlineWriteBuf[45] = {0};
char fifthlineWriteBuf[45] =  {0};
char sixthlineWriteBuf[45] =  {0};


int whatToSave = 0; // value 0 no file selected to save, 1 save first file, 2 save second file
double minPh = 4.0;
double maxPh = 9.0;
double nutrientMax = 5000;
double ph_calibration_dose = 1.0;
double nutrient_calibration_dose = 1.0;

void getFiveSamples()
{
	water_temp = 0;
	TDS = 0;
	pH = 0;
	for(int fiveSamples = 0; fiveSamples<5;fiveSamples++)
	{
		water_temp	  += readWaterTemp();			// get sensor data so we can tell if its time to do pH up or pH down
		TDS 		  += readWaterTDS();
		pH 		 	  += readPH();
		HAL_Delay(100);
	}
	water_temp	  = water_temp/5;			// get sensor data so we can tell if its time to do pH up or pH down
	TDS 		  = TDS/5;
	pH 		 	  = pH/5;
}

void saveInitialState()
{
	getFiveSamples();
	sprintf(converted_char, "%.2f", TDS);
	for(int e = 0; e<6;e++) // transfer the 6 chars to the initial TDS and pH variables
	{
		initialTDS[e] = converted_char[e];
	}
	sprintf(converted_char, "%.2f", pH);
	for(int i = 0; i<6;i++) // transfer the 6 chars to the initial TDS and pH variables
	{
		initialPH[i] = converted_char[i];
	}

}


float totalTimeSeconds = 0;
void saveStateAfterNutrient(float totalSeconds)
{
	TDS = 0;
	pH = 0;
	for(int d = 0; d<5;d++) // get 5 samples of the initial sensor data
	{
		TDS  = TDS+readWaterTDS(); // get sensor data
		pH 	= pH+readPH();
	}
	TDS = TDS/5;
	pH = pH/5;
	sprintf(converted_char, "%.2f", TDS);
	for(int e = 0; e<6;e++) // transfer the 6 chars to the initial TDS and pH variables
	{
		postNutrientTDS[e] = converted_char[e];
	}
	sprintf(converted_char, "%.2f", pH);
	for(int i = 0; i<6;i++) // transfer the 6 chars to the initial TDS and pH variables
	{
		postNutrientPH[i] = converted_char[i];
	}

	sprintf(converted_char, "%.2f", totalSeconds);
	for(int f = 0; f<6;f++) // transfer the 6 chars to the initial TDS and pH variables
	{
		timeToEqualize[f] = converted_char[f];
	}

}

float endEqTimeSec = 0;

float startEqTimeSec = 0;
float sampleArrayTDS[30] = {0};
float sampleArrayPH[30] = {0};


float totalRuns = 0;
float startTimeMinutes = 0;
float startTimeSeconds = 0;
float endTimeMinutes = 0;
float endTimeSeconds = 0;
float smallestValueTDS = 100000; // set smallest values to value much higher than expected
float largestValueTDS = 0;			// set largest to the smallest possible value these steps ensure we catch error cases
float smallestValuePH = 100;
float largestValuePH = 0;
float prevLargestPH = 0;
float prevLargestTDS = 0;

int writeHeader = 0;
int isStabalized() // will take a few samples of the waters pH and TDS to determine if the solution has stabilized. If stabilized returns one else 0
{	getTime();// get the current system time sets global variables sTime and sDate in RTC_driver.c

	//startTimeMinutes = sTime.Minutes;
	startTimeSeconds =  sTime.Seconds+(sTime.Minutes*60);
	//endTimeMinutes = 0;
	endTimeSeconds = 0;

	for(int samples = 0; samples<30; samples++)	// sample TDS and PH every second for 30 times
	{
		sampleArrayTDS[samples] = readWaterTDS();
		sampleArrayPH[samples] = readPH();
		HAL_Delay(1000);
	}
	float smallestValueTDS = 10000; // set smallest values to value much higher than expected
	largestValueTDS = 0;			// set largest to the smallest possible value these steps ensure we catch error cases
	smallestValuePH = 100;
    largestValuePH = 0;

	for(int i = 0; i<30;i++) // calculate the range in the samples i.e. (largest valve)-(smallest value)
	{
		if(sampleArrayTDS[i]>largestValueTDS) largestValueTDS = sampleArrayTDS[i];

		if(sampleArrayTDS[i]<smallestValueTDS) smallestValueTDS = sampleArrayTDS[i];

		if(sampleArrayPH[i]>largestValuePH) largestValuePH = sampleArrayPH[i];

		if(sampleArrayPH[i]<smallestValuePH) smallestValuePH = sampleArrayPH[i];

	}
	float TdsRange = largestValueTDS - smallestValueTDS;
	float PhRange = largestValuePH - smallestValuePH;


	if(TdsRange > 15.0 || PhRange > 0.05)
	{
		isStabalized();
	}
	else if(TdsRange <= 15.0 && PhRange <= 0.05) // we are stabilized however, the system could still be changing so we need to check how many times we re-sampled the water if its > 1 check until its 1
	{
		getTime();
		//endTimeMinutes = sTime.Minutes;
		endTimeSeconds = sTime.Seconds+(sTime.Minutes*60);
		//totalTimeMinutes = endTimeMinutes-startTimeMinutes;
		totalTimeSeconds = endTimeSeconds-startTimeSeconds;
		totalRuns = (totalTimeSeconds/36);

		if(prevLargestTDS != 0 && abs(largestValueTDS-prevLargestTDS)>2.0 && prevLargestPH != 0 && abs(largestValuePH-prevLargestPH)>0.2) isStabalized();// if we have run again, check to see if the values changed from last run if so run again if not continue
		prevLargestTDS = largestValueTDS;
		prevLargestPH = largestValuePH;
		if(totalRuns <= 1.3 )
		{
			largestValueTDS = 0;
			prevLargestPH = 0;
			return 1;  // system is stabilized return its good
		}
		else isStabalized(); 			// system just experienced alot of change so check to see if its actually stabilized i.e. run again

	}
	return 0;

}

int initialRun = 1;
float minimumSecDuration = 36; // this holds the number of seconds to get the data.
int stabilityValue = 0;

float waitForWaterToStabilize() // Returns the total time in seconds
{
	getTime();
	startEqTimeSec = sTime.Seconds + sTime.Minutes*60;
	if(stabilityValue != 1)
	{	stabilityValue = 0;
		stabilityValue = isStabalized();
	}
	getTime();

	endEqTimeSec = sTime.Seconds + sTime.Minutes*60;
	stabilityValue = 0;
	// data_array_length can range between 10-10,000
	totalTimeSeconds = endEqTimeSec-startEqTimeSec;

	return (totalTimeSeconds);
}

void set_init_water_ph()
{
	if((pH-minPh)<(maxPh-pH) && (pH-minPh) > 0.5) // if we are closer to minimum set point dose pH-Down until we reach the minimum set point
	{
		while (pH>minPh)
		{
			doseWater(ph_calibration_dose, 0, 0);
			waitForWaterToStabilize();
		}
	}
	else if((pH-minPh)>(maxPh-pH) && (maxPh-pH) > 0.5)
	{
		while (pH<maxPh)
		{
			doseWater(0, ph_calibration_dose, 0);	// else dose with pH-up
			waitForWaterToStabilize();//
		}
	}

}

void calibrateSys()
{

	if(whatToSave == 0) getFiveSamples();	// get initial sensor data

	if(initialRun == 1)
	{
		set_init_water_ph();		// add ph-up/ph-down if the water pH is out of our calibration bounds
		saveInitialState();			 // save initial state
		doseWater(0, 0, nutrient_calibration_dose);		// add nutrient here and wait for it to equalize step the pump motors (pHDown,pHup,nutrient_dose)
		totalTimeSeconds = waitForWaterToStabilize();	// Returns the total time in seconds
		saveStateAfterNutrient(totalTimeSeconds);
		initialRun = 0;
	}

	if((beenWritten == 2 || beenWritten == 1) && write_times < 2) // if we just wrote a file and have not completed collecting data for all files reset buffers/vars
	{
		doseWater(0, 0, nutrient_calibration_dose);		// add nutrient here and wait for it to equalize step the pump motors (pHDown,pHup,nutrient_dose)
		totalTimeSeconds = waitForWaterToStabilize();	// Returns the total time in seconds
		saveStateAfterNutrient(totalTimeSeconds);
		for(int i = 0; i<10000; i++) // if we wrote a file reset the dataBuffer and data array to allow multiple files to be written
		{
			dataBuffer[i-1] = '\0';
			calibration_data_array[i] = '\0';
			if(i<(44))secondlineWriteBuf[i] = '\0';   // append to beginning time to equilize, ph after 5ml ph up, TDS adter 5ml ph-up
			if(i<(44))thirdlineWriteBuf[i]  = '\0';
			if(i<(44))fourthlineWriteBuf[i] = '\0';
			if(i<(44))fifthlineWriteBuf[i] =  '\0';
			if(i<(44))sixthlineWriteBuf[i] =  '\0';

			if(i == 1)
			{
				writeHeader = 0;	// reset the write header var so we can output multiple files
				whatToSave = 0;
				initialRun = 0;
			}

		}
	}



	int end = 0;	// find the end of the databuffer so we can append
	for(int j = 0; j<data_array_length && whatToSave == 0;j++)
	{
		if(dataBuffer[j] == '\0')
		{
			end = j;
			break;
		}
	}

	int a = 0;
	//collectedData = 0;

// POSSIBLE REASON FOR POBLEM IS THAT WE ARE RUNNING OUT OF BUFFER SPACE?
	if(end<data_array_length && pH<=(minPh+.20) && (beenWritten == 0 || beenWritten == 2) && whatToSave == 0) // do pH-up calibration if the pH is less than or equal to 4.0 and we haven't already saved pH-up dose until we reached a pH of 9.0
	{
		for(int k = end;(k < (data_array_length-27)) && (whatToSave == 0) && (pH<maxPh) ;k++)
		{
			doseWater(0, ph_calibration_dose, 0);		// step the pump motors (pHDown,pHup,nutrient_dose)
			float timeSeconds = waitForWaterToStabilize();// Returns the total time in seconds// convert sensor float type to char array and add to buffer (water_temp, ph after, TDS after, time to equilize)

			getFiveSamples();			// get sensor data

			sprintf(converted_char, "%.2f", water_temp);
			for(a = 0; a < (sizeof converted_char);a++)
			{
				dataBuffer[k] = converted_char[a];
				k++;
			}
			dataBuffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", pH);
			for(a = 0; a < (sizeof converted_char);a++)
			{
				dataBuffer[k] = converted_char[a];
				k++;
			}
			dataBuffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", TDS);
			for(a = 0; a < (sizeof converted_char);a++)
			{
				dataBuffer[k] = converted_char[a];
				k++;
			}
			dataBuffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", timeSeconds);

			for(a = 0; a < (sizeof converted_char);a++)
			{
				dataBuffer[k] = converted_char[a];
				k++;
			}
			dataBuffer[k] = ',';
			k++;
			if(pH>=maxPh)
			{
				whatToSave = 1;
			}
		}


	}
	else if(end<data_array_length && pH>=(maxPh-.20) && (beenWritten == 0 || beenWritten == 1) && whatToSave == 0) // do pH-down calibration if the pH is greater than 4.0 and end at pH 4.0 and we wrote the first file or we didnt write the first file
	{
		for(int k = end;(k < (data_array_length-27)) &&  whatToSave == 0 && (pH>(minPh)) ;k++)
		{
 			doseWater(ph_calibration_dose, 0, 0);		// step the pump motors (pHDown,pHup,nutrient_dose)
			float timeSeconds = waitForWaterToStabilize();// Returns the total time in seconds// convert sensor float type to char array and add to buffer (water_temp, ph after, TDS after, time to equilize)

			getFiveSamples();		// get sensor data

			sprintf(converted_char, "%.2f", water_temp);					// converts input float to converted_char array (method level)
			for(a = 0; a < (sizeof converted_char);a++)
			{
				dataBuffer[k] = converted_char[a];
				k++;
			}
			dataBuffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", pH);
			for(a = 0; a < (sizeof converted_char);a++)
			{
				dataBuffer[k] = converted_char[a];
				k++;
			}
			dataBuffer[k] = ',';
			k++;

			sprintf(converted_char, "%.2f", TDS);
			for(a = 0; a < (sizeof converted_char);a++)
			{
				dataBuffer[k] = converted_char[a];
				k++;
			}
			dataBuffer[k] = ',';
			k++;


			sprintf(converted_char, "%.2f", timeSeconds);

			for(a = 0; a < (sizeof converted_char);a++)
			{
				dataBuffer[k] = converted_char[a];
				k++;
			}
			dataBuffer[k] = ',';
			k++;
			if(pH<=minPh )
			{
				whatToSave = 2;
			}
		}


	}// add else if(pH not within bounds to pH up/down add pH down until it reaches 4.0


		//if(pH>maxPh)
		//doseWater(5.0, 0, 0);		// step the pump motors (pHDown,pHup,nutrient_dose)


	if(usb_good == 1 && (whatToSave < 3) && (whatToSave != 0))
	{
		writeCalibrationData(whatToSave);

	}
}


char newLine[] = "\n";
char four_commas[] = ",,,,";
char quote[] = "\"";
char title[] = "3ml nurient solution vs pH-down characteristics. Fresh tap water volume of 5 gal, nutrient solution appx 2.4g in .5 gal";
char firstline[] = "Temp,pH after 5ml pH-up ,TDS after 5ml ph-up,time to equalize pH,Notes";

char secondline[45] = "TDS before nutrient addition: ,";   // append to beginning time to equilize, ph after 5ml ph up, TDS adter 5ml ph-up
char thirdline[45]  = "TDS after 3ml nutrient addition:,";
char fourthline[45] = "pH before nutrient addition: ,";
char fifthline[45] =  "pH after 3ml nutrient addition: ,";
char sixthline[45] =  "Time to stabalize:,";


int lastValTwo = 0,lastValThree = 0,lastValFour = 0,lastValFive = 0,lastValSix = 0;

int lineNumber = 0;
char singleChar[1] = {0};
int dataValueReady = 0;
int beenAppended = 0;


void writeCalibrationData(int file) // file refers to the document to be written (pH-down characteristcs=0, pH-up=1)
{

	int j =0; // indexes for concating float to char array
	int k = 0;
	lineNumber = 0;
	if(write_times < 2)
	{
		// NOTE: Following code was created because strcat() would somewhat randomly concat to the beginning or end
		// We need to add the initial water parameters to their respective strings
		// find the end of the array we want to place our merge val
		// append the initial TDS to the end of the second line

		for(int i = 0; i<44; i++) // copy over the text that will be appended so not to edit original
		{
			secondlineWriteBuf[i] = secondline[i];   // append to beginning time to equilize, ph after 5ml ph up, TDS adter 5ml ph-up
			thirdlineWriteBuf[i]  = thirdline[i];
			fourthlineWriteBuf[i] = fourthline[i];
			fifthlineWriteBuf[i] =  fifthline[i];
			sixthlineWriteBuf[i] =  sixthline[i];
		}

		if(lineNumber == 0 && writeHeader == 0)
		{
			strcat(calibration_data_array,quote);
			strcat(calibration_data_array,title);
			strcat(calibration_data_array,quote);
			strcat(calibration_data_array,four_commas);
			strcat(calibration_data_array,newLine);
			strcat(calibration_data_array,firstline);
			strcat(calibration_data_array,newLine);
			writeHeader = 1;
		}


		for( j = 0; j<44;j++)
		{
			if(secondlineWriteBuf[j] != '\0')lastValTwo = j;

			if(thirdlineWriteBuf[j] != '\0')lastValThree = j;

			if(fourthlineWriteBuf[j] != '\0') lastValFour = j;

			if(fifthlineWriteBuf[j] != '\0')lastValFive = j;

			if(sixthlineWriteBuf[j] != '\0') lastValSix = j;
		}
		for(k = 1; k<7;k++)	// write the initialTDS at the end of the array
		{
			secondlineWriteBuf[lastValTwo+k] = initialTDS[k-1];
			thirdlineWriteBuf[lastValThree+k] = postNutrientTDS[k-1];
			fourthlineWriteBuf[lastValFour+k] = initialPH[k-1];
			fifthlineWriteBuf[lastValFive+k] = postNutrientPH[k-1];
			sixthlineWriteBuf[lastValSix+k] = timeToEqualize[k-1];
		}

		for( j = 0; j<data_array_length; j++)
		{
			singleChar[0] = dataBuffer[j];
			strncat(calibration_data_array,singleChar,1);	// add the data one char at a time

			if(singleChar[0] == ',')	// every 3 commas is a new line
			{
				dataValueReady++;
			}
			if(dataValueReady == 4)			// every 4 data points append to char array string
			{
				dataValueReady = 0;
				lineNumber++;
				beenAppended = 0;
			}
			if(lineNumber == 1 && beenAppended == 0)
			{
				strcat(calibration_data_array,secondlineWriteBuf);
				strcat(calibration_data_array,newLine);
				beenAppended = 1;
			}
			else if(lineNumber == 2 && beenAppended == 0)
			{
				strcat(calibration_data_array,thirdlineWriteBuf);
				strcat(calibration_data_array,newLine);
				beenAppended = 1;
			}
			else if(lineNumber == 3 && beenAppended == 0)
			{
				strcat(calibration_data_array,fourthlineWriteBuf);
				strcat(calibration_data_array,newLine);
				beenAppended = 1;
			}
			else if(lineNumber == 4 && beenAppended == 0)
			{
				//strcat(calibration_data_array,newLine);
				strcat(calibration_data_array,fifthlineWriteBuf);
				strcat(calibration_data_array,newLine);
				beenAppended = 1;
			}
			else if(lineNumber == 5 && beenAppended == 0)
			{
				strcat(calibration_data_array,sixthlineWriteBuf);
				strcat(calibration_data_array,newLine);
				beenAppended = 1;
			}
			else if(lineNumber >5 && beenAppended == 0)
			{
				strcat(calibration_data_array,newLine);
				beenAppended = 1;
			}

		}

			if(file == 1 && write_times < 2)
			{
				Write_File("/TDS_PH_UP_Calibration_data.csv",calibration_data_array); // write the data to pH-up data.
				beenWritten = 1;
				write_times++;
				whatToSave = 0;
			}
			else if(file == 2 && write_times < 2)	// if we wrote the last file unmount the USB
			{
				beenWritten = 2;
				Write_File("/TDS_PH_Down_Calibration_data.csv",calibration_data_array); // write the data to pH-up data.
				write_times++;
				whatToSave = 0;
			}

			if(write_times > 2) Unmount_USB();

		}
		else return;



}

