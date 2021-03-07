/*
 * hw_buttons.h
 *
 *  Created on: Mar 7, 2021
 *      Author: Mikhail
 */

#ifndef INC_HW_BUTTONS_H_
#define INC_HW_BUTTONS_H_
#include <stdbool.h>

#include "lvgl.h"

lv_indev_drv_t indev_hw_buttons_drv;
lv_indev_t *indev_hw_buttons;
bool hw_buttons_read(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data);

#endif /* INC_HW_BUTTONS_H_ */
