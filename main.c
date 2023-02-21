#include "xparameters.h" // IP-blocks parameters library
#include "xgpio.h"		 // GPIO functions library
#include "xiic.h"
#include "xil_exception.h"
#include "xstatus.h"

#include "bmp280.h"			// BMP280 library
#include "PL_CMOD_S7.h"		// CMOD S7 peripheral library

#define MST_IIC_BASE_ADDR   		XPAR_IIC_0_BASEADDR

XGpio gpio;		// GPIO structure
u32 led = 0; 	// LED state counter


int main()
{
    u32 dig_T1_lsb = 0x00, dig_T1_msb = 0x00, dig_T2_lsb = 0x00,
    dig_T2_msb = 0x00, dig_T3_lsb = 0x00, dig_T3_msb = 0x00;	// compensation parameters for temperature
    u32 temp_xlsb = 0x00, temp_lsb = 0x00, temp_msb = 0x00;		// temperature raw data
    double T;	// Temperature value

    XGpio_Initialize(&gpio, XPAR_GPIO_0_DEVICE_ID);	// GPIO initialization

    BMP280_ReadDeviceID();	// Read BMP280 device ID and print it via UART

    while(1)
    {
    	// Board LED blink
    	led_blink(gpio);

    	// Configuration
    	(void)i2c_write(BMP280_IIC_ADDR, 0xF4, 0xB7, 1);	// normal mod, oversampling

    	// Reading calibration constants
    	BMP280_ReadCalibConst(&dig_T1_lsb, &dig_T1_msb, &dig_T2_lsb, &dig_T2_msb, &dig_T3_lsb, &dig_T3_msb);

    	// Reading temperature values
    	BMP280_ReadTempValues(&temp_xlsb, &temp_lsb, &temp_msb);

    	T = BMP280_TempConvertion(temp_xlsb, temp_lsb, temp_msb, dig_T1_lsb, dig_T1_msb, dig_T2_lsb, dig_T2_msb, dig_T3_lsb, dig_T3_msb);
    	xil_printf("Temperature = %d°C\n\r", (int)T);

    }
    return 0;
}


/**
 * @brief Board 4 LED array cyclic blink
 * @return none.
 * @note ToDo: Transfer to PL_CMOD_S7 library
 */
void led_cyclic_blink()
{
	delay_sec();
	if (led == 31)
	{
		led = 0;
	}
	else
		led++;
	XGpio_DiscreteWrite(&gpio, 1, led);
}

