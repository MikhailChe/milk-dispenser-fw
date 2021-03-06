#include "interface.h"
#include "lvgl.h"

extern lv_obj_t *main_button_labels[APP_BUTTONS_COUNT];

static lv_obj_t *settings_window;

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

static void lv_spinbox_increment_event_cb(lv_obj_t *btn, lv_event_t e) {
	lv_obj_t *parent = lv_obj_get_parent(btn);
	lv_obj_t *btn1 = lv_obj_get_child_back(parent, NULL);
	lv_obj_t *spinbox = lv_obj_get_child_back(parent, btn1);

	if (e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_increment(spinbox);
	}
}

static void lv_spinbox_decrement_event_cb(lv_obj_t *btn, lv_event_t e) {
	lv_obj_t *parent = lv_obj_get_parent(btn);
	lv_obj_t *btn1 = lv_obj_get_child_back(parent, NULL);
	lv_obj_t *spinbox = lv_obj_get_child_back(parent, btn1);

	if (e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
		lv_spinbox_decrement(spinbox);
	}
}

static void create_spinbox(lv_obj_t *parent, struct tButtonConfig *config) {
	lv_obj_t *container = lv_cont_create(parent, NULL);
	lv_obj_set_width(container, 250);
	lv_cont_set_layout(container, LV_LAYOUT_CENTER);
	lv_cont_set_fit(container, LV_FIT_TIGHT);

	lv_obj_t *btn = lv_btn_create(container, NULL);

	lv_obj_t *spinbox = lv_spinbox_create(container, NULL);
	add_spinbox(spinbox);

	lv_spinbox_set_digit_format(spinbox, 4, 0);
	lv_spinbox_set_range(spinbox, 10, 1000);
	lv_spinbox_set_step(spinbox, 1);
	lv_spinbox_set_value(spinbox, config->volume_ml);

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

static void save_button_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_SHORT_CLICKED) {
		struct tAppConfig *config = AppConfig_get();
		for (int i = 0; i < APP_BUTTONS_COUNT; i++) {
			if (spinboxes[i]) {
				uint32_t volume = lv_spinbox_get_value(spinboxes[i]);
				config->buttons[i].volume_ml = volume;
				lv_label_set_text_fmt(main_button_labels[i], "%d ml", volume);
			}
		}
		AppConfig_save();
	}
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
		create_spinbox(window, &(config->buttons[i]));
		;;
	}

	settings_window = window;
}
