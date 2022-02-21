/*
 * File_Handling_RTOS.c
 *
 *  Created on: 26-June-2020
 *      Author: Controllerstech.com
 */

#include "File_Handling.h"
#include "stm32f2xx_hal.h"
#include "main.h"
#include "hydro_control_loop.h"


/* =============================>>>>>>>> NO CHANGES AFTER THIS LINE =====================================>>>>>>> */



extern char USBHPath[4];   /* USBH logical drive path */
extern FATFS USBHFatFS;    /* File system object for USBH logical drive */
extern FIL USBHFile;       /* File object for USBH */

FILINFO USBHfno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pUSBHFatFS;
DWORD fre_clust;
uint32_t total, free_space;






void Mount_USB (void)
{
	fresult = f_mount(&USBHFatFS, USBHPath, 1);
}
void Unmount_USB (void)
{
	fresult = f_mount(NULL, USBHPath, 1);
}

/* Start node to be scanned (***also used as work area***) */
FRESULT Scan_USB (char* pat)
{
    DIR dir;
    UINT i;
    char *path = malloc(20*sizeof (char));

    fresult = f_opendir(&dir, path);                       /* Open the directory */
    if (fresult == FR_OK)
    {
        for (;;)
        {
            fresult = f_readdir(&dir, &USBHfno);                   /* Read a directory item */
            if (fresult != FR_OK || USBHfno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (USBHfno.fattrib & AM_DIR)     /* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", USBHfno.fname))) continue;
            	if (!(strcmp("System Volume Information", USBHfno.fname))) continue;
                fresult = Scan_USB(path);                     /* Enter the directory */
                if (fresult != FR_OK) break;
                path[i] = 0;
            }
            else
            {
            }
        }
        f_closedir(&dir);
    }
    free(path);
    return fresult;
}

/* Only supports removing files from home directory */
FRESULT Format_USB (void)
{
    DIR dir;
    char *path = malloc(20*sizeof (char));
    sprintf (path, "%s","/");

    fresult = f_opendir(&dir, path);                       /* Open the directory */
    if (fresult == FR_OK)
    {
        for (;;)
        {
            fresult = f_readdir(&dir, &USBHfno);                   /* Read a directory item */
            if (fresult != FR_OK || USBHfno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (USBHfno.fattrib & AM_DIR)     /* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", USBHfno.fname))) continue;
            	if (!(strcmp("System Volume Information", USBHfno.fname))) continue;
            	fresult = f_unlink(USBHfno.fname);
            	if (fresult == FR_DENIED) continue;
            }
            else
            {   /* It is a file. */
               fresult = f_unlink(USBHfno.fname);
            }
        }
        f_closedir(&dir);
    }
    free(path);
    return fresult;
}




FRESULT Write_File (char *name, char *data)
{

	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &USBHfno);
	if (fresult != FR_OK)
	{
	    return fresult;
	}

	else
	{
	    /* Create a file with read write access and open it */
	    fresult = f_open(&USBHFile, name, FA_OPEN_EXISTING | FA_WRITE);
	    if (fresult != FR_OK)
	    {
	        return fresult;
	    }

	    else
	    {

	    	fresult = f_write(&USBHFile, data, strlen(data), &bw);

	    	/* Close file */
	    	fresult = f_close(&USBHFile);
	    }
	    return fresult;
	}
}

FRESULT Read_File (char *name)
{
	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &USBHfno);
	if (fresult != FR_OK)
	{
	    return fresult;
	}

	else
	{
		/* Open file to read */
		fresult = f_open(&USBHFile, name, FA_READ);

		if (fresult != FR_OK)
		{
		    return fresult;
		}

		/* Read data from the file
		* see the function details for the arguments */


		char *buffer = malloc(sizeof(f_size(&USBHFile)));
		fresult = f_read (&USBHFile, buffer, f_size(&USBHFile), &br);
		if (fresult != FR_OK)
		{

		}
		else
		{
			free(buffer);

			/* Close file */
			fresult = f_close(&USBHFile);

		}
	    return fresult;
	}
}

FRESULT Create_File (char *name)
{
	fresult = f_stat (name, &USBHfno);
	if (fresult == FR_OK)
	{
	    return fresult;
	}
	else
	{
		fresult = f_open(&USBHFile, name, FA_CREATE_ALWAYS|FA_READ|FA_WRITE);
		if (fresult != FR_OK)
		{

		    return fresult;
		}
		else
		{

		}

		fresult = f_close(&USBHFile);

	}
    return fresult;
}

FRESULT Update_File (char *name, char *data)
{
	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &USBHfno);
	if (fresult != FR_OK)
	{
	    return fresult;
	}

	else
	{
		 /* Create a file with read write access and open it */
	    fresult = f_open(&USBHFile, name, FA_OPEN_ALWAYS | FA_WRITE);



	    /* Writing text */
	    fresult = f_write(&USBHFile, data, strlen (data), &bw);


	    /* Close file */
	    fresult = f_close(&USBHFile);
	}
    return fresult;
}

FRESULT Remove_File (char *name)
{
	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &USBHfno);
	if (fresult != FR_OK)
	{
		return fresult;
	}

	else
	{
		fresult = f_unlink (name);
	}
	return fresult;
}

FRESULT Create_Dir (char *name)
{
    fresult = f_mkdir(name);

    return fresult;
}

void Check_USB_Details (void)
{
    /* Check free space */
    f_getfree("", &fre_clust, &pUSBHFatFS);

    total = (uint32_t)((pUSBHFatFS->n_fatent - 2) * pUSBHFatFS->csize * 0.5);

    free_space = (uint32_t)(fre_clust * pUSBHFatFS->csize * 0.5);
}

