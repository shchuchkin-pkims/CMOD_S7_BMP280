/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief BMP280 driver
 * 
 * Commands:
 *  get_temp
 * 
 * @version 0.1
 * @date 2025-07-03
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "bmp280.h" // BMP280 library
#include "xgpio.h"  // GPIO functions library
#include "xiic.h"
#include "xil_exception.h"
#include "xparameters.h" // IP-blocks parameters library
#include "xstatus.h"
#include "xuartlite.h"


#define MST_IIC_BASE_ADDR XPAR_IIC_0_BASEADDR

XGpio gpio;  // GPIO structure
XUartLite uart;
u32 led = 0; // LED state counter

/**
 * @brief
 *
 * @return int
 */
int main()
{
    // :toDo make this var's as one array?
    u32 digT1Lsb = 0; // compensation parameters for temperature
    u32 digT1Msb = 0;
    u32 digT2Lsb = 0;
    u32 digT2Msb = 0;
    u32 digT3Lsb = 0;
    u32 digT3Msb = 0;
    u32 tempXlsb = 0; // temperature raw data
    u32 tempLsb = 0;
    u32 tempMsb = 0;
    double temp; // Temperature value

    char recvBuf[32];
    u32 recvIndex = 0;
    char txBuf[64];
    u32 numOfRxBytes;
    u8 ch;

    XGpio_Initialize(&gpio, XPAR_AXI_GPIO_0_BASEADDR);  // GPIO initialization
    XUartLite_Initialize(&uart, XPAR_XUARTLITE_0_BASEADDR);
    XGpio_DiscreteWrite(&gpio, 1, 0);                // Turn off all LEDs;
    bmp280ReadDeviceID();                            // Read BMP280 device ID and print it via UART
    (void)i2c_write(BMP280_IIC_ADDR, 0xF4, 0xB7, 1); // Configurate BMP280: normal mod, oversampling
    bmp280ReadCalibConst(&digT1Lsb,
                         &digT1Msb,
                         &digT2Lsb,
                         &digT2Msb,
                         &digT3Lsb,
                         &digT3Msb); // Read calibration constants

    while (1) {
        // // Reading temperature values
        // bmp280ReadTempValues(&tempXlsb, &tempLsb, &tempMsb);
        // temp = bmp280TempConvertion(tempXlsb, tempLsb, tempMsb, digT1Lsb, digT1Msb, digT2Lsb,
        // digT2Msb, digT3Lsb, digT3Msb); xil_printf("Temperature = % C\n\r", (int)temp);

        numOfRxBytes = XUartLite_Recv(&uart, &ch, 1); // Read 1 char
        // UART commands handler
        if (numOfRxBytes == 1) { // If there is data in FIFO
            if ((ch == '\n') || (ch == '\r')) {
                recvBuf[recvIndex] = '\0'; // End of the line
                if ((strcmp(recvBuf, "get_temp")) == 0) {
                    bmp280ReadTempValues(&tempXlsb, &tempLsb, &tempMsb); // Read temperature values
                    temp = bmp280TempConvertion(tempXlsb,
                                                tempLsb,
                                                tempMsb,
                                                digT1Lsb,
                                                digT1Msb,
                                                digT2Lsb,
                                                digT2Msb,
                                                digT3Lsb,
                                                digT3Msb);
                    xil_printf("Temperature = %d.%02d C\n\r", (int)temp, (int)((temp - (int)temp)*100)); // xil_printf doesn't support floating numbers
                }
                recvIndex = 0; // Clear buffer for new command
                memset(recvBuf, 0, sizeof(recvBuf));
            } else {
                if (recvIndex < (sizeof(recvBuf) - 1)) { // Add char to buffer
                    recvBuf[recvIndex] = ch;
                    recvIndex++;
                } else { // Reset when overflow
                    recvIndex = 0;
                    memset(recvBuf, 0, sizeof(recvBuf));
                }
            }
        }
    }
    return 0;
}
