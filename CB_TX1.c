#include <p33FJ128MC804.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_TX1.h"
#define CBTX1_BUFFER_SIZE 256

unsigned int cbTx1Tail;
unsigned int cbTx1Head;
unsigned char cbTx1Buffer[CBTX1_BUFFER_SIZE];
unsigned char isTransmitting = 0;

void SendMessage(unsigned char* message, int length)
{
    unsigned char i=0;
    if (CB_TX1_RemainingSize()>length) {
        for (i=0;i<length;i++)
            CB_TX1_Add(message[i]);
        
        if (!CB_TX1_IsTransmitting())
            SendOne();
    }
}

void CB_TX1_Add(unsigned char value)
{
    if (CB_TX1_RemainingSize() != 0){
        cbTx1Buffer[cbTx1Head] = value;
        if (cbTx1Head == CBTX1_BUFFER_SIZE-1)
            cbTx1Head = 0;
        else
            cbTx1Head++;
    }
}

unsigned char CB_TX1_Get(void)
{
    unsigned char value = 0;
    if (CB_TX1_RemainingSize() > 0){
        value = cbTx1Buffer[cbTx1Tail];
        if (cbTx1Tail == CBTX1_BUFFER_SIZE-1)
            cbTx1Tail = 0;
        else
            cbTx1Tail++;
    }
    return value;
}

void __attribute__((__interrupt__,__auto_psv__)) _U1TXInterrupt(void)
{
    IFS0bits.U1TXIF = 0;
    if (cbTx1Head != cbTx1Tail)
    {
        SendOne();
    }
    else
        isTransmitting = 0;
}

void SendOne()
{
    isTransmitting = 1;
    unsigned char value=CB_TX1_Get();
    U1TXREG = value;
    
}

unsigned char CB_TX1_IsTransmitting(void)
{
    return isTransmitting;
}

unsigned int CB_TX1_RemainingSize(void)
{
    unsigned int rSize=CBTX1_BUFFER_SIZE;
    if (cbTx1Head>cbTx1Tail)
        rSize=CBTX1_BUFFER_SIZE-1-cbTx1Head+cbTx1Tail;
    else if (cbTx1Head<cbTx1Tail)
        rSize=cbTx1Tail-cbTx1Head;
    return rSize;
}