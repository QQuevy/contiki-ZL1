#include "contiki.h"
#include "sys/etimer.h"
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
  etimer_set(&et, 2*CLOCK_SECOND);
  lora_initialize();
  set_radio_settings("wdt","0",(char*)NULL);
  
  static int i=1;
  while(1){
	  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	  if(i==1){
		macpause();
		i=2;
	  }
	  else{
		ptp_receive("0");
	  }
	  etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
