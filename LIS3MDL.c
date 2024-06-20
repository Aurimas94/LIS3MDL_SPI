/*
 * LIS3MDL.c
 *
 *  Created on: Jun 16, 2024
 *      Author: cezis
 */

#include "LIS3MDL.h"


void LIS3MDL_Init(LIS3MDL_t *device, SPI_HandleTypeDef *hspi, LIS3MDL_Scale_t scale, LIS3MDL_OperativeMode_t mode, LIS3MDL_DataRate_t odr, bool readTemp)
{
    uint8_t data;

    device->scale = (LIS3MDL_Scale_t)scale;
    device->Temperature.EnableMeas = readTemp;
    readBytes(hspi, LIS3MDL_WHO_AM_I_REG, &device->ID, 1);
    HAL_Delay(1);
    readBytes(hspi, LIS3MDL_CTRL_REG1, &data, 1); // read existing parameters

    data &= ~0xFE; //bit mask
    data |= ((uint8_t)mode << 5) | (uint8_t)odr; // shift mode selection and data rate bits
    if(device->Temperature.EnableMeas) data |= 0x80; // Enable Temperature measurement

    writeByte(hspi, LIS3MDL_CTRL_REG1, data);

    //HAL_Delay(100);
    readBytes(hspi, LIS3MDL_CTRL_REG2, &data, 1);
    data &= ~0x60; // change only FS1 and FS0 bits
    data |= (uint8_t)scale;
    writeByte(hspi, LIS3MDL_CTRL_REG2, data);

    //HAL_Delay(100);
    readBytes(hspi, LIS3MDL_CTRL_REG3, &data, 1);
    data &= ~0x03;
    data |= 0x00;
    writeByte(hspi, LIS3MDL_CTRL_REG3, data);
    //HAL_Delay(100);
    readBytes(hspi, LIS3MDL_CTRL_REG4, &data, 1);
    data &= ~0x0C; // change only OMZ1 and OMX0  bits
    data |= (uint8_t)(mode << 2);
    writeByte(hspi, LIS3MDL_CTRL_REG4, data);
    //HAL_Delay(100);
    readBytes(hspi, LIS3MDL_CTRL_REG5, &data, 1);
    data &= ~0xC0;
    data |= 0x00;
    writeByte(hspi, LIS3MDL_CTRL_REG5, data);
    //HAL_Delay(100);
    readBytes(hspi, LIS3MDL_INT_CFG, &data, 1);
    data &= 0x00;
    data |= 0x00;
    writeByte(hspi, LIS3MDL_INT_CFG, data);
    //HAL_Delay(100);
}

