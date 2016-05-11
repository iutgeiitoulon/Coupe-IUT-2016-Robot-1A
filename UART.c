#include <p33FJ128MC804.h>
#include "UART.h"
#include "oscillator.h"

#define BAUDRATE_38400 38400
#define BAUDRATE_115200 115200
#define BRGVAL_38400 ((FCY/BAUDRATE_38400)/4)-1
#define BRGVAL_115200 ((FCY/BAUDRATE_115200)/4)-1

void InitUART1(void)
{
    U1MODEbits.STSEL = 0; // 1-stop bit
    U1MODEbits.PDSEL = 0; //No Parity, 8-data bits
    U1MODEbits.ABAUD = 0; //Auto-Baud Disabled
    U1MODEbits.BRGH = 1;//low speed mode
    U1BRG = BRGVAL_38400;//baud rate setting
   
    U1STAbits.UTXISEL0=0;//Interrupt after one Tx character is transmitted
    U1STAbits.UTXISEL1=0;
    IFS0bits.U1TXIF=0;//clear TX interrupt flag
    IEC0bits.U1TXIE=1;//Enable UART TX interrupt
    
    U1STAbits.URXISEL = 1;//Interrupt after one RX character is received
    IFS0bits.U1RXIF = 0;//clear RX interrupt flag
    IEC0bits.U1RXIE = 1;//Enable UART Rx interrupt
    
    U1MODEbits.UARTEN = 1;//Enable UART
    U1STAbits.UTXEN =1;//Enable UART Tx
    
    int i =0;
    for(i=0;i<4160;i++)
    {
        Nop();
    }
}

void SendMessageDirect(unsigned char* message, int length)
{
    unsigned char i=0;
    for (i=0;i<length;i++)
    {
        while (U1STAbits.UTXBF);
        U1TXREG= *(message)++;
    }
}

//void __attribute((__interrupt__,__auto_psv__)) _U1RXInterrupt(void)
//{
//    IFS0bits.U1RXIF = 0;
//    if (U1STAbits.FERR == 1)
//        U1STAbits.FERR = 0;
//    
//    if (U1STAbits.OERR == 1)
//        U1STAbits.OERR = 0;
//    
//    while (U1STAbits.URXDA == 1)
//        U1TXREG = U1RXREG;
//}


void InitUART2(void)
{
    U2MODEbits.STSEL = 0; // 1-stop bit
    U2MODEbits.PDSEL = 0; //No Parity, 8-data bits
    U2MODEbits.ABAUD = 0; //Auto-Baud Disabled
    U2MODEbits.BRGH = 1;//low speed mode
    U2BRG = BRGVAL_115200;//baud rate setting
   
    U2STAbits.UTXISEL0=0;//Interrupt after one Tx character is transmitted
    U2STAbits.UTXISEL1=0;
    IFS1bits.U2TXIF=0;//clear TX interrupt flag
    IEC1bits.U2TXIE=1;//Enable UART TX interrupt
    
    U2STAbits.URXISEL = 1;//Interrupt after one RX character is received
    IFS1bits.U2RXIF = 0;//clear RX interrupt flag
    IEC1bits.U2RXIE = 1;//Enable UART Rx interrupt
    IPC7bits.U2RXIP = 6; //Une priorité élevée est nécessaire pour éviter les 
                         //pertes de flux d'arrivée. Pas grave car on stocke un CB
    
    U2MODEbits.UARTEN = 1;//Enable UART
    U2STAbits.UTXEN =1;//Enable UART Tx
    
    int i =0;
    for(i=0;i<4160;i++)
    {
        Nop();
    } 
}