#include "interface.h"
#include "pump.h"

static lv_obj_t *window;

//static lv_obj_t *spinbox;
static lv_obj_t *time_slider;
static lv_obj_t *timeValue_label;

static lv_obj_t *resVal_slider;
static lv_obj_t *resultInMlValue_label;

static lv_obj_t *resInMlParMinute_label;

//static void lv_spinbox_increment_event_cb(lv_obj_t *btn, lv_event_t e) {
//	if (e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
//		lv_spinbox_increment(spinbox);
//	}
//}
//
//static void lv_spinbox_decrement_event_cb(lv_obj_t *btn, lv_event_t e) {
//	if (e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
//		lv_spinbox_decrement(spinbox);
//	}
//}

//static lv_obj_t* create_spinbox(lv_obj_t *parent, struct tAppConfig *config) {
//	spinbox = lv_spinbox_create(parent, NULL);
//
//	lv_spinbox_set_digit_format(spinbox, 5, 0);
//	lv_spinbox_set_range(spinbox, 500, 90000);
//	lv_spinbox_set_step(spinbox, 5);
//	lv_spinbox_set_value(spinbox, config->ml_per_min);
//
//	lv_textarea_set_cursor_hidden(spinbox, true);
//
//	lv_obj_set_width(spinbox, 150);
//	lv_obj_align(spinbox, NULL, LV_ALIGN_CENTER, 0, 0);
//
//	lv_coord_t h = lv_obj_get_height(spinbox);
//
//	lv_obj_t *btn = lv_btn_create(parent, NULL);
//	lv_obj_set_size(btn, h, h);
//	lv_obj_align(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
//	lv_theme_apply(btn, LV_THEME_SPINBOX_BTN);
//	lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
//	LV_SYMBOL_PLUS);
//	lv_obj_set_event_cb(btn, lv_spinbox_increment_event_cb);
//
//	btn = lv_btn_create(parent, btn);
//	lv_obj_align(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
//	lv_obj_set_event_cb(btn, lv_spinbox_decrement_event_cb);
//	lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,
//	LV_SYMBOL_MINUS);
//
//	return spinbox;
//}

//static void save_button_cb(lv_obj_t *btn, lv_event_t e) {
//	if (e == LV_EVENT_SHORT_CLICKED) {
//		struct tAppConfig *config = AppConfig_get();
//		uint64_t ml_per_min = lv_slider_get_value(resVal_slider) * 60UL
//				/ (lv_slider_get_value(time_slider));
//		config->ml_per_min = ml_per_min;
//		AppConfig_save();
//	}
//}
//
//static void time_slider_cb(lv_obj_t *obj, lv_event_t e) {
//	if (e == LV_EVENT_VALUE_CHANGED) {
//		lv_label_set_text_fmt(timeValue_label, "%d sec.",
//				lv_slider_get_value(obj));
//	}
//}
//static void resVal_slider_cb(lv_obj_t *obj, lv_event_t e) {
//	if (e == LV_EVENT_VALUE_CHANGED) {
//		lv_label_set_text_fmt(resultInMlValue_label, "%d ml.",
//				lv_slider_get_value(obj));
//		lv_label_set_text_fmt(resInMlParMinute_label, "%d ml par minute",
//				lv_slider_get_value(resVal_slider) * 60UL
//						/ (lv_slider_get_value(time_slider)));
//	}
//}
//static void startCal_btn_cb(lv_obj_t *obj, lv_event_t e) {
//	if (e == LV_EVENT_PRESSED) {
//		pump_schedule_turn_off(lv_slider_get_value(time_slider) * 1000);
//		pump_on();
//	}
//}

lv_obj_t* create_calibration_window(lv_obj_t *parent, struct tAppConfig *config) {
	window = lv_win_create(parent, NULL);
	lv_win_set_title(window, "Calibration");
	{
		lv_obj_t *save_btn = lv_win_add_btn(window, LV_SYMBOL_SAVE);
//		lv_obj_set_event_cb(save_btn, save_button_cb);

		lv_obj_t *close_btn = lv_win_add_btn(window, LV_SYMBOL_CLOSE);
		lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);

		lv_cont_set_layout(window, LV_LAYOUT_PRETTY_TOP);

//		create_spinbox(window, config);

		lv_obj_t *timeChanger_cont = lv_obj_create(window, NULL);
		{
			lv_cont_set_layout(timeChanger_cont, LV_LAYOUT_PRETTY_MID);

//			lv_cont_set_fit2(timeChanger_cont, LV_FIT_MAX, LV_FIT_NONE);
			lv_obj_set_height_fit(timeChanger_cont,
					(lv_obj_get_height(window)
							- lv_win_get_header_height(window)));
			lv_obj_set_width_fit(timeChanger_cont, lv_obj_get_width(window));

			lv_obj_t *timeSlider_cont = lv_obj_create(timeChanger_cont, NULL);
			{
				lv_cont_set_layout(timeSlider_cont, LV_LAYOUT_COLUMN_MID);
				time_slider = lv_slider_create(timeSlider_cont, NULL);
				lv_slider_set_range(time_slider, 1, 120);
				lv_slider_set_value(time_slider, 1, 0);
//				lv_obj_set_event_cb(time_slider, time_slider_cb);
				timeValue_label = lv_label_create(timeSlider_cont, NULL);
				lv_label_set_text_fmt(timeValue_label, "%d sec.", 1);
			}
			lv_obj_t *startCal_btn = lv_btn_create(timeChanger_cont, NULL);
			{
//				lv_obj_set_event_cb(startCal_btn, startCal_btn_cb);
				lv_obj_t *startCalBtn_label = lv_label_create(startCal_btn,
				NULL);
				lv_label_set_text(startCalBtn_label, "Enable pump");
			}
		}

//		lv_obj_t *resultValue_cont = lv_obj_create(window, NULL);
//		{
//			lv_cont_set_fit2(resultValue_cont, LV_FIT_PARENT, LV_FIT_NONE);
//			lv_obj_set_height_fit(resultValue_cont,
//					(lv_obj_get_height(window)
//							- lv_win_get_header_height(window)) / 2);
//			lv_cont_set_layout(resultValue_cont, LV_LAYOUT_ROW_MID);
//			lv_obj_t *resultSlider_cont = lv_obj_create(resultValue_cont, NULL);
//			{
//				lv_cont_set_layout(resultSlider_cont, LV_LAYOUT_COLUMN_MID);
//				resVal_slider = lv_slider_create(resultSlider_cont,
//				NULL);
//				lv_slider_set_range(resVal_slider, 1, 3000);
//				lv_slider_set_value(resVal_slider, 1, 0);
//				lv_obj_set_event_cb(resVal_slider, resVal_slider_cb);
//				resultInMlValue_label = lv_label_create(resultSlider_cont,
//				NULL);
//				lv_label_set_text_fmt(resultInMlValue_label, "%d ml.", 1);
//			}
//			resInMlParMinute_label = lv_label_create(resultValue_cont, NULL);
//			lv_label_set_text_fmt(resInMlParMinute_label, "%d ml par minute",
//					config->ml_per_min);
//		}

	}
	return window;
}
