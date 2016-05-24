/*
 * File:   IO.c
 * Author: E105-POSTE2
 *
 * Created on 7 septembre 2015, 12:03
 */


#include "IO.h"
#include <p33FJ128MC804.h>

void InitIO()
{
    //IMPORTANT : désactive les entrées analogiques, sinon on perd les entrées numériques
    AD1PCFGL = 0xFFFF;

    //Configuration des sorties

    //LED
    _TRISA7 = 0; //Led 1 sur RA7
    _TRISA9 = 0; //Led 2 sur RA9
    _TRISA10 = 0; //LED 3 sur RA10
    _TRISB14 = 0; //MOTOR_DROIT_IN1 sur RB14
    _TRISB15 = 0; //MOTOR_DROIT_IN2 sur RB15
    _TRISB12 = 0; //MOTOR_GAUCHE_IN1 sur RB12
    _TRISB13 = 0; //MOTOR_GAUCHE_IN2 sur RB13
    _TRISB10 = 0; //MOTOR_ROULEAU_IN1 sur RB10
    _TRISB11 = 0; //MOTOR_ROULEAU_IN2 sur RB11
    
    _TRISA4 = 0; //servo 1
    _TRISB4 = 0; //servo 2
    _TRISA8 = 0; //servo 3
    _TRISC8 = 0; //servo 4
    _TRISC6 = 0; //servo 5

    //Configuration des entrées
    _TRISC1 = 1;  //Entrée Jack
    CNEN1bits.CN9IE = 0; //Diaable interrupt for CN pin
    CNPU1bits.CN9PUE = 1; //Enable Pull-up for CN pin
    
    _TRISC9 = 1;  //Entrée Sélecteur Couleur
    CNEN2bits.CN19IE = 0; //Disable interrupt for CN pin
    CNPU2bits.CN19PUE = 1; //Enable Pull-up for CN pin

    //Configuration des pins remappables
    UnlockIO();
    //TX/RX 1
    RPINR18bits.U1RXR = 25;          //Rx du PIC sur RP24 
    RPOR12bits.RP24R = 0b00011;       //Tx du PIC sur RP25 
    //TX/RX 2
    RPINR19bits.U2RXR= 16;          //Rx2 du PIC sur RP16 (SPI MISO sur la carte)
    //RPOR5bits.RP10R= 0b00101;

    //QEI
    RPINR14bits.QEA1R = 20;
    RPINR14bits.QEB1R = 19;
    RPINR15bits.INDX1R = 21;

    RPINR16bits.QEA2R = 8;
    RPINR16bits.QEB2R = 7;
    RPINR17bits.INDX2R = 9;
    LockIO();


}

void LockIO()
{
    asm volatile ("mov #OSCCON, w1 \n"
            "mov #0x46, w2 \n"
            "mov #0x57, w3 \n"
            "mov.b w2,[w1] \n"
            "mov.b w3,[w1] \n"
            "bset OSCCON, #6":: : "w1", "w2", "w3");
}

void UnlockIO()
{
    asm volatile ("mov #OSCCON,w1 \n"
            "mov #0x46, w2 \n"
            "mov #0x57, w3 \n"
            "mov.b w2,[w1] \n"
            "mov.b w3,[w1] \n"
            "bclr OSCCON, #6":: : "w1", "w2", "w3");
}
