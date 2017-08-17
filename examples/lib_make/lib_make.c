#include "stdlib.h"
#include "contiki.h"
#include "sys/etimer.h"
#include "dev/lora-reception.h"
#include "dev/lora-send.h"
#include "lib/RN2483.h"
#include <stdio.h> /* For printf() */
#define RELAY_INTERVAL (CLOCK_SECOND)
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
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
