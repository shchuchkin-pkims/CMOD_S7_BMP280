#include "PL_CMOD_S7.h"
#include "xbasic_types.h"
#include "xgpio.h"

/**
 * @brief One second delay function for Microblaze with freq = 100 MHz
 * @return none.
 * @note ToDo: Add param for delay in sec/ms; Assign counter to hardware frequency.
 */
/*
void delay_sec()
{
    	for (u32 i   = 0; i<10000000; i++)
    	{

    	}
}
*/

// Board one LED blink
void led_blink(XGpio gpio)
{
	XGpio_DiscreteWrite(&gpio, 1, 0);
	usleep(500000);						// xil_sleepcommon.c default function
	XGpio_DiscreteWrite(&gpio, 1, 1);
	usleep(500000);
}
