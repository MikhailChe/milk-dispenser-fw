/*
 * interface.c
 *
 *  Created on: 5 февр. 2021 г.
 *      Author: Mikhail
 */
#include "interface.h"

#include <stdio.h>

#include "app_config.h"
#include "lvgl.h"
#include "pump.h"

#define MP 2
#define CANCEL "Cancel"

static lv_obj_t *main_window;
static lv_obj_t *settings_window;

static lv_obj_t *btnmtrx = NULL;

static const char *password_bts[] = { "0", "\n", "1", "2", "3", "\n", "4", "5",
		"6", "\n", "7", "8", "9", "\n", "OK", CANCEL, "" };

static lv_style_t style_modal;

static char password[10] = { 0 };
static uint8_t cur_pwd_chr = 0;

static const char *main_buttons[APP_BUTTONS_COUNT + 1];

static lv_obj_t *spinboxes[APP_BUTTONS_COUNT];

static void clear_spinboxes() {
	for (int i = 0; i < APP_BUTTONS_COUNT; i++) {
		spinboxes[i] = NULL;
	}
}

static void add_spinbox(lv_obj_t *sbox) {
	for (int i = 0; i < APP_BUTTONS_COUNT; i++) {
		if (spinboxes[i] == NULL) {
			spinboxes[i] = sbox;
			return;
		}
		if (spinboxes[i] == sbox) {
			return;
		}
	}
}

static int get_spinbox(lv_obj_t *sbox) {
	for (int i = 0; i < APP_BUTTONS_COUNT; i++) {
		if (spinboxes[i] == sbox) {
			return i;
		}
	}
	return -1;
}

static void save_button_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_SHORT_CLICKED) {
		struct tAppConfig *config = AppConfig_get();
		for (int i = 0; i < APP_BUTTONS_COUNT; i++) {
			if (spinboxes[i]) {
				config->buttons[i].time = lv_spinbox_get_value(spinboxes[i]);
			}
		}
		AppConfig_save();
	}
}

static void lv_spinbox_increment_event_cb(lv_obj_t *btn, lv_event_t e) {
	lv_obj_t *parent = lv_obj_get_parent(btn);
	lv_obj_t *btn1 = lv_obj_get_child_back(parent, NULL);
	lv_obj_t *spinbox = lv_obj_get_child_back(parent, btn1);

	if (e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_increment(spinbox);

		struct tAppConfig *config = AppConfig_get();
		config->buttons[get_spinbox(spinbox)].time = lv_spinbox_get_value(
				spinbox);
	}
}

static void lv_spinbox_decrement_event_cb(lv_obj_t *btn, lv_event_t e) {
	lv_obj_t *parent = lv_obj_get_parent(btn);
	lv_obj_t *btn1 = lv_obj_get_child_back(parent, NULL);
	lv_obj_t *spinbox = lv_obj_get_child_back(parent, btn1);

	if (e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_decrement(spinbox);
		struct tAppConfig *config = AppConfig_get();
		config->buttons[get_spinbox(spinbox)].time = lv_spinbox_get_value(
				spinbox);
	}
}

static void create_spinbox(lv_obj_t *parent, struct tButtonConfig *config) {
	lv_obj_t *container = lv_cont_create(parent, NULL);
	lv_obj_set_width(container, 250);
	lv_cont_set_layout(container, LV_LAYOUT_CENTER);
	lv_cont_set_fit(container, LV_FIT_TIGHT);
	lv_obj_set_height_margin(container, MP);
	lv_obj_set_width_margin(container, MP);
	lv_obj_set_height_fit(container, MP);
	lv_obj_set_width_fit(container, MP);

	lv_obj_t *btn = lv_btn_create(container, NULL);

	lv_obj_t *spinbox = lv_spinbox_create(container, NULL);
	add_spinbox(spinbox);

	lv_spinbox_set_digit_format(spinbox, 5, 0);
	lv_spinbox_set_range(spinbox, 50, 60000);
	lv_spinbox_set_step(spinbox, 50);
	lv_spinbox_set_value(spinbox, config->time);

	lv_textarea_set_cursor_hidden(spinbox, true);

	lv_obj_set_width(spinbox, 125);
	lv_obj_align(spinbox, NULL, LV_ALIGN_CENTER, 0, 0);

	lv_coord_t h = lv_obj_get_height(spinbox);

	lv_obj_set_size(btn, h, h);
	lv_theme_apply(btn, LV_THEME_SPINBOX_BTN);
	lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
	LV_SYMBOL_PLUS);
	lv_obj_set_event_cb(btn, lv_spinbox_increment_event_cb);

	btn = lv_btn_create(container, btn);
	lv_obj_set_size(btn, h, h);
	lv_theme_apply(btn, LV_THEME_SPINBOX_BTN);
	lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
	LV_SYMBOL_MINUS);
	lv_obj_set_event_cb(btn, lv_spinbox_decrement_event_cb);
}

static void cancel_pouring_cb(struct _lv_obj_t *obj, lv_event_t event) {
	if (event == LV_EVENT_VALUE_CHANGED) {
		pump_off();
	}
}

static void create_pouring_popup(lv_obj_t *parent, const char *button_text) {
	lv_obj_t *popup = lv_msgbox_create(parent, NULL);

	int count = sprintf(NULL, "Pouring (%s)", button_text);
	char msg_text[count + 1];
	sprintf(msg_text, "Pouring (%s)", button_text);

	lv_msgbox_set_text(popup, msg_text);
	static const char *btns2[] = { "STOP", "" };
	lv_msgbox_add_btns(popup, btns2);

	uint32_t period = AppConfig_delay_for_button(button_text);
	lv_msgbox_start_auto_close(popup, period);
	pump_schedule_turn_off(period);
	pump_on();
	lv_obj_set_event_cb(popup, cancel_pouring_cb);
}

