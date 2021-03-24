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

#define CANCEL "Cancel"

static lv_obj_t *main_window;
lv_obj_t *main_button_labels[APP_BUTTONS_COUNT];

static lv_obj_t *volume_label;
static const char *volume_label_fmt = "Total volume: %d ml";
static uint64_t volume_total = 0;

static const char *password_bts[] = { "1", "2", "3", "\n", "4", "5", "6", "\n",
		"7", "8", "9", "\n", "0", CANCEL, "" };

static lv_style_t style_modal;

static char password[10] = { 0 };
static uint8_t cur_pwd_chr = 0;

static void pouring_popup_cb(struct _lv_obj_t *obj, lv_event_t event) {
	if (event == LV_EVENT_DELETE) {
		/* Delete the parent modal background */
		lv_obj_del_async(lv_obj_get_parent(obj));
	} else if (event == LV_EVENT_VALUE_CHANGED) {
		pump_off();
		lv_msgbox_start_auto_close(obj, 0);
	}
}

void add_volume(uint64_t volume) {
	volume_total += volume;
	lv_label_set_text_fmt(volume_label, volume_label_fmt, volume_total);
}

static void create_pouring_popup(lv_obj_t *parent,
		struct tButtonConfig *buttonConfig) {
	lv_style_init(&style_modal);
	lv_style_set_bg_color(&style_modal, LV_STATE_DEFAULT, LV_COLOR_BLACK);

	lv_obj_t *black_pie = lv_obj_create(parent, NULL);
	lv_obj_reset_style_list(black_pie, LV_OBJ_PART_MAIN);
	lv_obj_add_style(black_pie, LV_OBJ_PART_MAIN, &style_modal);
	lv_obj_set_pos(black_pie, 0, 0);
	lv_obj_set_size(black_pie, LV_HOR_RES, LV_VER_RES);

	lv_obj_t *popup = lv_msgbox_create(black_pie, NULL);

	int count = snprintf(NULL, 0, "Pouring %ld ml", buttonConfig->volume_ml);
	char msg_text[count + 1];
	sprintf(msg_text, "Pouring %ld ml", buttonConfig->volume_ml);

	lv_msgbox_set_text(popup, msg_text);
	static const char *btns2[] = { "STOP", "" };
	lv_msgbox_add_btns(popup, btns2);

	uint64_t volume = buttonConfig->volume_ml;
	uint64_t ml_per_min = AppConfig_get()->ml_per_min;

	const uint64_t ms_in_min = 60UL * 1000UL;

	uint32_t period = (volume * ms_in_min) / ml_per_min;
	lv_msgbox_start_auto_close(popup, period);
	add_volume(buttonConfig->volume_ml);
	pump_schedule_turn_off(period);
	pump_on();
	lv_obj_set_event_cb(popup, pouring_popup_cb);
}

static void create_pouring_popup_cb(struct _lv_obj_t *obj, lv_event_t event) {
	if (event == LV_EVENT_CLICKED) {
		struct tButtonConfig *buttonConfig =
				(struct tButtonConfig*) lv_obj_get_user_data(obj);
		create_pouring_popup(lv_scr_act(), buttonConfig);
	}
}

static void password_popup_cb(struct _lv_obj_t *obj, lv_event_t event) {
	if (event == LV_EVENT_DELETE) {
		/* Delete the parent modal background */
		lv_obj_del_async(lv_obj_get_parent(obj));
	} else if (event == LV_EVENT_VALUE_CHANGED) {
		/* A button was clicked */
		const char *text = lv_msgbox_get_active_btn_text(obj);
		const char ch = text[0];
		if (ch == CANCEL[0]) {
			lv_msgbox_start_auto_close(obj, 0);
			return;
		}

		password[cur_pwd_chr] = ch;
		cur_pwd_chr++;

		int password_compare = AppConfig_compare_password(password);

		bool many_letters = cur_pwd_chr >= (sizeof(password) - 1);

		if (many_letters || (password_compare == 0)) {
			lv_msgbox_start_auto_close(obj, 0);
			if (password_compare == 0) {
				create_settings_menu(lv_scr_act(), AppConfig_get());
			}
		}
	}
}

