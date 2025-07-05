/**
 * @file bmp280.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "bmp280.h"
#include <xparameters.h>

/**
 * @brief IIC read register data
 *
 * @param i2c_address IIC device bus address
 * @param reg_address IIC device register to be read address
 * @param data Pointer to data buffer
 * @param read_count Number of bytes to read
 * @return int 1 if success
 * @todo Make simple function from Xilinx I2C lib
 */
int i2c_read(int i2c_address,
             unsigned char reg_address,
             u8 *data,
             int read_count) // change u8 to u32?
{
    int received;
    unsigned short status_reg;
    status_reg = XIic_ReadReg(XPAR_AXI_IIC_0_BASEADDR, XIIC_SR_REG_OFFSET);
    if (!(status_reg & XIIC_SR_BUS_BUSY_MASK)) {
        received = XIic_Send(XPAR_AXI_IIC_0_BASEADDR, i2c_address, (u8 *)&reg_address, 1, XIIC_STOP);
        if (received != 1) {
            XIic_WriteReg(XPAR_AXI_IIC_0_BASEADDR, XIIC_CR_REG_OFFSET, XIIC_CR_TX_FIFO_RESET_MASK);
            XIic_WriteReg(XPAR_AXI_IIC_0_BASEADDR, XIIC_CR_REG_OFFSET, XIIC_CR_ENABLE_DEVICE_MASK);
        }
    }
    received = XIic_Recv(XPAR_AXI_IIC_0_BASEADDR, i2c_address, data, read_count, XIIC_STOP);
    return received;
}

/**
 * @brief IIC write register data
 * @param i2c_address IIC device bus address
 * @param reg_address IIC device register address to write data
 * @param data Pointer to data buffer
 * @param read_count Number of bytes to write
 * @return int The number of bytes sent.
 * @todo Make simple function from Xilinx I2C lib
 */
int i2c_write(int i2c_address, unsigned char reg_address, u8 *data, int write_count)
{
    int sent;
    unsigned short status_reg;
    u8 data_buf[1];
    data_buf[0] = (u8)reg_address;
    data_buf[1] = (u8)data;
    status_reg = XIic_ReadReg(XPAR_AXI_IIC_0_BASEADDR, XIIC_SR_REG_OFFSET);
    if (!(status_reg & XIIC_SR_BUS_BUSY_MASK)) {
        sent =
            XIic_Send(XPAR_AXI_IIC_0_BASEADDR, i2c_address, &data_buf[0], write_count + 1, XIIC_STOP);

        if (sent != (write_count + 1)) {
            XIic_WriteReg(XPAR_AXI_IIC_0_BASEADDR, XIIC_CR_REG_OFFSET, XIIC_CR_TX_FIFO_RESET_MASK);
            XIic_WriteReg(XPAR_AXI_IIC_0_BASEADDR, XIIC_CR_REG_OFFSET, XIIC_CR_ENABLE_DEVICE_MASK);
        }
    }
    return sent;
}

/**
 * @brief
 *
 */
// void bmp280PrintTempUART()
// {
//                    //    xil_printf("Temperature = %d C\n\r", (int)temp);
// }

/**
 * @brief
 *
 */
void bmp280ReadDeviceID()
{
    u32 *msg = 0x00;
    int received = 0; // return 1 in case of success
    xil_printf("Reading device ID...\n\r");
    received = i2c_read(BMP280_IIC_ADDR, BMP280_ID_REG_ADDR, &msg, 1);
    xil_printf("Device ID = %X\n\r", msg);
}

/**
 * @brief Reading BMP280 calibration constants registers values.
 *
 * @param digT1Lsb is a lesser byte of a temperature calibration constant T1
 * @param digT1Msb is a major byte of a temperature calibration constant T1
 * @param digT2Lsb is a lesser byte of a temperature calibration constant T2
 * @param digT2Msb is a major byte of a temperature calibration constant T2
 * @param digT3Lsb is a lesser byte of a temperature calibration constant T3
 * @param digT3Msb is a major byte of a temperature calibration constant T3

 */
void bmp280ReadCalibConst(u32 *digT1Lsb,
                          u32 *digT1Msb,
                          u32 *digT2Lsb,
                          u32 *digT2Msb,
                          u32 *digT3Lsb,
                          u32 *digT3Msb)
{
    (void)i2c_read(BMP280_IIC_ADDR, 0x88, digT1Lsb, 1);
    (void)i2c_read(BMP280_IIC_ADDR, 0x89, digT1Msb, 1);
    (void)i2c_read(BMP280_IIC_ADDR, 0x8A, digT2Lsb, 1);
    (void)i2c_read(BMP280_IIC_ADDR, 0x8B, digT2Msb, 1);
    (void)i2c_read(BMP280_IIC_ADDR, 0x8C, digT3Lsb, 1);
    (void)i2c_read(BMP280_IIC_ADDR, 0x8D, digT3Msb, 1);
}

/**
 * @brief Reading BMP280 temperature registers raw data.
 *
 * @param tempXlsb is a XLSB part [3:0] of the raw temperature measurement.
 * @param tempLsb is a LSB part [11:4] of the raw temperature measurement.
 * @param tempMsb is a MSB part [19:12] of the raw temperature measurement.
 */
void bmp280ReadTempValues(u32 *tempXlsb, u32 *tempLsb, u32 *tempMsb)
{
    (void)i2c_read(BMP280_IIC_ADDR, 0xFC, tempXlsb, 1);
    (void)i2c_read(BMP280_IIC_ADDR, 0xFB, tempLsb, 1);
    (void)i2c_read(BMP280_IIC_ADDR, 0xFA, tempMsb, 1);
}

/**
 * @brief Converting BMP280 registers values to temperature in Celsius.
 *
 * @param tempXlsb  is a part of temperature value register.
 * @param tempLsb
 * @param tempMsb
 * @param digT1Lsb is a lesser byte of a temperature calibration constant
 * @param digT1Msb
 * @param digT2Lsb
 * @param digT2Msb
 * @param digT3Lsb
 * @param digT3Msb
 * @return double - Temperature in degrees in Celsius.
 */
double bmp280TempConvertion(u32 tempXlsb,
                            u32 tempLsb,
                            u32 tempMsb,
                            u32 digT1Lsb,
                            u32 digT1Msb,
                            u32 digT2Lsb,
                            u32 digT2Msb,
                            u32 digT3Lsb,
                            u32 digT3Msb)
{
    double temp, var1, var2;
    u32 adc_T;
    u32 dig_T1, dig_T2, dig_T3;

    adc_T = (tempMsb << 12) + (tempLsb << 4) + (tempXlsb & 0xF);

    dig_T1 = (digT1Msb << 8) + digT1Lsb;
    dig_T2 = (digT2Msb << 8) + digT2Lsb;
    dig_T3 = (digT3Msb << 8) + digT3Lsb;

    var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) *
            (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0)) *
           ((double)dig_T3);
    temp = (var1 + var2) / 5120.0;
    return temp;
}
