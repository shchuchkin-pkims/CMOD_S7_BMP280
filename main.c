#include "xparameters.h" //IP-blocks parameters library
#include "xgpio.h"		 //GPIO functions library
#include "xiic.h"
#include "xil_exception.h"
#include "xstatus.h"

#include "bmp280.h"	//BMP280 library

#define MST_IIC_BASE_ADDR   		XPAR_IIC_0_BASEADDR

XGpio gpio;

//delay, f = 100 MHz
void delay_sec()
{
    	for (u32 i   = 0; i<10000000; i++)
    	{

    	}
}

int main(){

    u32 led = 0; //LED state
	u32 msg = 0x00;
	//u32 msg_direct = 0x00;		//for direct read from IIC TX FIFO
    int sent = 0;
    int received = 0;	//return 1 in case of success

    u32 dig_T1_lsb = 0x00, dig_T1_msb = 0x00, dig_T2_lsb = 0x00, dig_T2_msb = 0x00, dig_T3_lsb = 0x00, dig_T3_msb = 0x00;			//compensation parameters for temp
    u32 temp_xlsb = 0x00, temp_lsb = 0x00, temp_msb = 0x00;		//temp data

    u32 adc_T;
    u32 dig_T1, dig_T2, dig_T3;
    double T, var1, var2;

    XGpio_Initialize(&gpio, XPAR_GPIO_0_DEVICE_ID);	//GPIO init

    BMP280_ReadDeviceID();

    while(1) //infinite cycle
    {
    	// Board 4 LED cyclic blink
    	delay_sec();
    	if (led == 31)
    	{
    		led = 0;
    	}
    	else
    		led++;
    	XGpio_DiscreteWrite(&gpio, 1, led);
    	//-----------------------------------

    	// Configuration
    	(void)i2c_write(BMP280_IIC_ADDR, 0xF4, 0xB7, 1);	// normal mod, oversampling

    	//	xil_printf("Reading Calibration constants\n\r");
    	received = i2c_read(BMP280_IIC_ADDR, 0x88, &dig_T1_lsb, 1);
    	received = i2c_read(BMP280_IIC_ADDR, 0x89, &dig_T1_msb, 1);

    	received = i2c_read(BMP280_IIC_ADDR, 0x8A, &dig_T2_lsb, 1);
    	received = i2c_read(BMP280_IIC_ADDR, 0x8B, &dig_T2_msb, 1);

    	received = i2c_read(BMP280_IIC_ADDR, 0x8C, &dig_T3_lsb, 1);
    	received = i2c_read(BMP280_IIC_ADDR, 0x8D, &dig_T3_msb, 1);


    	 //	xil_printf("Reading Temperature value from ADC\n\r");
    	received = i2c_read(BMP280_IIC_ADDR, 0xFC, &temp_xlsb, 1);
    	received = i2c_read(BMP280_IIC_ADDR, 0xFB, &temp_lsb, 1);
    	received = i2c_read(BMP280_IIC_ADDR, 0xFA, &temp_msb, 1);

    	double tt = (double) temp_msb;

    	T = BMP280_TempConvertion(temp_xlsb, temp_lsb, temp_msb, dig_T1_lsb, dig_T1_msb, dig_T2_lsb, dig_T2_msb, dig_T3_lsb, dig_T3_msb);
    	xil_printf("Temperature = %d°C\n\r", (int)T);

    }
    return 0;
}
