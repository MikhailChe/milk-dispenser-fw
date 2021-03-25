#include "interface.h"
#include "lvgl.h"
#include "pump.h"

//static lv_obj_t *calibration_window;
static lv_obj_t *timeChngr_slider;
static lv_obj_t *timeChngrSlider_label;

static lv_obj_t *resValChngr_slider;
static lv_obj_t *resValChngrSlider_label;

static lv_obj_t *resValMlParMinute_label;

static void save_button_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_SHORT_CLICKED) {

		struct tAppConfig *config = AppConfig_get();
		uint32_t timeVal = lv_slider_get_value(timeChngr_slider);
		uint32_t mlVal = lv_slider_get_value(resValChngr_slider);
		uint64_t ml_per_min = (mlVal * 60UL) / timeVal;
		config->ml_per_min = ml_per_min;
		AppConfig_save();
	}
}

static void time_slider_cb(lv_obj_t *obj, lv_event_t e) {
	if (e == LV_EVENT_VALUE_CHANGED) {
		uint16_t timeVal = lv_slider_get_value(timeChngr_slider);
		uint16_t mlVal = lv_slider_get_value(resValChngr_slider);
		lv_label_set_text_fmt(timeChngrSlider_label,
				"Pump enable time: %d sec.",timeVal);
		lv_label_set_text_fmt(resValMlParMinute_label,
				"Calibration value:\n%d ml par minute", mlVal*60/timeVal);
	}
}

static void resValChngr_slider_cb(lv_obj_t *obj, lv_event_t e) {
	if (e == LV_EVENT_VALUE_CHANGED) {
		uint16_t timeVal = lv_slider_get_value(timeChngr_slider);
		uint16_t mlVal = lv_slider_get_value(resValChngr_slider);
		lv_label_set_text_fmt(resValChngrSlider_label, "Result value: %d ml.",
				mlVal);
		lv_label_set_text_fmt(resValMlParMinute_label,
				"Calibration value:\n%d ml par minute", (mlVal*60)/timeVal);
	}
}

static void en_pump_button_cb(lv_obj_t *btn, lv_event_t e) {
	if (e == LV_EVENT_SHORT_CLICKED) {
		pump_schedule_turn_off(lv_slider_get_value(timeChngr_slider) * 1000);
		pump_on();
	}
}

