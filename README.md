# LIS3MDL_SPI
Driver library for LIS3MDL magnetometer using SPI
>TODO: Add SPI DMA functionality

# main.c
>1: To use this driver:

    LIS3MDL_t LIS3MDL_Magneto; // give any struct name

>2: Call init:

    LIS3MDL_Init(&LIS3MDL_Magneto, &hspi2, LIS3MDL_Scale_4G, LIS3MDL_MODE_UHP, LIS3MDL_ODR_7, true);
  
>3: Read in while loop:

    while (1)
    {
	     LIS3MDL_ReadMag(&LIS3MDL_Magneto, &hspi2);
       HAL_Delay(100);
    }
