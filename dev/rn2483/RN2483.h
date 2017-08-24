#ifndef RN2483_H_
#define RN2483_H_

#include "contiki.h"
int lora_initialize();
void RN2483_version();
void RN2483_sleep(int duration);
void RN2483_reset();
void RN2483_Freset();
void RN2483_StoreData(char* address,char* data);
int get_system_settings(char *setting);
void RN2483_GetData(char* address);

void mac_reset(int band);
void mac_send(char *type,int portnumber,char *data);
void mac_join(char *mode);// otaa or abp // called by join OTAA or ABP
void mac_save();
void mac_force_enable();
int set_mac_settings(const char *first, ...);
int set_mac_channel_settings(const char *first, ...);// NOT OK
int get_mac_settings(const char *first, ...);
int get_mac_channel_settings(const char *first, ...);

void radio_settings();
void ptp_send(char *message);
void ptp_receive(int window);
void cw_state(char *state);
int set_radio_settings(const char *first, ...);
int get_radio_settings(const char *first, ...);

void join_abp(char *devaddr, char *nwkskey,char *appskey);
void join_otaa(char *deveui, char *appeui,char *appkey);
void Ldelay();
PROCESS_NAME(lora_process);
void process_polll();
#endif 
