#include <p33FJ128MC804.h>
#include<stdio.h>
#include<stdlib.h>
#include "CB_TX2.h"
#define CBTX2_BUFFER_SIZE 256

int cbTx2Head=0;//tete
int cbTx2Tail=0;//queue
unsigned char cbTx2Buffer[CBTX2_BUFFER_SIZE];
unsigned char isTransmitting2 = 0;

void SendMessage2(unsigned char* message,int length)
{
    unsigned char i=0;
   
    if(CB_TX2_RemainingSize()>length)
    {
        for(i=0;i<length;i++)
            CB_TX2_Add(message[i]);
        if(!CB_TX2_IsTranmitting())
            SendOne2();    
    }
}
void CB_TX2_Add(unsigned char value)
{
    cbTx2Buffer[cbTx2Head]=value;
    if(cbTx2Head<CBTX2_BUFFER_SIZE-1)
    {
        cbTx2Head++;
    }
    else
        cbTx2Head=0;
}

unsigned char CB_TX2_Get(void)
{
    unsigned char value;
    value=cbTx2Buffer[cbTx2Tail];
    
    if (cbTx2Tail<CBTX2_BUFFER_SIZE-1)
    {
        cbTx2Tail++;
    }
    else 
        cbTx2Tail=0;
    return value;
}
void __attribute__((__interrupt__,__auto_psv__))_U2TXInterrupt(void)
{
    IFS1bits.U2TXIF=0;
    if(cbTx2Tail!=cbTx2Head)
    {
        SendOne2();
    }
    else
        isTransmitting2 = 0;
}

void SendOne2()//envoie un caractere
{
    isTransmitting2 = 1;
    unsigned char value =CB_TX2_Get();
    U2TXREG = value;//transmit one character
}

unsigned char CB_TX2_IsTranmitting(void)
{
    return isTransmitting2;
}

unsigned int CB_TX2_RemainingSize(void)
{
    unsigned int rSize;
    if(cbTx2Head >= cbTx2Tail)
        rSize=CBTX2_BUFFER_SIZE-(cbTx2Head-cbTx2Tail);
    else
        rSize=cbTx2Tail-cbTx2Head;
    return rSize;
}

