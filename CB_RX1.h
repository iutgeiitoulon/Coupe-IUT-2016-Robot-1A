#ifndef CB_RX1_H
#define	CB_RX1_H

void CB_RX1_Add(unsigned char);
unsigned char CB_RX1_Get(void);
unsigned char CB_RX1_IsDataAvailable(void);
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void);
unsigned char CB_RX1_GetRemainingSize(void);
unsigned char CB_RX1_GetDataSize(void);

#endif	/* CB_RX1_H */

