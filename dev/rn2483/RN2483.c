#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "core/dev/radio.h"
#include "dev/leds.h"
#include "sys/clock.h"
#include "sys/ctimer.h"
#include "core/sys/rtimer.h"

#include "net/packetbuf.h"
#include "net/rime/rimestats.h"
#include "net/netstack.h"

#include "RN2483.h"
#include "dev/rn2483/rn2483-uart.h"
static char rxbuf_data[64]={0};
#define INTER_PACKET_INTERVAL  RTIMER_ARCH_SECOND/ 10 //=100ms
#define CLEAR_RXBUF()		(rxbuf_data[0] = 0)
#define IS_RXBUF_EMPTY()	(rxbuf_data[0] == 0) 
static struct ctimer timer;
static int i=0;
static int u=10;
static const void *packet_payload;
static unsigned short packet_payload_len=0;

char *system_enum[3]={"ver","vdd","hweui"};
/*---------------------------------------------------------------------------*/
void blink_led();
static void BRScallback(void *ptr);// get Brief Radio Settings callback
static void RScallback(char ptr[20][24]);// Radio Settings callback
void vset_settings(const char *setting, const char *first, va_list argp,...);
void vget_settings(const char *setting, const char *first, va_list argp,...);
static int lora_init(void);
static int lora_prepare(const void *payload, unsigned short payload_len);
static int lora_transmit(unsigned short payload_len);
static int lora_radio_send(const void *payload, unsigned short payload_len);
static int lora_radio_read(void *buf, unsigned short bufsize);
static int lora_channel_clear(void);
static int lora_receiving_packet(void);
static int lora_pending_packet(void);
static int lora_off(void);
static int lora_on(void);

/*---------------------------------------------------------------------------*/
PROCESS(lora_process,"LoRa recption handling process");
/*---------------------------------------------------------------------------*/
const struct radio_driver lora_radio_driver =
{
lora_init,
lora_prepare,
lora_transmit,
lora_radio_send,
lora_radio_read,
lora_channel_clear,
lora_receiving_packet,
lora_pending_packet,
lora_off,
lora_on,
};
/*---------------------------------------------------------------------------*/
static int lora_init(void)
{
printf("initializing..\n");
process_start(&lora_process,NULL);
printf("done\n");
return 1;
}
/*---------------------------------------------------------------------------*/
static int lora_prepare(const void *payload, unsigned short payload_len)
{
printf("Prepare in: %u bytes\n",payload_len);
RIMESTATS_ADD(lltx);
if(payload_len>64){return 0;}
packet_payload=payload;
packet_payload_len=payload_len;
printf("Prepared succesfully\n");
return 1;
}
/*---------------------------------------------------------------------------*/
static int lora_transmit(unsigned short payload_len)
{
        char* c;
	c = malloc(payload_len+1);
	strcpy(c,"radio tx ");
	printf("sending message: ");
        strcat(c,(char *)packet_payload);
        strcat(c,"\r\n");
        lora_send(c);// get the used modulation technique
	blink_led();
}
/*---------------------------------------------------------------------------*/
static int lora_radio_send(const void *payload, unsigned short payload_len)//point-to-point link: hex value; 48656c6c6f = hello /
{
	lora_prepare(payload, payload_len);
	lora_transmit(payload_len);
return 1;
}
/*---------------------------------------------------------------------------*/
static int lora_radio_read(void *buf, unsigned short bufsize)
{
if(IS_RXBUF_EMPTY()){
printf("buffer empty");
return 0;
}
if(bufsize<64){
printf("buffer too small");
return 0;
}
memcpy(buf,rxbuf_data,packet_payload_len+1);
RIMESTATS_ADD(llrx);
CLEAR_RXBUF();
return 1;
}
/*---------------------------------------------------------------------------*/
static int lora_channel_clear(void)
{
return 0;
}
/*---------------------------------------------------------------------------*/
static int lora_receiving_packet(void)
{
return 0;
}
/*---------------------------------------------------------------------------*/
static int lora_pending_packet(void)
{
printf("pending packet\n");
return !IS_RXBUF_EMPTY();
}
/*---------------------------------------------------------------------------*/
static int lora_off(void)
{
printf("radio off\n");
RN2483_sleep(15000);
return 1;
}
/*---------------------------------------------------------------------------*/
static int lora_on(void)
{
printf("radio on\n");
lora_send((int)0x55);
return 1;
}
/*---------------------------------------------------------------------------*/
/*
 *system commands 
 */
