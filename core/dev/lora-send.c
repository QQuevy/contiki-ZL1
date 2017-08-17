#include "contiki.h"
#include "uart1.h"
/*---------------------------------------------------------------------------*/
int
lora_send(char *c)
{
int i=0;
while(1){
   uart1_writeb(c[i]);
   if(c[i]==0x0A){ // if a newline charachter is detected stop sending
      break;
   }
i++;
}
 return 1;
}
