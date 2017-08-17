/**
 * \file
 * output uart1 header file, used for LZ1 shield
 * \author
 * Quentin Quévy
 *
 */
#ifndef LORA-SEND_H_  
#define LORA-SEND_H_  

#include "contiki.h"
/**
 * allows the user to send char arrays on the uart1 port
 *
 * The array is send byte by byte.
 * when a newline character is detected, 
 * sending is stopped
 */
int lora_send(char *c);

#endif /* LORA_H_ */
