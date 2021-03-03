#include <stdbool.h>
#include <string.h>

#include "app_config.h"
#include "ff.h"

#define APP_CFG_PATH "app.cfg"

union uConfig {
	struct tAppConfig config;
	uint8_t data[sizeof(struct tAppConfig)];
};

struct tConfig {
	union uConfig u;
	uint32_t crc;
};

struct tConfig config;
bool loaded = false;

static uint32_t _crc(union uConfig *u) {
	uint32_t sum;
	for (int i = 0; i < sizeof(u->data); i++) {
		sum += u->data[i];
	}
	return ~sum;
}

static bool _load_file(FIL *file){
	struct tConfig temp;
	int configSz = sizeof(struct tConfig);
	UINT br;
	if(FR_OK!=f_read(file, &temp, configSz, &br)){
		return false;
	}

	if (br != configSz){
		return false;
	}

	uint32_t crc = _crc(&(temp.u));

	bool res =  (temp.crc == crc);
	if (res){
		config = temp;
	}
	return res;
}

static bool _load() {
	if(loaded){
		return true;
	}
	FIL file;
	if(FR_OK!= f_open(&file, APP_CFG_PATH, FA_READ)){
		f_close(&file);
		return false;
	}

	bool res = _load_file(&file);
	f_close(&file);

	if(res){
		loaded = true;
	}

	return res;
}



static void _load_defaults() {

	struct tConfig temp = { .u = { .config = { .password = "12345678",
			.buttons = { { .name = "25", .time = 250, }, { .name = "50", .time =
					500, }, { .name = "200", .time = 2000, } } } } };
	temp.crc = _crc(&(temp.u));

	config = temp;

	loaded = true;
}

static bool _save_file(FIL *file){
	struct tConfig temp = config;
	int configSz = sizeof(struct tConfig);
	UINT bw;
	if(FR_OK!=f_write(file, &temp, configSz, &bw)){
		return false;
	}

	if(bw!=configSz){
		return false;
	}

	return true;
}

static bool _save() {
	FIL file;
	if(FR_OK!=f_open(&file, APP_CFG_PATH, FA_WRITE | FA_CREATE_ALWAYS)){
		f_close(&file);
		return false;
	}

	bool res = _save_file(&file);
	f_close(&file);
	return res;
}

static struct tAppConfig* _get() {
	return &(config.u.config);
}

struct tAppConfig* AppConfig_get() {
	if (_load()) {
		return _get();
	}

	_load_defaults();
	if(!_save()){
		Error_Handler();
	}
	return _get();
}

uint32_t AppConfig_delay_for_button(const char * text){
	struct tAppConfig* config = AppConfig_get();

	for(int i=0; i<APP_BUTTONS_COUNT; i++){
		if(strcmp(text, config->buttons[i].name) == 0){
			return config->buttons[i].time;
		}
	}

	return 5000;
}

int AppConfig_compare_password(char * text){
	char * password = AppConfig_get()->password;
	return strcmp(text, password);
}
