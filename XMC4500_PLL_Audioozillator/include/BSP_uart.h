/*
 * BSP_uart.h
 *
 *  Created on: 07.06.2017
 *      Author: Thomas
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_

#include <stdint.h>
/******************************************************************** GLOBALS */
/******************************************************************** FUNCTION PROTOTYPES */
void _initUART0_CH0(void);
uint8_t _uart_send_char(char c);
uint8_t _uart_printf(char *fmt, ...);
uint8_t _uart_send_string(char *str);
uint8_t _uart_get_string (char *str);
#endif /* BSP_UART_H_ */

/* EOF */
