#include "hw_buttons.h"
#include "stm32h7xx_hal.h"

#define HW_BTN3_Pin GPIO_PIN_12
#define HW_BTN3_GPIO_Port GPIOB
#define HW_BTN2_Pin GPIO_PIN_13
#define HW_BTN2_GPIO_Port GPIOB
#define HW_BTN1_Pin GPIO_PIN_8
#define HW_BTN1_GPIO_Port GPIOD
#define HW_BTN0_Pin GPIO_PIN_9
#define HW_BTN0_GPIO_Port GPIOD

#define HW_BUTONS_COUNT 4

static GPIO_TypeDef *const button_ports[] = {
HW_BTN0_GPIO_Port, HW_BTN1_GPIO_Port, HW_BTN2_GPIO_Port, HW_BTN3_GPIO_Port, };

static uint16_t const button_pins[] = {
HW_BTN0_Pin, HW_BTN1_Pin, HW_BTN2_Pin, HW_BTN3_Pin, };

static int my_btn_read() {
	for (int i = 0; i < HW_BUTONS_COUNT; i++) {
		GPIO_TypeDef *port = button_ports[i];
		uint16_t pin = button_pins[i];
		GPIO_PinState state = HAL_GPIO_ReadPin(port, pin);
		if (GPIO_PIN_RESET == state) {
			return i;
		}
	}
	return -1;

}

bool hw_buttons_read(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
	static uint32_t last_btn = 0; /*Store the last pressed button*/
	int btn_pr = my_btn_read(); /*Get the ID (0,1,2...) of the pressed button*/
	if (btn_pr >= 0) { /*Is there a button press? (E.g. -1 indicated no button was pressed)*/
		last_btn = btn_pr; /*Save the ID of the pressed button*/
		data->state = LV_INDEV_STATE_PR; /*Set the pressed state*/
	} else {
		data->state = LV_INDEV_STATE_REL; /*Set the released state*/
	}

	data->btn_id = last_btn; /*Save the last button*/

	return false; /*No buffering now so no more data read*/
}