/*---------------------------------------------------------------------------*/
int lora_initialize()
{
printf("initializing..\n");
process_start(&lora_process,NULL);
printf("done\n");
return 1;
}
/*---------------------------------------------------------------------------*/
void RN2483_version()
{
   lora_send("sys get ver\r\n");
   blink_led();
}
/*---------------------------------------------------------------------------*/
void RN2483_sleep(int duration)// #miliseconds the system will sleep
{
   char* c;
   char cduration[11];
   itoa(duration,cduration,10);
   c = malloc(strlen(cduration)+11);
   strcpy(c,"sys sleep ");
   strcat(c,cduration);
   strcat(c,"\r\n"); 
   printf("sleeping ...");
   lora_send(c);
   free(c);
   blink_led();
}
/*---------------------------------------------------------------------------*/
void RN2483_reset()
{
  lora_send("sys reset\r\n");
  blink_led();
}
/*---------------------------------------------------------------------------*/
void RN2483_Freset()
{
  lora_send("sys factoryRESET\r\n");
  blink_led();
}

/*---------------------------------------------------------------------------*/
void RN2483_StoreData(char* address,char* data) // address from 300 to 3FF, data from 00 to FF, chars because the values have to be entered in a hexadecimal values
{
   char* c;
   c = malloc(strlen(address)+strlen(data)+14);
   strcpy(c,"sys set nvm ");
   strcat(c,address);
   strcat(c," ");
   strcat(c,data);
   strcat(c,"\r\n"); 
   printf("data stored in EEPROM ...");
   lora_send(c);
   free(c);
   blink_led();
}
/*---------------------------------------------------------------------------*/
int get_system_settings(char *setting)
{
for(i=0;i<3;i++){
	if(!strcmp(system_enum[i],setting)){
	u=i;
	}
}
	switch(u) {
		   case 0 :
		      printf("version: \r\n");
		      lora_send("sys get ver\r\n");
		      break; 
		   case 1 :
		      printf("VDD (mV): \r\n");
		      lora_send("sys get vdd\r\n"); 
		      break; 
		   case 2 :
		      printf("programmed EUI node address: \r\n");
		      lora_send("sys get hweui\r\n");
		      break; 
		   default :
		      printf("error: wrong argument\r\n");
		      break; 
	}
u=10;
i=0;
blink_led();
return 1;
}
/*---------------------------------------------------------------------------*/
void RN2483_GetData(char* address)
{
   char* c;
   c = malloc(strlen(address)+13);
   strcpy(c,"sys get nvm ");
   strcat(c,address);
   strcat(c,"\r\n"); 
   printf("get data stored in EEPROM ... : ");
   lora_send(c);
   free(c);
   blink_led();
}
/*---------------------------------------------------------------------------*/
/*
 * MAC commands
 */
/*---------------------------------------------------------------------------*/
void mac_reset(int band)
{
   char* c;
   char cband[4];
   itoa(band,cband,10);
   c = malloc(strlen(cband)+11);
   strcpy(c,"mac reset ");
   strcat(c,cband);
   strcat(c,"\r\n"); 
   printf("reset ... : ");
   lora_send(c);
   free(c);
   blink_led();
}
/*---------------------------------------------------------------------------*/
void mac_send(char *type,int portnumber,char *data)
{
   char* c;
   char cportnumber[4];
   itoa(portnumber,cportnumber,10);
   c = malloc(strlen(type)+strlen(cportnumber)+strlen(data)+10);
   strcpy(c,"mac tx ");
   strcat(c,type);
   strcat(c," ");
   strcat(c,cportnumber);
   strcat(c," ");
   strcat(c,data);
   strcat(c,"\r\n"); 
   printf("sending ... : ");
   lora_send(c);
   free(c);
   blink_led();
}
/*---------------------------------------------------------------------------*/
void mac_join(char *mode)// otaa or abp // called by join OTAA or ABP
{
   if(!strcmp(mode,"otaa")){
	printf("joining network using otaa procedure: ... ");
	lora_send("mac join otaa\r\n");
   }
   else if(!strcmp(mode,"abp")){
	printf("joining network using abp procedure: ... ");
	lora_send("mac join abp\r\n");
   }
   else{printf("error: not a valid mode\n");}
   blink_led();
}
/*---------------------------------------------------------------------------*/
void mac_save()
{
   printf("Saving LoRaWAN Class A configuration parameters to EEPROM:... ");
   lora_send("mac save\r\n");
   blink_led();
}
/*---------------------------------------------------------------------------*/
void mac_force_enable()
{
   printf("Forcing enable: ... \r\n");
   lora_send("mac forceENABLE\r\n");
   blink_led();
}
/*---------------------------------------------------------------------------*/
int set_mac_settings(const char *first, ...)
{
	va_list argp;
	va_start(argp,first);
	vset_settings("mac set ",first,argp);
	va_end(argp);
	return 1;
}
/*---------------------------------------------------------------------------*/
int set_mac_channel_settings(const char *first, ...)
{
	va_list argp;
	va_start(argp,first);
	vset_settings("mac set ch ",first,argp);
	va_end(argp);
	return 1;
}
/*---------------------------------------------------------------------------*/
int get_mac_settings(const char *first, ...)
{
	static const char setting[]="mac get ";
	va_list argp;
	va_start(argp,first);
	vget_settings(setting,first,argp);
	va_end(argp);
	return 1;
}
/*---------------------------------------------------------------------------*/
int get_mac_channel_settings(const char *first, ...)
{
	va_list argp;
	va_start(argp,first);
	vset_settings("mac get ch ",first,argp);
	va_end(argp);
	return 1;
}
/*---------------------------------------------------------------------------*/
/*
 * radio commands 
 */
