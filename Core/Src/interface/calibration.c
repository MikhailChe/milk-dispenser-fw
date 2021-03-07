#include "interface.h"

static lv_obj_t *window;

static lv_obj_t *spinbox;

static void lv_spinbox_increment_event_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_increment(spinbox);
	}
}

static void lv_spinbox_decrement_event_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_decrement(spinbox);
	}
}

static lv_obj_t* create_spinbox(lv_obj_t *parent, struct tAppConfig *config) {
	spinbox = lv_spinbox_create(parent, NULL);

	lv_spinbox_set_digit_format(spinbox, 5, 0);
	lv_spinbox_set_range(spinbox, 500, 90000);
	lv_spinbox_set_step(spinbox, 5);
	lv_spinbox_set_value(spinbox, config->ml_per_min);

	lv_textarea_set_cursor_hidden(spinbox, true);

	lv_obj_set_width(spinbox, 150);
	lv_obj_align(spinbox, NULL, LV_ALIGN_CENTER, 0, 0);


	lv_coord_t h = lv_obj_get_height(spinbox);

	lv_obj_t *btn = lv_btn_create(parent, NULL);
	lv_obj_set_size(btn, h, h);
	lv_obj_align(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
	lv_theme_apply(btn, LV_THEME_SPINBOX_BTN);
	lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
	LV_SYMBOL_PLUS);
	lv_obj_set_event_cb(btn, lv_spinbox_increment_event_cb);

	btn = lv_btn_create(parent, btn);
	lv_obj_align(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
	lv_obj_set_event_cb(btn, lv_spinbox_decrement_event_cb);
	lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
	LV_SYMBOL_MINUS);

	return spinbox;
}

static void save_button_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_SHORT_CLICKED) {
		struct tAppConfig *config = AppConfig_get();
		config->ml_per_min = lv_spinbox_get_value(spinbox);
		AppConfig_save();
	}
}

lv_obj_t* create_calibration_window(lv_obj_t *parent, struct tAppConfig *config) {
	window = lv_win_create(parent, NULL);
	lv_win_set_title(window, "Calibration");
	{
		lv_obj_t *save_btn = lv_win_add_btn(window, LV_SYMBOL_SAVE);
		lv_obj_set_event_cb(save_btn, save_button_cb);

		lv_obj_t *close_btn = lv_win_add_btn(window, LV_SYMBOL_CLOSE);
		lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);

		create_spinbox(window, config);
	}
	return window;
}
