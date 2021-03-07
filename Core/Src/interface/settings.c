#include "interface.h"
#include "lvgl.h"

static lv_obj_t *window;

static const char *names[] = { "Change volumes", "Calibration", "Change password" };

static lv_obj_t* (*callbacks[])(lv_obj_t*, struct tAppConfig*) = {
	create_volumes_interface,
	create_calibration_window,
	create_change_password_window,
};

static void links_cb(lv_obj_t *obj, lv_event_t e) {
	if (e == LV_EVENT_SHORT_CLICKED) {
		lv_obj_t* (*callback)(lv_obj_t*, struct tAppConfig*) = lv_obj_get_user_data(obj);
		callback(lv_scr_act(), AppConfig_get());
	}
}

lv_obj_t* create_settings_menu(lv_obj_t *parent, struct tAppConfig *config) {
	window = lv_win_create(parent, NULL);
	lv_win_set_layout(window, LV_LAYOUT_COLUMN_LEFT);
	{
		lv_obj_t *close_btn = lv_win_add_btn(window, LV_SYMBOL_CLOSE);
		lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);

		for (int i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
			lv_obj_t *link = lv_btn_create(window, NULL);
			lv_obj_set_user_data(link, callbacks[i]);
			lv_obj_set_event_cb(link, links_cb);
			lv_btn_set_fit(link, LV_FIT_TIGHT);
			{
				lv_obj_t *label = lv_label_create(link, NULL);
				lv_label_set_text(label, names[i]);
			}
		}
	}
	return window;
}
