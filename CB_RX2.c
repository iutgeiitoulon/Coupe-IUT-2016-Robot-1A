
#include <p33FJ128MC804.h>
#include<stdio.h>
#include<stdlib.h>
#include "CB_RX2.h"
#include "LIDAR.h"
#include "IO.h"

#define CBRX2_BUFFER_SIZE 256

unsigned int cbRX2Head = 0;
unsigned int cbRX2Tail = 0;
unsigned char cbRx2Buffer[CBRX2_BUFFER_SIZE];

void CB_RX2_Add(unsigned char value)
{
    if (CB_RX2_GetRemainingSize() > 0)
    {
        cbRx2Buffer[cbRX2Head] = value;
        if (cbRX2Head < CBRX2_BUFFER_SIZE)
        {
            cbRX2Head++;
        }
        else
            cbRX2Head = 0;
    }
}

unsigned char CB_RX2_Get(void)
{
    unsigned char value = cbRx2Buffer[cbRX2Tail];
    if (cbRX2Tail < CBRX2_BUFFER_SIZE)
    {
        cbRX2Tail++;
    }
    else
        cbRX2Tail = 0;
    return value;
}

unsigned char CB_RX2_IsDataAvailable(void)
{
    if (cbRX2Head != cbRX2Tail)
        return 1;
    else
        return 0;
}

void __attribute__((__interrupt__, __auto_psv__)) _U2RXInterrupt(void)
{

    IFS1bits.U2RXIF = 0; //clear RX interrupt flg
    /*check for receive errors*/
    if (U2STAbits.FERR == 1)
    {
        U2STAbits.FERR = 0;
    }
    /*must clear the overrun error to keep uart receiving*/
    if (U2STAbits.OERR == 1)
    {
        U2STAbits.OERR = 0;
    }
    /*get the data*/
    while (U2STAbits.URXDA == 1)
    {
        unsigned char c = U2RXREG;
        CB_RX2_Add(c);
    }
}

unsigned int CB_RX2_GetRemainingSize(void)
{
    unsigned int rSizeRecep;
    if (cbRX2Head >= cbRX2Tail)
        rSizeRecep = CBRX2_BUFFER_SIZE - (cbRX2Head - cbRX2Tail);
    else
        rSizeRecep = cbRX2Tail - cbRX2Head;
    return rSizeRecep;
}

unsigned int CB_RX2_GetDataSize(void)
{
    return CBRX2_BUFFER_SIZE - CB_RX2_GetRemainingSize();
}
