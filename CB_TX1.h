/* 
 * File:   CB_TX1.h
 * Author: E105-POSTE2
 *
 * Created on 18 novembre 2015, 11:45
 */

#ifndef CB_TX1_H
#define	CB_TX1_H

void SendMessage(unsigned char* message, int);
void CB_TX1_Add(unsigned char);
unsigned char CB_TX1_Get(void);
void __attribute__((__interrupt__,__auto_psv__)) _U1TXInterrupt(void);
void SendOne();
unsigned char CB_TX1_IsTransmitting(void);
unsigned int CB_TX1_RemainingSize(void);

#endif	/* CB_TX1_H */