void create_password_popup(lv_obj_t *parent) {
	memset(password, 0, sizeof(password));
	cur_pwd_chr = 0;

	lv_style_init(&style_modal);
	lv_style_set_bg_color(&style_modal, LV_STATE_DEFAULT, LV_COLOR_BLACK);

	lv_obj_t *black_pie = lv_obj_create(parent, NULL);
	lv_obj_reset_style_list(black_pie, LV_OBJ_PART_MAIN);
	lv_obj_add_style(black_pie, LV_OBJ_PART_MAIN, &style_modal);
	lv_obj_set_pos(black_pie, 0, 0);
	lv_obj_set_size(black_pie, LV_HOR_RES, LV_VER_RES);

	lv_obj_t *popup = lv_msgbox_create(black_pie, NULL);

	lv_msgbox_set_text(popup, "Enter password:");
	lv_msgbox_add_btns(popup, password_bts);
	lv_btnmatrix_set_btn_width(lv_msgbox_get_btnmatrix(popup), 10, 2);

	lv_obj_align(popup, black_pie, LV_ALIGN_CENTER, 0, 0);

	lv_msgbox_start_auto_close(popup, 65535U);
	lv_obj_set_event_cb(popup, password_popup_cb);
}

static void settings_btn_cb(struct _lv_obj_t *obj, lv_event_t event) {
	if (event == LV_EVENT_CLICKED) {
		if (AppConfig_compare_password("") == 0) {
			create_settings_menu(lv_scr_act(), AppConfig_get());
		} else {
			create_password_popup(lv_scr_act());
		}
	}

}

static void resetCurrVal_btn_cb(struct _lv_obj_t *obj, lv_event_t event){
	if (event == LV_EVENT_CLICKED) {
		volume_total = 0;
		lv_label_set_text_fmt(volume_label, volume_label_fmt, volume_total);
	}
}



void create_main_interface(struct tAppConfig *config) {
	lv_obj_t *window = lv_win_create(lv_scr_act(), NULL);
	lv_win_set_title(window, "Got milk?");
	lv_win_set_scrollbar_mode(window, LV_SCROLLBAR_MODE_OFF);
	lv_win_set_layout(window, LV_LAYOUT_OFF);
	lv_win_set_content_size(window, LV_HOR_RES,
	LV_VER_RES - lv_win_get_header_height(window));
	{
		lv_obj_t *settings_btn = lv_win_add_btn(window, LV_SYMBOL_SETTINGS);
		lv_obj_set_event_cb(settings_btn, settings_btn_cb);

		lv_obj_t *resetCurrVal_btn = lv_win_add_btn(window, LV_SYMBOL_REFRESH);
		lv_obj_set_event_cb(resetCurrVal_btn, resetCurrVal_btn_cb);

		lv_obj_t *button_holder = lv_cont_create(window, NULL);

		for (int i = 0; i < APP_BUTTONS_COUNT; i++) {

			lv_obj_t *btn = lv_btn_create(button_holder, NULL);
			lv_obj_set_height(btn, 200);
			lv_obj_set_user_data(btn, &(config->buttons[i]));
			lv_obj_set_event_cb(btn, create_pouring_popup_cb);

			lv_obj_t *label = lv_label_create(btn, NULL);
			lv_label_set_text_fmt(label, "%d ml", config->buttons[i].volume_ml);

			main_button_labels[i] = label;
		}

		lv_cont_set_layout(button_holder, LV_LAYOUT_PRETTY_MID);
		lv_cont_set_fit2(button_holder, LV_FIT_PARENT, LV_FIT_TIGHT);
		lv_obj_align(button_holder, window, LV_ALIGN_CENTER, 0, 0);

		volume_label = lv_label_create(window, NULL);
		lv_label_set_text_fmt(volume_label, volume_label_fmt, volume_total);
		lv_obj_align(volume_label, window, LV_ALIGN_IN_BOTTOM_LEFT, 19, -19);

	}
	main_window = window;
}

