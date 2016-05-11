#include <p33FJ128MC804.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_RX1.h"
#include "UART_Protocol.h"

#define CBRX1_BUFFER_SIZE 128

int cbRx1Head;
int cbRx1Tail;
unsigned char cbRx1Buffer[CBRX1_BUFFER_SIZE];

void CB_RX1_Add(unsigned char value)
{
    if (CB_RX1_GetRemainingSize() > 0)
    {
        cbRx1Buffer[cbRx1Head] = value;
        if (cbRx1Head == (CBRX1_BUFFER_SIZE - 1))
            cbRx1Head = 0;
        else
            cbRx1Head++;
    }
}

unsigned char CB_RX1_Get(void)
{
    unsigned char value = cbRx1Buffer[cbRx1Tail];
    if (cbRx1Tail != cbRx1Head)
    {
        if (cbRx1Tail == CBRX1_BUFFER_SIZE - 1)
            cbRx1Tail = 0;
        else
            cbRx1Tail++;
    }
    return value;
}

unsigned char CB_RX1_IsDataAvailable(void)
{
    if (cbRx1Head != cbRx1Tail)
        return 1;
    else
        return 0;
}

void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;

    if (U1STAbits.FERR == 1)
    {
        U1STAbits.FERR = 0;
    }

    if (U1STAbits.OERR == 1)
    {
        U1STAbits.OERR = 0;
    }

    while(U1STAbits.URXDA == 1)
    {
        CB_RX1_Add(U1RXREG);
    }
}

unsigned char CB_RX1_GetRemainingSize(void)
{
    unsigned char rSizeRecep = CBRX1_BUFFER_SIZE;
    if (cbRx1Head > cbRx1Tail)
        rSizeRecep = CBRX1_BUFFER_SIZE - 1 - cbRx1Head + cbRx1Tail;
    else if (cbRx1Head < cbRx1Tail)
        rSizeRecep = cbRx1Head - cbRx1Tail;
    return rSizeRecep;
}

unsigned char CB_RX1_GetDataSize(void)
{
    unsigned char rSizeRecep = 0;
    if (cbRx1Head > cbRx1Tail)
        rSizeRecep = cbRx1Head - cbRx1Tail - 1;
    else if (cbRx1Head < cbRx1Tail)
        rSizeRecep = CBRX1_BUFFER_SIZE - cbRx1Tail + cbRx1Head - 2;
    return rSizeRecep;
}