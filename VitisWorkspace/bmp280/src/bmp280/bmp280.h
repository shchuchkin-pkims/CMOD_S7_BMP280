/**
 * @file bmp280.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef SRC_BMP280_BMP280_H_
#define SRC_BMP280_BMP280_H_

#include "xbasic_types.h"
#include "xiic.h"

// BMP280 address 0x76 - address lsb pulled down on module board.

#define BMP280_IIC_ADDR 0x76
#define BMP280_ID_REG_ADDR 0xD0

void bmp280ReadDeviceID();
// void bmp280printTemp_UART();
void bmp280ReadCalibConst();
void bmp280ReadTempValues();
double bmp280TempConvertion();

#endif /* SRC_BMP280_BMP280_H_ */
