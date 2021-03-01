#ifndef INC_APP_CONFIG_H_
#define INC_APP_CONFIG_H_

#include <stdint.h>

struct tButtonConfig{
	char name[128];
	uint32_t time;
};

struct tAppConfig {
	struct tButtonConfig buttons[3];
	char password[128];
};

struct tAppConfig* AppConfig_get();

#endif /* INC_APP_CONFIG_H_ */
