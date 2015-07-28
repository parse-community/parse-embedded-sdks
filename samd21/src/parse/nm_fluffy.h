/*
 * nm_fluffy.h
 *
 * Created: 2014-11-05 오후 5:25:24
 *  Author: jini
 */ 


#ifndef NM_FLUFFY_H_
#define NM_FLUFFY_H_


void fluffy_wifi_noti_cb(uint8_t u8MsgType, void * pvMsg);

uint8_t fluffy_check_home_ap(char* ssid, char* pw, uint8_t secType);

void fluffy_set_home_ap_info(char* ssid, char* pw, uint8_t secType, uint8_t fileWrite);
int fluffy_remove_home_ap_info(void);

uint8_t fluffy_load_fluffy_info(void);
void fluffy_set_fluffy_info(uint8_t fileWrite);
int fluffy_remove_fluffy_info(void);

int fluffy_device_config_add_key( const char* key, const char* label, const char* value );
const char * fluffy_device_config_get_value( const char *key );
const char * fluffy_device_config_get_value_by_index( int index );
void fluffy_device_config_set_value( int index, char* value );
const char * fluffy_device_config_get_key( int index );
const char * fluffy_device_config_get_label( int index );
int fluffy_device_config_get_count( void );
	
void fluffy_wifi_ap_mode(int8_t isAPEnable, uint8_t* macAddr);

uint8_t fluffy_get_ap_mode_completed(void);

void fluffy_wifi_stat_mode(int8_t isStaEnable);

uint8_t fluffy_get_wifi_connected(void);

#endif /* NM_FLUFFY_H_ */