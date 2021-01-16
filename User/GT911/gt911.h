/*
 * gt911.h
 *
 *  Created on: Jan 13, 2021
 *      Author: Mikhail
 */

#ifndef GT911_GT911_H_
#define GT911_GT911_H_

#include <stdint.h>
#include <stm32h7xx.h>


#define GT911_MAX_WIDTH		1024    	//Touchscreen pad max width
#define GT911_MAX_HEIGHT		600			//Touchscreen pad max height

#define GT911_CMD_WR_ADDR		0XBA					//Write data command
#define GT911_CMD_RD_ADDR   	0XBB					//Read data command

//The maximum number of points supported by the capacitive touch screen
#define GT911_MAX_TOUCH    5

#define GT911_COMMAND_REG   				0x8040	/* Real-time command */
#define GT911_CONFIG_REG						0x8047	/* Configuration parameter register */
#define GT911_PRODUCT_ID_REG 				0x8140 	/* Product ID */
#define GT911_FIRMWARE_VERSION_REG  0x8144  /* Firmware version number */
#define GT911_READY_REG 					0x814E	/* Coordinates ready register */
#define GT911_READY_REG_BUFFER_STATUS 0x80 // buffer is ready = 1
#define GT911_READY_REG_LARGE_DETECT 0x40 // large-area touch = 1
#define GT911_READY_REG_TOUCH_KEY 0x10 // have touch key = 1, released = 0
#define GT911_READY_REG_NUMBER_OF_TOUCH_POINTS 0x0F // number of touch points detected

#define GT911_COORDINATES_REG_START 0x8150


struct GT911_Config{
	GPIO_TypeDef* resetPort;
	uint16_t resetPin;

	GPIO_TypeDef* interruptPort;
	uint16_t interruptPin;

	I2C_HandleTypeDef* i2c;
};


#pragma pack(1)
typedef __PACKED_STRUCT  {
	uint8_t point_x_low;
	uint8_t point_x_high;
	uint8_t point_y_low;
	uint8_t point_y_high;
	uint8_t point_size_low;
	uint8_t point_size_high;
	uint16_t __reserved__;
} GT911_TouchInfo;
#pragma pack()


struct GT911
{
	struct GT911_Config config;

	volatile uint8_t Touch;

	uint8_t TouchpointFlag;
	uint8_t TouchCount;

	GT911_TouchInfo Touches[GT911_MAX_TOUCH];
};
struct GT911 GT911_Init(struct GT911_Config);
uint8_t GT911_Scan(volatile struct GT911* gt911, uint32_t timeout);


#endif /* GT911_GT911_H_ */
