/* 
 * File:   UART.h
 * Author: E105-POSTE2
 *
 * Created on 18 novembre 2015, 10:14
 */

#ifndef UART_H
#define	UART_H

void InitUART1(void);
void SendMessageDirect(unsigned char * message, int length);
void InitUART2(void);

#endif	/* UART_H */