static void create_pouring_popup_from_settings_cb(struct _lv_obj_t *obj,
		lv_event_t event) {
	if (event == LV_EVENT_CLICKED) {
		lv_obj_t *label = lv_obj_get_child(obj, NULL);
		const char *label_text = lv_label_get_text(label);
		create_pouring_popup(lv_scr_act(), label_text);
	}
}

static void create_settings_pour_column(lv_obj_t *parent,
		struct tButtonConfig *config) {
	lv_obj_t *container = lv_cont_create(parent, NULL);
	lv_obj_set_width(container, 250);
	lv_cont_set_layout(container, LV_LAYOUT_CENTER);
	lv_cont_set_fit(container, LV_FIT_TIGHT);
	lv_obj_set_height_margin(container, MP);
	lv_obj_set_width_margin(container, MP);
	lv_obj_set_height_fit(container, MP);
	lv_obj_set_width_fit(container, MP);

	create_spinbox(container, config);

	lv_obj_t *btn = lv_btn_create(container, NULL);
	lv_obj_set_width(btn, 125);

	lv_obj_t *label = lv_label_create(btn, NULL);
	lv_label_set_text(label, config->name);

	lv_obj_set_event_cb(btn, create_pouring_popup_from_settings_cb);
}
static void password_popup_cb(struct _lv_obj_t *obj, lv_event_t event) {
	if (event == LV_EVENT_DELETE) {
		/* Delete the parent modal background */
		lv_obj_del_async(lv_obj_get_parent(obj));
	} else if (event == LV_EVENT_VALUE_CHANGED) {
		/* A button was clicked */
		password[cur_pwd_chr] = lv_msgbox_get_active_btn_text(obj)[0];
		cur_pwd_chr++;

		int password_compare = AppConfig_compare_password(password);

		bool many_letters = cur_pwd_chr >= (sizeof(password) - 1);

		if (many_letters || (password_compare == 0)) {
			lv_msgbox_start_auto_close(obj, 0);
			if (password_compare == 0) {
				create_settings_interface(AppConfig_get());
			}
		}
	}
}

void create_password_popup(lv_obj_t *parent) {
	memset(password, 0, sizeof(password));
	cur_pwd_chr = 0;

	lv_style_init(&style_modal);
	lv_style_set_bg_color(&style_modal, LV_STATE_DEFAULT, LV_COLOR_BLACK);

	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_reset_style_list(obj, LV_OBJ_PART_MAIN);
	lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style_modal);
	lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_size(obj, LV_HOR_RES, LV_VER_RES);

	lv_obj_t *popup = lv_msgbox_create(obj, NULL);

	lv_msgbox_set_text(popup, "Enter password:");
	lv_msgbox_add_btns(popup, password_bts);

	lv_obj_set_pos(popup, 0, 0);

	lv_msgbox_start_auto_close(popup, 65535U);
	lv_obj_set_event_cb(popup, password_popup_cb);
}

static void settings_btn_cb(struct _lv_obj_t *obj, lv_event_t event) {
	if (event == LV_EVENT_CLICKED) {
		create_password_popup(lv_scr_act());
	}
}

static void create_pouring_popup_from_main_cb(struct _lv_obj_t *obj,
		lv_event_t event) {
	if (event == LV_EVENT_CLICKED) {
		create_pouring_popup(lv_scr_act(),
				lv_btnmatrix_get_active_btn_text(btnmtrx));
	}
}

void create_main_interface(struct tAppConfig *config) {
	lv_obj_t *window = lv_win_create(lv_scr_act(), NULL);

	lv_win_set_title(window, "Got milk?");

	lv_obj_t *settings_btn = lv_win_add_btn(window, LV_SYMBOL_SETTINGS);
	lv_obj_set_event_cb(settings_btn, settings_btn_cb);
	lv_win_set_scrollbar_mode(window, LV_SCROLLBAR_MODE_OFF);

	lv_win_set_layout(window, LV_LAYOUT_PRETTY_TOP);

	int numbuttons = sizeof(config->buttons) / sizeof(*(config->buttons));

	btnmtrx = lv_btnmatrix_create(window, NULL);

	for (int i = 0; i < numbuttons; i++) {
		main_buttons[i] = config->buttons[i].name;
	}
	main_buttons[numbuttons] = "";

	lv_btnmatrix_set_map(btnmtrx, main_buttons);

	lv_obj_set_event_cb(btnmtrx, create_pouring_popup_from_main_cb);

	main_window = window;
}

void create_settings_interface(struct tAppConfig *config) {
	lv_obj_t *window = lv_win_create(lv_scr_act(), NULL);

	lv_win_set_title(window, "Setting");

	lv_obj_t *save_btn = lv_win_add_btn(window, LV_SYMBOL_SAVE);
	lv_obj_set_event_cb(save_btn, save_button_cb);
	lv_obj_t *close_btn = lv_win_add_btn(window, LV_SYMBOL_CLOSE);
	lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);
	lv_win_set_scrollbar_mode(window, LV_SCROLLBAR_MODE_OFF);

	lv_win_set_layout(window, LV_LAYOUT_PRETTY_TOP);

	int numbuttons = sizeof(config->buttons) / sizeof(*(config->buttons));

	clear_spinboxes();
	for (int i = 0; i < numbuttons; i++) {
		create_settings_pour_column(window, &(config->buttons[i]));
	}

	settings_window = window;
}

