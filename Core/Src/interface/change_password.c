#include "interface.h"
#include "app_config.h"
#include "lvgl.h"

static lv_obj_t *window;
static lv_obj_t *ta;
static lv_obj_t *kb;

static const char *accpeted_chars = "0123456789";

static const char *default_kb_map_num[] = { //
		"1", "2", "3", LV_SYMBOL_CLOSE, "\n",
		//
				"4", "5", "6", LV_SYMBOL_OK, "\n",
				//
				"7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
				//
				"0", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, "" };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_num_map[] = {
//
		1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
		//
		1, 1, 1, LV_BTNMATRIX_CTRL_HIDDEN | 2,
		//
		1, 1, 1, 2,
		//
		3, 1, 1 };

static void kb_event_cb(lv_obj_t *keyboard, lv_event_t e) {
	lv_keyboard_def_event_cb(kb, e);
	if (e == LV_EVENT_CANCEL) {
		lv_keyboard_set_textarea(kb, NULL);
		lv_obj_del(kb);
		kb = NULL;
	}
}

static void kb_create(void) {
	kb = lv_keyboard_create(lv_scr_act(), NULL);
	lv_keyboard_set_cursor_manage(kb, true);
	lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_NUM, default_kb_map_num);
	lv_keyboard_set_ctrl_map(kb, LV_KEYBOARD_MODE_NUM, default_kb_ctrl_num_map);
	lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUM);
	lv_obj_set_event_cb(kb, kb_event_cb);
	lv_keyboard_set_textarea(kb, ta);
}

static void ta_event_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_CLICKED && kb == NULL) {
		kb_create();
	}
}

static void save_button_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_SHORT_CLICKED) {
		struct tAppConfig *config = AppConfig_get();
		const char *new_password = lv_textarea_get_text(ta);
		char *password_in_config = config->password;
		strcpy(password_in_config, new_password);
		AppConfig_save();
	}
}

static void lv_close_window_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_RELEASED) {
		if (kb != NULL) {
			lv_event_send(kb, LV_EVENT_CANCEL, NULL);
		}
		lv_obj_del(window);
	}
}

lv_obj_t* create_change_password_window(lv_obj_t *parent,
		struct tAppConfig *config) {
	window = lv_win_create(parent, NULL);
	lv_win_set_title(window, "Change password");
	lv_win_set_layout(window, LV_LAYOUT_PRETTY_TOP);
	{
		lv_obj_t *save_btn = lv_win_add_btn(window, LV_SYMBOL_SAVE);
		lv_obj_set_event_cb(save_btn, save_button_cb);

		lv_obj_t *close_btn = lv_win_add_btn(window, LV_SYMBOL_CLOSE);
		lv_obj_set_event_cb(close_btn, lv_close_window_cb);

		ta = lv_textarea_create(window, NULL);
//		lv_textarea_set_pwd_mode(ta, true);
		lv_textarea_set_one_line(ta, true);
		lv_textarea_set_accepted_chars(ta, accpeted_chars);
		lv_textarea_set_text(ta, config->password);
		lv_obj_set_event_cb(ta, ta_event_cb);

	}
	return window;
}
