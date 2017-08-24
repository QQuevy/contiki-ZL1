#include "contiki.h"
#include "sys/etimer.h"
#include "dev/rn2483/rn2483-uart.h"
#include "dev/rn2483/RN2483.h"
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
  etimer_set(&et, 10*CLOCK_SECOND);
  lora_initialize();
  //set_radio_settings("pwr","14",(char *)NULL);
  while(1){
	  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	  ptp_send("1234");
	  //lora_send("radio tx 1234\r\n");
	  //ptp_receive(0);
	  //RN2483_version();
	  //RN2483_sleep(5000);
	  etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
