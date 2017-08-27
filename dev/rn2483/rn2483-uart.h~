/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
/**
 * \file
 * Generic serial I/O process header file, UART1
 * \author
 * Adam Dunkels
 * modified by Quentin Quevy
 *
 */
#ifndef RN2483_UART_H_
#define RN2483_UART_H_

#include "contiki.h"

/**
 * Event posted when a line of input has been received from UART1 ( especially made for ZL1 shield ) .
 *
 * This event is posted when an entire line of input has been received
 * from the serial port. A data pointer to the incoming line of input
 * is sent together with the event. 
 * This file is needed to distinguish input UART0 from UART1
 * Another event is posted. Here called lora_event_message
 */
extern process_event_t lora_event_message;

/**
 * Get one byte of input from the serial driver.
 *
 * This function is to be called from the actual RS232 driver to get
 * one byte of serial data input.
 *
 * For systems using low-power CPU modes, the return value of the
 * function can be used to determine if the CPU should be woken up or
 * not. If the function returns non-zero, the CPU should be powered
 * up. If the function returns zero, the CPU can continue to be
 * powered down.
 *
 * \param c The data that is received.
 *
 * \return Non-zero if the CPU should be powered up, zero otherwise.
 */

int lora_input_byte(unsigned char c);

void lora_reception_init(void);

void lora_send(char *c);

PROCESS_NAME(lora_reception_process);

#endif /* LORA_RECEPTION_H_ */
