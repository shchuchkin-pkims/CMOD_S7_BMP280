//BMP280 address 0x76 - address lsb pulled down.

#define BMP280_IIC_ADDR				0x76
#define BMP280_ID_REG_ADDR			0xD0


void BMP280_printTemp_UART();
double BMP280_TempConvertion();
