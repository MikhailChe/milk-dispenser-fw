#ifndef INC_APP_CONFIG_H_
#define INC_APP_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

#define APP_BUTTONS_COUNT 3
#define APP_BUTTONS_LABEL_LENGTH 64

struct tButtonConfig{
	char name[APP_BUTTONS_LABEL_LENGTH];
	uint32_t time;
};

struct tAppConfig {
	struct tButtonConfig buttons[APP_BUTTONS_COUNT];
	char password[128];
};

struct tAppConfig* AppConfig_get();
bool AppConfig_save();

uint32_t AppConfig_delay_for_button(const char * text);
int AppConfig_compare_password(char * text);

#endif /* INC_APP_CONFIG_H_ */
