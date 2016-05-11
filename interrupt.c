
#include <p33FJ128MC804.h>

void SetInterruptPriority()
{
    IPC2bits.T3IP = 7;  //Servos - antiblock I2C - horodatage
    IPC0bits.T1IP = 6;  //Asservissement
    IPC6bits.T4IP = 5;  //System loop
    IPC1bits.T2IP = 4;  //Telemetres Ultra Son (I2C)
    IPC2bits.U1RXIP = 4; //réception des commandes
    IPC3bits.U1TXIP = 3; //Transmission de trames UART
}