void LIS3MDL_ReadMag(LIS3MDL_t *device, SPI_HandleTypeDef *hspi)
{
    uint8_t dataXL;
    uint8_t dataXH;
    uint8_t dataYL;
    uint8_t dataYH;
    uint8_t dataZL;
    uint8_t dataZH;

#ifdef USE_DMA

    if(device->Temperature.EnableMeas) // if temp measurement enabled
    {
		readBytes_DMA(hspi, LIS3MDL_TEMP_OUT_L, &device->Temperature.dataL, 1);
		readBytes_DMA(hspi, LIS3MDL_TEMP_OUT_H, &device->Temperature.dataH, 1);

		device->Temperature.Temp_raw = ((int16_t)device->Temperature.dataL << 8) | device->Temperature.dataH;
		device->Temperature.Temp_C = (float)(device->Temperature.Temp_raw / 8.0f)/100;
    }

    readBytes_DMA(hspi, LIS3MDL_OUTX_L, &dataXL, 1);
    readBytes_DMA(hspi, LIS3MDL_OUTX_H, &dataXH, 1);

    readBytes_DMA(hspi, LIS3MDL_OUTY_L, &dataYL, 1);
    readBytes_DMA(hspi, LIS3MDL_OUTY_H, &dataYH, 1);

    readBytes_DMA(hspi, LIS3MDL_OUTZ_L, &dataZL, 1);
    readBytes_DMA(hspi, LIS3MDL_OUTZ_H, &dataZH, 1);

    device->X_vRaw = ((int16_t)dataXH << 8) | dataXL;
    device->Y_vRaw = ((int16_t)dataYH << 8) | dataYL;
    device->Z_vRaw = ((int16_t)dataZH << 8) | dataZL;

    switch (device->scale)
    {
        case LIS3MDL_Scale_4G:
        	device->X_vector = (float)(device->X_vRaw / 6842.0f);
        	device->Y_vector = (float)(device->Y_vRaw / 6842.0f);
        	device->Z_vector = (float)(device->Z_vRaw / 6842.0f);
            break;
        case LIS3MDL_Scale_8G:
        	device->X_vector = (float)(device->X_vRaw / 3421.0f);
        	device->Y_vector = (float)(device->Y_vRaw / 3421.0f);
        	device->Z_vector = (float)(device->Z_vRaw / 3421.0f);
            break;
        case LIS3MDL_Scale_12G:
        	device->X_vector = (float)(device->X_vRaw / 2281.0f);
        	device->Y_vector = (float)(device->Y_vRaw / 2281.0f);
        	device->Z_vector = (float)(device->Z_vRaw / 2281.0f);
            break;
        case LIS3MDL_Scale_16G:
        	device->X_vector = (float)(device->X_vRaw / 1711.0f);
        	device->Y_vector = (float)(device->Y_vRaw / 1711.0f);
        	device->Z_vector = (float)(device->Z_vRaw / 1711.0f);
            break;
    }
    getCardinal(device); // get directions


#else

    if(device->Temperature.EnableMeas) // if temp measurement enabled
    {
		readBytes(hspi, LIS3MDL_TEMP_OUT_L, &device->Temperature.dataL, 1);
		readBytes(hspi, LIS3MDL_TEMP_OUT_H, &device->Temperature.dataH, 1);

		device->Temperature.Temp_raw = ((int16_t)device->Temperature.dataL << 8) | device->Temperature.dataH;
		device->Temperature.Temp_C = (float)(device->Temperature.Temp_raw / 8.0f)/100;
    }

    readBytes(hspi, LIS3MDL_OUTX_L, &dataXL, 1);
    readBytes(hspi, LIS3MDL_OUTX_H, &dataXH, 1);

    readBytes(hspi, LIS3MDL_OUTY_L, &dataYL, 1);
    readBytes(hspi, LIS3MDL_OUTY_H, &dataYH, 1);

    readBytes(hspi, LIS3MDL_OUTZ_L, &dataZL, 1);
    readBytes(hspi, LIS3MDL_OUTZ_H, &dataZH, 1);

    device->X_vRaw = ((int16_t)dataXH << 8) | dataXL;
    device->Y_vRaw = ((int16_t)dataYH << 8) | dataYL;
    device->Z_vRaw = ((int16_t)dataZH << 8) | dataZL;

    switch (device->scale)
    {
        case LIS3MDL_Scale_4G:
        	device->X_vector = (float)(device->X_vRaw / 6842.0f);
        	device->Y_vector = (float)(device->Y_vRaw / 6842.0f);
        	device->Z_vector = (float)(device->Z_vRaw / 6842.0f);
            break;
        case LIS3MDL_Scale_8G:
        	device->X_vector = (float)(device->X_vRaw / 3421.0f);
        	device->Y_vector = (float)(device->Y_vRaw / 3421.0f);
        	device->Z_vector = (float)(device->Z_vRaw / 3421.0f);
            break;
        case LIS3MDL_Scale_12G:
        	device->X_vector = (float)(device->X_vRaw / 2281.0f);
        	device->Y_vector = (float)(device->Y_vRaw / 2281.0f);
        	device->Z_vector = (float)(device->Z_vRaw / 2281.0f);
            break;
        case LIS3MDL_Scale_16G:
        	device->X_vector = (float)(device->X_vRaw / 1711.0f);
        	device->Y_vector = (float)(device->Y_vRaw / 1711.0f);
        	device->Z_vector = (float)(device->Z_vRaw / 1711.0f);
            break;
    }
    getCardinal(device); // get directions
#endif
}
void getCardinal(LIS3MDL_t *device)
{

	float headingRadians = atan2(device->Y_vector, device->X_vector);
	float headingDegrees = headingRadians * 180 / PI;


	headingDegrees += declinationAngle;

	if (headingDegrees < 0)
	{
		headingDegrees += 360;
	}

	device->Heading = headingDegrees;
	if (headingDegrees > 348.75 || headingDegrees < 11.25) {
		sprintf(device->cardinal, "N  %f", headingDegrees); // siaure
		LED1_ON;
		LED2_OFF;
		LED3_OFF;
		LED4_OFF;
	}
	else if (headingDegrees > 11.25 && headingDegrees < 33.75) {
		sprintf(device->cardinal, "NNE  %f", headingDegrees); // siaure- siaures rytai
	}
	else if (headingDegrees > 33.75 && headingDegrees < 56.25) {
		sprintf(device->cardinal, "NE  %f", headingDegrees); //siaures rytai
	}
	else if (headingDegrees > 56.25 && headingDegrees < 78.75) {
		sprintf(device->cardinal, "ENE  %f", headingDegrees);
	}
	else if (headingDegrees > 78.75 && headingDegrees < 101.25) {
		sprintf(device->cardinal, "E  %f", headingDegrees);
		LED1_OFF;
		LED2_ON;
		LED3_OFF;
		LED4_OFF;
	}
	else if (headingDegrees > 101.25 && headingDegrees < 123.75) {
		sprintf(device->cardinal, "ESE  %f", headingDegrees);
	}
	else if (headingDegrees > 123.75 && headingDegrees < 146.25) {
		sprintf(device->cardinal, "SE  %f", headingDegrees);
	}
	else if (headingDegrees > 146.25 && headingDegrees < 168.75) {
		sprintf(device->cardinal, "SSE  %f", headingDegrees);
	}
	else if (headingDegrees > 168.75 && headingDegrees < 191.25) {
		sprintf(device->cardinal, "S  %f", headingDegrees);
		LED1_OFF;
		LED2_OFF;
		LED3_ON;
		LED4_OFF;
	}
	else if (headingDegrees > 191.25 && headingDegrees < 213.75) {
		sprintf(device->cardinal, "SSW  %f", headingDegrees);
	}
	else if (headingDegrees > 213.75 && headingDegrees < 236.25) {
		sprintf(device->cardinal, "SW  %f", headingDegrees);
	}
	else if (headingDegrees > 236.25 && headingDegrees < 258.75) {
		sprintf(device->cardinal, "WSW  %f", headingDegrees);
	}
	else if (headingDegrees > 258.75 && headingDegrees < 281.25) {
		sprintf(device->cardinal, "W  %f", headingDegrees);
		LED1_OFF;
		LED2_OFF;
		LED3_OFF;
		LED4_ON;
	}
	else if (headingDegrees > 281.25 && headingDegrees < 303.75) {
		sprintf(device->cardinal, "WNW  %f", headingDegrees);
	}
	else if (headingDegrees > 303.75 && headingDegrees < 326.25) {
		sprintf(device->cardinal, "NW  %f", headingDegrees);
	}
	else if (headingDegrees > 326.25 && headingDegrees < 348.75) {
		sprintf(device->cardinal, "NNW  %f", headingDegrees);
	}

}
HAL_StatusTypeDef writeByte(SPI_HandleTypeDef *hspi, uint8_t register_addr, uint8_t data)
{
	//TODO: ERROR Handling
    uint8_t buffer[2];
    buffer[0] = register_addr;
    buffer[1] = data;

    LIS3MDL_CSE;

    if (HAL_SPI_Transmit(hspi, (uint8_t *)buffer, 2, 1000) != HAL_OK)
    {
    	return HAL_ERROR;
    }
    LIS3MDL_CSD;

    return HAL_OK;
}

