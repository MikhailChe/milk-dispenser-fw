/*
 * interface.h
 *
 *  Created on: 5 февр. 2021 г.
 *      Author: Mikhail
 */

#ifndef INC_INTERFACE_H_
#define INC_INTERFACE_H_

#include "app_config.h"
#include "lvgl.h"

void create_main_interface(struct tAppConfig *config);
lv_obj_t* create_volumes_interface(lv_obj_t*parent,struct tAppConfig *config);
lv_obj_t * create_change_password_window(lv_obj_t*parent,struct tAppConfig *config);
lv_obj_t* create_settings_menu(lv_obj_t* parent, struct tAppConfig* config);
lv_obj_t* create_calibration_window(lv_obj_t *parent, struct tAppConfig *config);

#endif /* INC_INTERFACE_H_ */
