#include "contiki.h"
#include "sys/etimer.h"
#include "dev/button-sensor.h"
#include <stdlib.h>
#include "dev/leds.h"
#include "dev/z1-phidgets.h"
#include <stdio.h> 
#define RELAY_INTERVAL (CLOCK_SECOND)
/*---------------------------------------------------------------------------*/
PROCESS(radio_settings_process, "asking for the actual radio settings process");
AUTOSTART_PROCESSES(&radio_settings_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(radio_settings_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();
  etimer_set(&et, CLOCK_SECOND);
  lora_initialize();
  SENSORS_ACTIVATE(phidgets);
  SENSORS_ACTIVATE(button_sensor);
  power("10");
  static int i=0;
  while(1){
	  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	  if(i==0){
		macpause();
		i=1;
	  }
	  else
	  {
	  char illuminance[5];
	  float lux=phidgets.value(PHIDGET3V_2)/4.095;// voltage=value*3.3/4095 and lux=voltage*1000/3.3 (approximation)
	  itoa((int)lux, illuminance, 10);
	  printf("illuminance:%s\n", illuminance);

	   char* c;
	   c = malloc(strlen(illuminance)+10);
	   strcpy(c,"radio tx ");
	   strcat(c,illuminance);
	   strcat(c,"\r\n");
	   lora_send(c);
	   free(c);
	   //ptp_send(C);
	  }
	  etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