/*---------------------------------------------------------------------------*/
void radio_settings()
{
  ctimer_set(&timer, CLOCK_SECOND/20, BRScallback,NULL);//to print all the settings, it will take 6/20 seconds (0.3s)
}
/*---------------------------------------------------------------------------*/
void macpause()
{
  lora_send("mac pause\r\n");
}
/*---------------------------------------------------------------------------*/
void power(char *power)
{
  	char* c;
	c = malloc(strlen(power)+15);
	strcpy(c,"radio set pwr ");
        strcat(c,power);
        strcat(c,"\r\n");
        lora_send(c);
	blink_led();
}
/*---------------------------------------------------------------------------*/
void ptp_send(char *message)//point-to-point link: hex value; 48656c6c6f = hello /
{
        char* c;
	c = malloc(strlen(message)+20);
	strcpy(c,"radio tx ");
	printf("sending message: ");
        strcat(c,message);
        strcat(c,"\r\n");
        lora_send(c);
	blink_led();
}
/*---------------------------------------------------------------------------*/
void ptp_receive(int window)//point-to-point link    
{
   char* c;// if 0 wait until reception of a message
   char cwindow[20];
   itoa(window,cwindow,10);
   c = malloc(strlen(cwindow)+10);
   strcpy(c,"radio rx ");
   strcat(c,cwindow);
   strcat(c,"\r\n");
   printf("waiting for reception...");
   lora_send(c);
   free(c);
}
/*---------------------------------------------------------------------------*/
/*static void
ptpcallback(char ptr[24])
{
	if(i==0){
	printf("mac pause: \r\n");
	lora_send("mac pause\r\n");
	i++;
	ctimer_reset(&timer);
	}
	if(i==1){
	printf("%s",ptr[i-1]);
	lora_send(ptr[i-1]);
	i++;
	ctimer_reset(&timer);
	}
	else{
	printf("mac resume: ");
	lora_send("mac resume\r\n");
	i=0;
	blink_led();
	}
}*/
/*---------------------------------------------------------------------------*/
void cw_state(char *state)//continous wave state
{
   printf("pause mac: ...\r\n");
   printf("changing cw state: ...\r\n");
   lora_send("mac pause\r\n");
   Ldelay();
   if(!strcmp(state,"on")){lora_send("radio cw on\r\n");}
   else if(!strcmp(state,"off")) {lora_send("radio cw off\r\n");}
   else{printf("error: not a valid state\r\n");}
   Ldelay();
   lora_send("mac resume\r\n");
}
/*---------------------------------------------------------------------------*/
int set_radio_settings(const char *first, ...)
{
	va_list argp;
	va_start(argp,first);
	vset_settings("radio set ",first,argp);
	va_end(argp);
	return 1;
}
/*---------------------------------------------------------------------------*/
int get_radio_settings(const char *first, ...)
{
	va_list argp;
	va_start(argp,first);
	vget_settings("radio get ",first,argp);
	va_end(argp);
	return 1;
}
/*---------------------------------------------------------------------------*/
/*
* radio callback functions
*/
/*---------------------------------------------------------------------------*/
static void
BRScallback(void *ptr)
{
	   switch(i) {
	   case 0 :
	      printf("Radio transmission settings:\n");
	      printf("pause duration of the mac level: ");
	      lora_send("mac pause\r\n");// pause the mac level
	      i++;
	      ctimer_reset(&timer);
	      break; 
	   case 1 :
	      printf("mode: ");
	      lora_send("radio get mod\r\n");// get the used modulation technique
	      i++;
	      ctimer_reset(&timer);
	      break;
	   case 2 :
	      printf("frequency (Hz): ");
	      lora_send("radio get freq\r\n");// get the used frequency in Hertz
	      i++;
	      ctimer_reset(&timer);
	      break; 
	   case 3 :
	      printf("spread factor: ");
	      lora_send("radio get sf\r\n");// get the spread factor 
  //(increase sensitivity of the reception, increase duration of transmission) SNR can be up to -20dB
	      i++;
	      ctimer_reset(&timer);
	      break; 
    	   case 4 :
	      printf("bandwidth (KHz): ");
	      lora_send("radio get bw\r\n");// get the bandwidth
	      i++;
	      ctimer_reset(&timer);
	      break;  
	   case 5 :
	      printf("transmission power (dB): ");
	      lora_send("radio get pwr\r\n");// get the transmission power
	      i++;
	      ctimer_reset(&timer);
	      break; 
	   case 6 :
	      printf("resume mac: ");
	      lora_send("mac resume\r\n");
	      i=0;
	      blink_led();
	      break;
	  }
}
/*---------------------------------------------------------------------------*/
static void
RScallback(char ptr[20][24])
{
	if(i==0){
	printf("mac pause: \r\n");
	lora_send("mac pause\r\n");
	i++;
	ctimer_reset(&timer);
	}
	else{
		if(strcmp(ptr[i-1],"STOP")>0){
		printf("%s",ptr[i-1]);
		lora_send(ptr[i-1]);
		i++;
		ctimer_reset(&timer);
		}
		else{
		printf("mac resume: ");
		lora_send("mac resume\r\n");
		i=0;
		blink_led();
		}
	}
}
/*---------------------------------------------------------------------------*/
static void
MScallback(char ptr[20][71])
{
	if(strcmp(ptr[i],"STOP")>0){
	printf("%s",ptr[i]);
	lora_send(ptr[i]);
	i++;
	ctimer_reset(&timer);
	}
	else{
	i=0;
	blink_led();
	}
}
/*---------------------------------------------------------------------------*/
void blink_led()
{
  //clock_init();
  leds_on(LEDS_BLUE);
  //clock_delay(50);
  leds_off(LEDS_BLUE);
}
/*---------------------------------------------------------------------------*/

