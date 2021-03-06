#ifndef INC_APP_CONFIG_H_
#define INC_APP_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

#define APP_BUTTONS_COUNT 3
#define APP_BUTTONS_LABEL_LENGTH 64

struct tButtonConfig{
	uint32_t volume_ml;
};

struct tAppConfig {
	struct tButtonConfig buttons[APP_BUTTONS_COUNT];
	uint32_t ml_per_min;
	char password[128];
};

struct tAppConfig* AppConfig_get();
bool AppConfig_save();

struct tButtonConfig * AppConfig_button_for_text(const char *text);
int AppConfig_compare_password(char * text);

#endif /* INC_APP_CONFIG_H_ */
