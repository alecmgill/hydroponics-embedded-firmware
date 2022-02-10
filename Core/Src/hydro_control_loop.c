/*
 * hydro_control_loop.c
 *
 *  Created on: Feb 9, 2022
 *      Author: Alec
 */


void systemControl()
{
	int runOnce = 1;
	setTimeDate(0x01, 0x08, 0x22, 0x19, 0x09, 0x00); // MUST BE HEX BUT NOT CONVERTED i,e,(the 22 day of the month is represented as 0x22 NOT 0x16) (month, day, year, hours, min, sec)
	setLightCyle(19, 9, 19, 10); 			   		   // MUST BE INT (start hour, start min, start sec, end hour, end min)

	while (1)
	{
		fanOn();
		getTime();
		checkLightCycle();

		double water_temp = readWaterTemp();
		float TDS = readWaterTDS();
		float pH = readPH();

		if(runOnce != 0)
		{
			doseWater(5.0, 5.0, 5.0);		// step the pump motor
			runOnce = 0;
		}
	}
}