HAL_StatusTypeDef readBytes(SPI_HandleTypeDef *hspi, uint8_t register_addr, uint8_t *data, uint16_t dataCount)
{

	//TODO: ERROR Handling
	uint8_t writeAddr = register_addr | 0x80; // SET RW BIT TO READING (1)

	LIS3MDL_CSE;

	if (HAL_SPI_Transmit(hspi, &writeAddr, 1, 1000) != HAL_OK)
	{
		return HAL_ERROR;
	}

	if(HAL_SPI_Receive(hspi, data, dataCount, 1000) != HAL_OK)
	{
		return HAL_ERROR;
	}
	LIS3MDL_CSD;

    return HAL_OK;
}
HAL_StatusTypeDef writeByte_DMA(SPI_HandleTypeDef *hspi, uint8_t register_addr, uint8_t data)
{
	//TODO: NOT_INPLEMENTED

    uint8_t buffer[2];
    buffer[0] = register_addr;
    buffer[1] = data;

    LIS3MDL_CSE;

    HAL_StatusTypeDef status =  HAL_SPI_Transmit_DMA(hspi, (uint8_t *)buffer, 2);

    while(HAL_SPI_GetState(hspi) != HAL_SPI_STATE_READY){;}

    LIS3MDL_CSD;

    return status;
}
HAL_StatusTypeDef readBytes_DMA(SPI_HandleTypeDef *hspi, uint8_t register_addr, uint8_t *data, uint16_t dataCount)
{
	//TODO: NOT INPLEMENTED
	uint8_t writeAddr = register_addr | 0x80; // SET RW BIT TO READING (1)

	LIS3MDL_CSE;

	if (writeByte_DMA(hspi, writeAddr, 1) != HAL_OK)
	{
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_SPI_Receive_DMA(hspi, data, dataCount);
	while(HAL_SPI_GetState(hspi) != HAL_SPI_STATE_READY){;}

	LIS3MDL_CSD;
	return status;

}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef * hspi)
{

}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef * hspi)
{

}


/* EOF */
