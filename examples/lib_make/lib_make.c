#include "contiki.h"
#include "sys/etimer.h"
#include "lib/RN2483.h"
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
  etimer_set(&et, 5*CLOCK_SECOND);
  lora_initialize();
  while(1){
	  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	  radio_settings();
	  etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/