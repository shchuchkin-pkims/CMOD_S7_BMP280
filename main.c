#include "xparameters.h" //Библиотека с параметрами IP-блоков
#include "xgpio.h"		 //Библиотека с функциями GPIO
#include "xiic.h"
#include "xil_exception.h"
#include "xstatus.h"

#include "bmp280.h"	//BMP280 library
/*
#define BMP280_IIC_ADDR				0x76
#define BMP280_ID_REG_ADDR			0xD0
*/

#define MST_IIC_BASE_ADDR   		XPAR_IIC_0_BASEADDR

XGpio gpio; //Создаем "программную" модель GPIO


int main(){

    u32 i   = 0; //используем для задержки
    u32 led = 0; //состояние светодиода

	u32 msg = 0x00;
	//u32 msg_direct = 0x00;		//for direct read from IIC TX FIFO
    int sent = 0;
    int received = 0;	//return 1 in case of success

    u32 dig_T1_lsb = 0x00, dig_T1_msb = 0x00, dig_T2_lsb = 0x00,  dig_T2_msb = 0x00, dig_T3_lsb = 0x00, dig_T3_msb = 0x00;			//compensation parameter for temp
    u32 temp_xlsb = 0x00, temp_lsb = 0x00, temp_msb = 0x00;		//temp data

    u32 adc_T;
    u32 dig_T1, dig_T2, dig_T3;
    double T, var1, var2;

    XGpio_Initialize(&gpio, XPAR_GPIO_0_DEVICE_ID);//Находим и инициализируем GPIO

    while(1){ //Бесконечный цикл мигания
        i++; //увеличиваем счётчик
        if(i == 300000)
        {//Если достигнуто значение 1_000_000
		  if (led == 31)
            	{
			  		led = 0;
			  		//received = i2c_read(BMP280_IIC_ADDR, BMP280_ID_REG_ADDR, &msg, 1);

			  		BMP280_ReadDeviceID();
			  		//Configuration
			  		 	(void)i2c_write(BMP280_IIC_ADDR, 0xF4, 0xB7, 1);	// normal mod, oversampling

			  		 //	xil_printf("Reading Calibration constants\n\r");
			  		 	received = i2c_read(BMP280_IIC_ADDR, 0x88, &dig_T1_lsb, 1);
			  		 	received = i2c_read(BMP280_IIC_ADDR, 0x89, &dig_T1_msb, 1);
			  		 /*	xil_printf("dig_T1_lsb=%X\n\r", dig_T1_lsb);
			  		 	xil_printf("dig_T1_msb=%X\n\r", dig_T1_msb);*/

			  		 	received = i2c_read(BMP280_IIC_ADDR, 0x8A, &dig_T2_lsb, 1);
			  		 	received = i2c_read(BMP280_IIC_ADDR, 0x8B, &dig_T2_msb, 1);
			  		 /* xil_printf("dig_T2_lsb=%X\n\r", dig_T2_lsb);
			  		 	xil_printf("dig_T2_msb=%X\n\r", dig_T2_msb);*/

			  		 	received = i2c_read(BMP280_IIC_ADDR, 0x8C, &dig_T3_lsb, 1);
			  		 	received = i2c_read(BMP280_IIC_ADDR, 0x8D, &dig_T3_msb, 1);
			  		 /*	xil_printf("dig_T3_lsb=%X\n\r", dig_T3_lsb);
			  		 	xil_printf("dig_T3_msb=%X\n\r", dig_T3_msb);*/

			  		 //	xil_printf("Reading Temperature value from ADC\n\r");
			  		 	received = i2c_read(BMP280_IIC_ADDR, 0xFC, &temp_xlsb, 1);
			  		 	received = i2c_read(BMP280_IIC_ADDR, 0xFB, &temp_lsb, 1);
			  		 	received = i2c_read(BMP280_IIC_ADDR, 0xFA, &temp_msb, 1);
			  		 /*	xil_printf("xlsb=%X\n\r", temp_xlsb);
			  		 	xil_printf("lsb=%X\n\r", temp_lsb);
			  		 	xil_printf("msb=%X\n\r", temp_msb);*/
			  		 	double tt = (double) temp_msb;

			  			T = BMP280_TempConvertion(temp_xlsb, temp_lsb, temp_msb, dig_T1_lsb, dig_T1_msb, dig_T2_lsb, dig_T2_msb, dig_T3_lsb, dig_T3_msb);


			  		 	//u32 temp = temp_msb << 3 + temp_lsb << 1 + temp_xlsb >> 1;
			  		 	xil_printf("Temperature = %d°C\n\r", (int)T);
			  		 	//xil_printf("Temperature=%4f\n\r", T);
            	}
            else
            	led++;
        	//led = !led;//Инвертируем состояние светодиода
            XGpio_DiscreteWrite(&gpio, 1, led);//Записываем состояние светодиода в GPIO
            i = 0;//Сбрасываем сётчик
        }
    }
    return 0;
}
