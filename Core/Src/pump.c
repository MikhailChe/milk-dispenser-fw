#include "pump.h"

static __IO uint64_t turnoffat = 0;

static __IO uint8_t pump_state = 0;

void pump_init(){
	pump_off();
}

void pump_on(){
	HAL_GPIO_WritePin(pump_config.port, pump_config.pin, GPIO_PIN_SET);
	pump_state = 1;
}

void pump_off(){
	HAL_GPIO_WritePin(pump_config.port, pump_config.pin, GPIO_PIN_RESET);
	pump_state = 0;
}

bool pump_should_turn_off(){
	return pump_state && (HAL_GetTick() > turnoffat);
}

void pump_schedule_turn_off(uint64_t timeout){
	turnoffat = HAL_GetTick() + timeout;
}
