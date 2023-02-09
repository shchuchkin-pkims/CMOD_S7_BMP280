#include "bmp280.h"
#include "xbasic_types.h"
#include "xiic.h"


/**
 * @brief IIC read register data
 * @param i2c_address IIC device bus address
 * @param reg_address IIC device register to be read address
 * @param data Pointer to data buffer
 * @param read_count Number of bytes to read
 * @return 1 if success
 */
int i2c_read(int i2c_address, unsigned char reg_address, u8 *data, int read_count)	//change u8 to u32?
	{
	    int received;
	    unsigned short status_reg;
	   // xil_printf("reg_address=%d\n\r", reg_address);	//display in address terminal for debuging

	    //do
	    //{
	        status_reg = XIic_ReadReg(XPAR_IIC_0_BASEADDR, XIIC_SR_REG_OFFSET);
	        if(!(status_reg & XIIC_SR_BUS_BUSY_MASK))
	        {
	            received = XIic_Send(XPAR_IIC_0_BASEADDR, i2c_address, (u8*)&reg_address, 1, XIIC_STOP);

	            if(received != 1)
	            {
	                XIic_WriteReg(XPAR_IIC_0_BASEADDR, XIIC_CR_REG_OFFSET, XIIC_CR_TX_FIFO_RESET_MASK);
	                XIic_WriteReg(XPAR_IIC_0_BASEADDR, XIIC_CR_REG_OFFSET, XIIC_CR_ENABLE_DEVICE_MASK);
	            }
	        }

	    //} while(received != 1);

	    received = XIic_Recv(XPAR_IIC_0_BASEADDR, i2c_address, data, read_count, XIIC_STOP);
	    return received;
	}

/**
 * @brief IIC write register data
 * @param i2c_address IIC device bus address
 * @param reg_address IIC device register address to write data
 * @param data Pointer to data buffer
 * @param read_count Number of bytes to write
 * @return The number of bytes sent.
 */
int i2c_write(int i2c_address, unsigned char reg_address, u8 *data, int write_count)
	{
	    int sent;
	    unsigned short status_reg;

		u8 data_buf[1];
		data_buf[0] = 1;
		data_buf[1] = 2;

		data_buf[0] = (u8)reg_address;
		data_buf[1] = (u8)data;

	    //xil_printf("reg_address=%X\n\r", reg_address);

	    //do
	    //{
	        status_reg = XIic_ReadReg(XPAR_IIC_0_BASEADDR, XIIC_SR_REG_OFFSET);
	        if(!(status_reg & XIIC_SR_BUS_BUSY_MASK))
	        {
	            sent = XIic_Send(XPAR_IIC_0_BASEADDR, i2c_address, &data_buf[0], write_count + 1, XIIC_STOP);

	            if(sent != (write_count + 1))
	            {
	                XIic_WriteReg(XPAR_IIC_0_BASEADDR, XIIC_CR_REG_OFFSET, XIIC_CR_TX_FIFO_RESET_MASK);
	                XIic_WriteReg(XPAR_IIC_0_BASEADDR, XIIC_CR_REG_OFFSET, XIIC_CR_ENABLE_DEVICE_MASK);
	            }
	        }

	    //} while(sent != (write_count + 1));

	    return sent;
	}




void BMP280_printTemp_UART()
{
			  	 //	xil_printf("Temperature = %d°C\n\r", (int)T);
}

/****************************************************************************/
/**
* Converting BMP280 registers values to temperature in Celsius.
*
*
* @param	temp_xlsb is a part of temperature value register.
* @param	dig_T1_lsb is a lesser byte of a temperature calibration constant
*
* @return
*		- Temperature in degrees in Celsius.
*
* @note		None.
*
*****************************************************************************/
void BMP280_ReadDeviceID()
{
	u32 *msg = 0x00;
	int received = 0;	//return 1 in case of success
	xil_printf("Reading device ID...\n\r");
	received = i2c_read(BMP280_IIC_ADDR, BMP280_ID_REG_ADDR, &msg, 1);
	xil_printf("Device ID = %X\n\r", msg);
}

/****************************************************************************/
/**
* Converting BMP280 registers values to temperature in Celsius.
*
*
* @param	temp_xlsb is a part of temperature value register.
* @param	dig_T1_lsb is a lesser byte of a temperature calibration constant
*
* @return
*		- Temperature in degrees in Celsius.
*
* @note		None.
*
*****************************************************************************/
double BMP280_ReadCalibConst(u32 temp_xlsb, u32 temp_lsb, u32 temp_msb, u32 dig_T1_lsb,	u32 dig_T1_msb, u32 dig_T2_lsb, u32 dig_T2_msb, u32 dig_T3_lsb, u32 dig_T3_msb)
{
	return 0;
}


/****************************************************************************/
/**
* Converting BMP280 registers values to temperature in Celsius.
*
*
* @param	temp_xlsb is a part of temperature value register.
* @param	dig_T1_lsb is a lesser byte of a temperature calibration constant
*
* @return
*		- Temperature in degrees in Celsius.
*
* @note		None.
*
*****************************************************************************/
double BMP280_TempConvertion(u32 temp_xlsb, u32 temp_lsb, u32 temp_msb, u32 dig_T1_lsb,	u32 dig_T1_msb, u32 dig_T2_lsb, u32 dig_T2_msb, u32 dig_T3_lsb, u32 dig_T3_msb)
{
	double T, var1, var2;
	u32 adc_T;
	u32 dig_T1, dig_T2, dig_T3;

	adc_T = (temp_msb<<12) + (temp_lsb<<4) + (temp_xlsb & 0xF);

	dig_T1 = (dig_T1_msb << 8) + dig_T1_lsb;
	dig_T2 = (dig_T2_msb << 8) + dig_T2_lsb;
	dig_T3 = (dig_T3_msb << 8) + dig_T3_lsb;

	var1 = ( ( (double)adc_T)/16384.0-( (double)dig_T1 )/1024.0)*( (double)dig_T2 );
	var2 = ((((double)adc_T )/131072.0-( (double)dig_T1 )/8192.0) * ( ( (double)adc_T )/131072.0 - ( (double)dig_T1 )/8192.0)) * ( (double)dig_T3 );
	T = (var1 + var2) / 5120.0;
	return T;
}