lv_obj_t* create_calibration_window(lv_obj_t *parent, struct tAppConfig *config) {
	lv_obj_t *calibration_window = lv_win_create(parent, NULL);
	lv_win_set_title(calibration_window, "Calibration");
	lv_win_set_scrollbar_mode(calibration_window, LV_SCROLLBAR_MODE_OFF);
	uint16_t content_height = lv_obj_get_height(calibration_window)
			- lv_win_get_header_height(calibration_window);
//	lv_cont_set_layout(calibration_window, LV_LAYOUT_GRID);
	{
		lv_obj_t *save_btn = lv_win_add_btn(calibration_window, LV_SYMBOL_SAVE);
		lv_obj_set_event_cb(save_btn, save_button_cb);

		lv_obj_t *close_btn = lv_win_add_btn(calibration_window,
		LV_SYMBOL_CLOSE);
		lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);

		lv_obj_t *time_changer_cont = lv_cont_create(calibration_window, NULL);
		{
			lv_obj_set_width_fit(time_changer_cont,
					lv_obj_get_width(calibration_window));
			lv_obj_set_height_fit(time_changer_cont, content_height / 2);

			lv_obj_t *time_changer_slider_cont = lv_cont_create(
					time_changer_cont, NULL);
			{
				lv_obj_set_height_fit(time_changer_slider_cont,
						lv_obj_get_height(time_changer_cont));
				lv_obj_set_width_fit(time_changer_slider_cont,
						(lv_obj_get_width(time_changer_cont) * 2) / 3);
				lv_obj_align_y(time_changer_slider_cont, NULL, LV_ALIGN_CENTER,
						0);
				lv_obj_align_x(time_changer_slider_cont, NULL,
						LV_ALIGN_IN_LEFT_MID,
						lv_obj_get_style_pad_left(time_changer_slider_cont, 0));
				lv_cont_set_layout(time_changer_slider_cont,
						LV_LAYOUT_COLUMN_MID);

				{
					timeChngr_slider = lv_slider_create(
							time_changer_slider_cont, NULL);
					lv_slider_set_range(timeChngr_slider, 1, 60);
					lv_slider_set_value(timeChngr_slider, 5, 0);
					lv_obj_set_width_fit(timeChngr_slider,
							lv_obj_get_width(time_changer_slider_cont) * 2 / 3);
					lv_obj_set_event_cb(timeChngr_slider, time_slider_cb);

					timeChngrSlider_label = lv_label_create(
							time_changer_slider_cont, NULL);
					lv_label_set_text_fmt(timeChngrSlider_label,
							"Pump enable time: %d sec.",
							lv_slider_get_value(timeChngr_slider));
				}
			}
			lv_obj_t *enable_pump_btn = lv_btn_create(time_changer_cont, NULL);
			lv_obj_set_width_fit(enable_pump_btn,
					lv_obj_get_width(time_changer_cont) / 3);
			lv_obj_align_x(enable_pump_btn, NULL, LV_ALIGN_IN_RIGHT_MID,
					-lv_obj_get_style_pad_right(enable_pump_btn, 0));
			lv_obj_align_y(enable_pump_btn, NULL, LV_ALIGN_CENTER, 0);
			lv_obj_set_event_cb(enable_pump_btn, en_pump_button_cb);
			lv_cont_set_layout(enable_pump_btn, LV_LAYOUT_CENTER);
			{
				lv_obj_t *enable_pump_btn_label = lv_label_create(
						enable_pump_btn, NULL);
				lv_label_set_text(enable_pump_btn_label, "Start!");
			}

		}

		lv_obj_t *resValChngr_cont = lv_cont_create(calibration_window,
		NULL);
		{
			lv_obj_set_width_fit(resValChngr_cont,
					lv_obj_get_width(calibration_window));
			lv_obj_set_height_fit(resValChngr_cont, content_height / 2);
			lv_obj_align(resValChngr_cont, NULL, LV_ALIGN_IN_BOTTOM_MID, 0,
					-lv_win_get_header_height(calibration_window) / 2);
			lv_obj_t *resValChngrSlider_cont = lv_cont_create(resValChngr_cont,
			NULL);
			{
				lv_obj_set_height_fit(resValChngrSlider_cont,
						lv_obj_get_height(resValChngr_cont));
				lv_obj_set_width_fit(resValChngrSlider_cont,
						(lv_obj_get_width(resValChngr_cont) * 2) / 3);
				lv_obj_align_y(resValChngrSlider_cont, NULL, LV_ALIGN_CENTER,
						0);
				lv_obj_align_x(resValChngrSlider_cont, NULL,
						LV_ALIGN_IN_LEFT_MID,
						lv_obj_get_style_pad_left(resValChngrSlider_cont, 0));
				lv_cont_set_layout(resValChngrSlider_cont,
						LV_LAYOUT_COLUMN_MID);

				{
					resValChngr_slider = lv_slider_create(
							resValChngrSlider_cont, NULL);
					lv_slider_set_range(resValChngr_slider, 1, 1000);
					lv_slider_set_value(resValChngr_slider, 1, 0);
					lv_obj_set_width_fit(resValChngr_slider,
							lv_obj_get_width(resValChngrSlider_cont) * 2 / 3);
					lv_obj_set_event_cb(resValChngr_slider,
							resValChngr_slider_cb);

					resValChngrSlider_label = lv_label_create(
							resValChngrSlider_cont, NULL);
					lv_label_set_text_fmt(resValChngrSlider_label,
							"Result value: %d ml.",
							lv_slider_get_value(resValChngr_slider));
				}
			}
			resValMlParMinute_label = lv_label_create(resValChngr_cont, NULL);
			lv_label_set_text_fmt(resValMlParMinute_label,
					"Calibration value:\n%d ml par minute", config->ml_per_min);
			lv_obj_set_width_fit(resValMlParMinute_label, lv_obj_get_width(resValChngr_cont) / 3);
			lv_label_set_align(resValMlParMinute_label, LV_LABEL_ALIGN_CENTER);
			lv_obj_align_x(resValMlParMinute_label, NULL, LV_ALIGN_IN_RIGHT_MID, -40);
			lv_obj_align_y(resValMlParMinute_label, NULL, LV_ALIGN_CENTER, 0);

		}
	}
	return calibration_window;
}
