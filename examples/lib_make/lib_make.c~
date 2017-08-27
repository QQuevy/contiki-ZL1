#include "contiki.h"
#include "sys/etimer.h"
//#include "dev/rn2483/rn2483-uart.h"
//#include "dev/rn2483/RN2483.h"
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
  etimer_set(&et, 7*CLOCK_SECOND);
  lora_initialize();
  //set_radio_settings("pwr","14","pwr","14",(char *)NULL);
  static int i=0;
  while(1){
	  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	  if(i==0){
		lora_send("mac pause\r\n");
	  }
	  if(i==1){
		lora_send("radio set pwr 14\r\n");
	  }
	  if(i==2)
	  {
		lora_send("radio tx 1234\r\n");
	  }
	  if(i==4)
	  {
		lora_send("radio get pwr\r\n");
	 	i=1;
	  }
	  i++;
	  //ptp_send("1234");
	  //lora_send("radio tx 1234\r\n");
	  //ptp_receive(0);
	  //RN2483_version();
	  //RN2483_sleep(5000);
	  etimer_reset(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