void join_abp(char *devaddr, char *nwkskey,char *appskey)
{
char radio_enum[20][71];
char *retbuf;
retbuf = malloc(71);
(void)strcpy(retbuf,"mac set devaddr ");
(void)strcat(retbuf,devaddr);
(void)strcat(retbuf,"\r\n");
(void)strcpy(radio_enum[0],retbuf);

(void)strcpy(retbuf,"mac set nwkskey ");
(void)strcat(retbuf,nwkskey);
(void)strcat(retbuf,"\r\n");
(void)strcpy(radio_enum[1],retbuf);

(void)strcpy(retbuf,"mac set appskey ");
(void)strcat(retbuf,appskey);
(void)strcat(retbuf,"\r\n");
(void)strcpy(radio_enum[2],retbuf);

(void)strcpy(radio_enum[3],"mac join abp\r\n");

(void)strcpy(radio_enum[4],"STOP");
free(retbuf);
ctimer_set(&timer, CLOCK_SECOND/20, MScallback,radio_enum);
}
/*---------------------------------------------------------------------------*/

void join_otaa(char *deveui, char *appeui,char *appkey)
{ 

char radio_enum[20][71];
char *retbuf;
retbuf = malloc(71);
(void)strcpy(retbuf,"mac set deveui ");
(void)strcat(retbuf,deveui);
(void)strcat(retbuf,"\r\n");
(void)strcpy(radio_enum[0],retbuf);

(void)strcpy(retbuf,"mac set appeui ");
(void)strcat(retbuf,appeui);
(void)strcat(retbuf,"\r\n");
(void)strcpy(radio_enum[1],retbuf);

(void)strcpy(retbuf,"mac set appkey ");
(void)strcat(retbuf,appkey);
(void)strcat(retbuf,"\r\n");
(void)strcpy(radio_enum[2],retbuf);

(void)strcpy(radio_enum[3],"mac join otaa\r\n");

(void)strcpy(radio_enum[4],"STOP");
free(retbuf);
ctimer_set(&timer, CLOCK_SECOND/20, MScallback,radio_enum);
}
/*---------------------------------------------------------------------------*/
void vset_settings(const char *setting, const char *first, va_list argp,...)
{
	size_t len;
        int callback=0;
	int drrange=0;
	int tel=0;
	char *retbuf;
	char *p;
	char radio_enum[20][71];//because real enum too long for printing
	if(!strcmp(setting,"mac set ")||!strcmp(setting,"mac set ch ")){callback=1;}
	else{callback=0;}
	if(first==NULL){printf("error: missing argument\n");}
	len = strlen(first)+strlen(setting);

	while((p=va_arg(argp, char*)) !=NULL){
		len += strlen(p);
	}
	va_end(argp); // reset ptr to first element
	va_start(argp, first);// initialize arguments to store all arguments 
	p =va_arg(argp, char*);
	//if(!strcmp(setting,"mac set ch ")){p =va_arg(argp, char*);}
	p =va_arg(argp, char*);// to avoid first 2 
	
	while((p =va_arg(argp, char*)) !=NULL){
		if(!strcmp(p,"drrange")){drrange=1;}
		else{drrange=0;}
		retbuf = malloc(len+1);// 0/ character
		if(retbuf == NULL){printf("error: missing argument\n");}
		(void)strcpy(retbuf,setting);
		if(tel==0){
			(void)strcat(retbuf, first);}
		else{
			(void)strcat(retbuf,p);
			p =va_arg(argp, char*);
		}
		if(!strcmp(retbuf,"mac set rx2")||!strcmp(setting,"mac set ch ")){
			(void)strcat(retbuf," ");
			(void)strcat(retbuf, p);
			(void)strcat(retbuf," ");
			(void)strcat(retbuf,va_arg(argp, char*));
			//printf("%s\n",retbuf);
		}
		else{(void)strcat(retbuf, " ");(void)strcat(retbuf, p);}
		if(drrange==1){
		(void)strcat(retbuf," ");
		(void)strcat(retbuf,va_arg(argp, char*));
		//printf("%s\n",retbuf);
		}
		(void)strcat(retbuf,"\r\n");
		//printf("%s",retbuf);
		strcpy(radio_enum[tel],retbuf);
		//printf("%s",radio_enum[0]);
		free(retbuf);
		tel++;
	}
	va_end(argp);
	strcpy(radio_enum[tel],"STOP");
	if(callback==0){ctimer_set(&timer, CLOCK_SECOND/20, RScallback,radio_enum);}
	else{ctimer_set(&timer, CLOCK_SECOND/20, MScallback,radio_enum);}
}
/*---------------------------------------------------------------------------*/
void vget_settings(const char *setting, const char *first, va_list argp,...)
{
	size_t len;
	int callback=0;
	int tel=0;
	char *retbuf;
	char *p;
	char radio_enum[20][71];//because real enum too long for printing
	if(!strcmp(setting,"mac get ")){callback=1;}
	else{callback=0;}
	if(first==NULL){printf("error: missing argument\n");}
	len = strlen(first)+strlen(setting);

	while((p=va_arg(argp, char*)) !=NULL){
		len += strlen(p);
	}
	va_end(argp); // reset ptr to first element
	va_start(argp, first);// initialize arguments to store all arguments 
	p=va_arg(argp, char*);
	while((p =va_arg(argp, char*)) !=NULL){
		retbuf = malloc(len+1);// 0/ character
		if(retbuf == NULL){printf("error: missing argument\n");}
		(void)strcpy(retbuf,setting);
		if(tel==0){
			(void)strcat(retbuf, first);}
		else{
			(void)strcat(retbuf,p);
		}
		if(!strcmp(retbuf,"mac get rx2")){(void)strcat(retbuf," ");(void)strcat(retbuf,va_arg(argp, char*));}
		(void)strcat(retbuf, "\r\n");
		strcpy(radio_enum[tel],retbuf);
		free(retbuf);
		tel++;
	}
	va_end(argp);
	strcpy(radio_enum[tel],"STOP");
	if(callback==0){ctimer_set(&timer, CLOCK_SECOND/20, RScallback,radio_enum);}
	else{ctimer_set(&timer, CLOCK_SECOND/20, MScallback,radio_enum);}
}
/*---------------------------------------------------------------------------*/
void Ldelay()
{
 rtimer_arch_init();
 rtimer_clock_t time =rtimer_arch_now();
while(RTIMER_CLOCK_LT(RTIMER_NOW(), time + INTER_PACKET_INTERVAL)) { }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(lora_process,ev,data)
{   
int len;
PROCESS_BEGIN();
printf("process started");
while(1) {
    PROCESS_YIELD_UNTIL(ev == lora_event_message);
    //packetbuf_clear();
    //strcpy(rxbuf_data,(char *)data);
    //len=lora_radio_read(packetbuf_dataptr(),PACKETBUF_SIZE);
    printf("%s\n", (char *)data);
    //packetbuf_set_datalen(len);

    //NETSTACK_RDC.input();
  }
  PROCESS_END();
}

