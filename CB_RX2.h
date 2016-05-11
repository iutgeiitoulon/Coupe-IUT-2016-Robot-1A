/* 
 * File:   CB_RX2.h
 * Author: Valentin
 *
 * Created on 9 mars 2016, 23:11
 */

#ifndef CB_RX2_H
#define	CB_RX2_H

void CB_RX2_Add(unsigned char value);
unsigned char CB_RX2_Get(void);
unsigned char CB_RX2_IsDataAvailable(void);
void __attribute__((__interrupt__,__auto_psv__)) _U2RXInterrupt(void);
unsigned int CB_RX2_GetRemainingSize(void);
unsigned int CB_RX2_GetDataSize(void);

#endif	/* CB_RX2_H */

