/*
 * LIS3MDL.h
 *
 *  Created on: Jun 16, 2024
 *      Author: cezis
 */

#ifndef INC_LIS3MDL_H_
#define INC_LIS3MDL_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>
#include "main.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define PI 						3.1415
#define declinationAngle  		8.283 // Lietuvos declination angle

/*
 * UNCOMMENT TO USE SPI DMA
 */

//#define USE_DMA

/*
 *
 */

/************** LIS3MDL Register  *******************/

#define LIS3MDL_WHO_AM_I_REG    0x0F
#define LIS3MDL_CTRL_REG1       0x20
#define LIS3MDL_CTRL_REG2       0x21
#define LIS3MDL_CTRL_REG3       0x22
#define LIS3MDL_CTRL_REG4       0x23
#define LIS3MDL_CTRL_REG5       0x24
#define LIS3MDL_STATUS_REG      0x27
#define LIS3MDL_OUTX_L          0x28
#define LIS3MDL_OUTX_H          0x29
#define LIS3MDL_OUTY_L          0x2A
#define LIS3MDL_OUTY_H          0x2B
#define LIS3MDL_OUTZ_L          0x2C
#define LIS3MDL_OUTZ_H          0x2D
#define LIS3MDL_TEMP_OUT_L      0x2E
#define LIS3MDL_TEMP_OUT_H      0x2F
#define LIS3MDL_INT_CFG         0x30
#define LIS3MDL_INT_SRC         0x31
#define LIS3MDL_INT_THS_L       0x32
#define LIS3MDL_INT_THS_H       0x33

/* ____________________________________________________________________________
 * CTRL_REG1  			|TEMP_EN | OM1 | OM0 | DO2 | DO1 | DO0 | FAST_ODR | ST |
 * -----------------------------------------------------------------------------
 * default					0		LP	  LP   0x10  0x10  0x10      0       0
 *  */
typedef enum
{
    LIS3MDL_MODE_LP = 0x00,
    LIS3MDL_MODE_MP = 0x01,
    LIS3MDL_MODE_HP = 0x02,
    LIS3MDL_MODE_UHP = 0x03
} LIS3MDL_OperativeMode_t;

typedef struct
{
	uint16_t Temp_raw;
	float Temp_C;
	bool EnableMeas;
	uint8_t dataL;
	uint8_t dataH;

} LIS3MDL_Temp_t;

typedef enum
{
    LIS3MDL_ODR_0 = 0x00,   /* 0.625 Hz */
    LIS3MDL_ODR_1 = 0x04,   /* 1.25 Hz  */
    LIS3MDL_ODR_2 = 0x08,   /* 2.5 Hz   */
    LIS3MDL_ODR_3 = 0x0C,   /* 5 Hz     */
    LIS3MDL_ODR_4 = 0x10,   /* 10 Hz    */
    LIS3MDL_ODR_5 = 0x14,   /* 20 Hz    */
    LIS3MDL_ODR_6 = 0x18,   /* 40 Hz    */
    LIS3MDL_ODR_7 = 0x1C,   /* 80 Hz    */
    LIS3MDL_FAST_ODR = 0x02

} LIS3MDL_DataRate_t;

typedef enum
{
    LIS3MDL_Scale_4G = 0x00,    /* ±4 G     */
    LIS3MDL_Scale_8G = 0x20,    /* ±8 G     */
    LIS3MDL_Scale_12G = 0x40,   /* ±12 G    */
    LIS3MDL_Scale_16G = 0x60    /* ±16 G    */
} LIS3MDL_Scale_t;


typedef struct
{
	uint8_t ID;

    float X_vector; // x-vector value
    float Y_vector;
    float Z_vector;

    int16_t X_vRaw; // X-vector raw value
    int16_t Y_vRaw;
    int16_t Z_vRaw;

    float Heading; // X-Y vector angle in degrees
    LIS3MDL_Scale_t scale;
    LIS3MDL_Temp_t Temperature;
    char cardinal[20];
} LIS3MDL_t;



void LIS3MDL_Init(LIS3MDL_t *device, SPI_HandleTypeDef *hspi, LIS3MDL_Scale_t scale, LIS3MDL_OperativeMode_t mode, LIS3MDL_DataRate_t odr, bool readTemp);
HAL_StatusTypeDef writeByte(SPI_HandleTypeDef *hspi, uint8_t register_addr, uint8_t data);
HAL_StatusTypeDef readBytes(SPI_HandleTypeDef *hspi, uint8_t register_addr, uint8_t *data, uint16_t dataCount);
void LIS3MDL_ReadMag(LIS3MDL_t *device, SPI_HandleTypeDef *hspi);
void getCardinal(LIS3MDL_t *device);
HAL_StatusTypeDef writeByte_DMA(SPI_HandleTypeDef *hspi, uint8_t register_addr, uint8_t data);
HAL_StatusTypeDef readBytes_DMA(SPI_HandleTypeDef *hspi, uint8_t register_addr, uint8_t *data, uint16_t dataCount);
#ifdef __cplusplus
}
#endif
#endif /* INC_LIS3MDL_H_ */
