/*
 * gt911.h
 *
 *  Created on: Jan 13, 2021
 *      Author: Mikhail
 */

#ifndef GT911_GT911_H_
#define GT911_GT911_H_


#define GT911_MAX_WIDTH		1024    	//Touchscreen pad max width
#define GT911_MAX_HEIGHT		600			//Touchscreen pad max height

#define CT_CMD_WR			0XBA					//Write data command
#define CT_CMD_RD   	0XBB					//Read data command

//The maximum number of points supported by the capacitive touch screen
#define CT_MAX_TOUCH    5

#define GT911_COMMAND_REG   				0x8040	/* Real-time command */
#define GT911_CONFIG_REG						0x8047	/* Configuration parameter register */
#define GT911_PRODUCT_ID_REG 				0x8140 	/* Product ID */
#define GT911_FIRMWARE_VERSION_REG  0x8144  /* Firmware version number */
#define GT911_READ_XY_REG 					0x814E	/* Coordinate register */

typedef struct
{
	uint8_t Touch;
	uint8_t TouchpointFlag;
	uint8_t TouchCount;

	uint8_t Touchkeytrackid[CT_MAX_TOUCH];
	uint16_t X[CT_MAX_TOUCH];
	uint16_t Y[CT_MAX_TOUCH];
	uint16_t S[CT_MAX_TOUCH];
}GT911_Dev;

void GT911_Init(void);
void GT911_Scan(void);


#endif /* GT911_GT911_H_ */
