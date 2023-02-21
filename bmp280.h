//BMP280 address 0x76 - address lsb pulled down.

#define BMP280_IIC_ADDR				0x76
#define BMP280_ID_REG_ADDR			0xD0


void BMP280_printTemp_UART();
void BMP280_ReadCalibConst();
void BMP280_ReadTempValues();
double BMP280_TempConvertion();
