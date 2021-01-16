#include "GT911.h"

#include <stdio.h>

static void GT911_Reset(struct GT911* gt911)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = gt911->config.interruptPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(gt911->config.interruptPort, &GPIO_InitStruct);

	HAL_GPIO_WritePin(gt911->config.resetPort, gt911->config.resetPin, GPIO_PIN_RESET);
	HAL_Delay(100);
	// Hold INT pin low to select 0xBA/0xBB address
	// TODO: implement selecting any address needed
	HAL_GPIO_WritePin(gt911->config.interruptPort, gt911->config.interruptPin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(gt911->config.resetPort, gt911->config.resetPin, GPIO_PIN_SET);
	HAL_Delay(200);

	GPIO_InitStruct.Pin = gt911->config.interruptPin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(gt911->config.interruptPort, &GPIO_InitStruct);

	HAL_Delay(100);
}


static HAL_StatusTypeDef GT911_WR_Reg(I2C_HandleTypeDef *hi2c, uint16_t reg, uint8_t *buf, uint8_t len){
	uint8_t buf_with_reg[256];
	buf_with_reg[0] = (reg >> 8)&0xFF;
	buf_with_reg[1] = reg&0xFF;
	for(uint8_t i=0; i<len; i++){
		buf_with_reg[i+2] = buf[i];
	}
	return HAL_I2C_Master_Transmit(hi2c, GT911_CMD_WR_ADDR, buf_with_reg, len+2, 10);
}

static HAL_StatusTypeDef GT911_RD_Reg(I2C_HandleTypeDef *hi2c, uint16_t reg,uint8_t *buf,uint8_t len){
	GT911_WR_Reg(hi2c, reg, buf, 0);
	return HAL_I2C_Master_Receive(hi2c, GT911_CMD_RD_ADDR, buf, len, 10);
}

static uint8_t GT911_Ready_Reg(I2C_HandleTypeDef *hi2c){
	uint8_t buf[1];
	GT911_RD_Reg(hi2c, GT911_READY_REG, buf, 1);
	return buf[0];
}

static HAL_StatusTypeDef GT911_Ready_Reg_Clear(I2C_HandleTypeDef *hi2c){
	uint8_t buf[1] = {0};
	return GT911_WR_Reg(hi2c, GT911_READY_REG, buf, 1);
}


HAL_StatusTypeDef GT911_Scan(volatile struct GT911* gt911, uint32_t timeout){
	uint32_t time_start = HAL_GetTick();
	if(!gt911->Touch){
		return HAL_OK;
	}
	gt911->Touch = 0;

	uint8_t ready_reg = 0;
	while(1){
		if(HAL_GetTick() - time_start > timeout){
			return HAL_TIMEOUT;
		}
		ready_reg = GT911_Ready_Reg(gt911->config.i2c);
		if (ready_reg & GT911_READY_REG_BUFFER_STATUS){
			break;
		}
		HAL_Delay(1);
	}

	const uint8_t touch_points = ready_reg & GT911_READY_REG_NUMBER_OF_TOUCH_POINTS;
	if(touch_points > GT911_MAX_TOUCH){
		return HAL_ERROR;
	}

	const uint16_t coordinates_buffer_size = touch_points * 8;
	uint8_t coordinates_buffer[coordinates_buffer_size];
	for(size_t i=0; i<coordinates_buffer_size; i++){
		coordinates_buffer[i] = 0;
	}

	if(GT911_RD_Reg(gt911->config.i2c, GT911_COORDINATES_REG_START, coordinates_buffer, coordinates_buffer_size-2)!=HAL_OK){
		return HAL_ERROR;
	}

	if(GT911_Ready_Reg_Clear(gt911->config.i2c)!=HAL_OK){
		return HAL_ERROR;
	}

	GT911_TouchInfo* touches = (GT911_TouchInfo*)coordinates_buffer;

	gt911->TouchpointFlag = ready_reg;
	gt911->TouchCount = touch_points;

	for(size_t i=0; i<touch_points; i++){
		gt911->Touches[i] = touches[i];
	}

	return HAL_OK;

}

struct GT911 GT911_Init(struct GT911_Config config)
{
	struct GT911  gt911 = {
		.config = config
	};
	gt911.config = config;
	printf("GT911_Init\r\n");
	GT911_Reset(&gt911);
	return gt911;
}

