/*
 * File:   IO.c
 * Author: E105-POSTE2
 *
 * Created on 7 septembre 2015, 12:03
 */


#include "IO.h"
#include <p33FJ128MC804.h>

void InitIO() {
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
    
    _TRISA8 = 0;//Servo Balloon Muderer
    
    _TRISC8 = 0;  //Tx1
    _TRISC7 = 0;  //Tx2

    //Configuration des entrées
    _TRISB4 = 1;//capteur contacte NF 
    _TRISA4 = 1;  //Entrée Jack NF
    _TRISB6= 1;//detecteur de couleur
    _TRISB5= 1;//detecteur de couleur
    
    CNEN1bits.CN0IE = 0; //Disable interrupt jack
    CNPU1bits.CN0PUE = 1; //Enable Pull-up for jack
    CNEN1bits.CN1IE=0;//Disable interrupt bumper
    CNPU1bits.CN1PUE=1;//Enable Pull-up for jack

    //Configuration des pins remappables
    UnlockIO();
    //TX/RX 1
    RPINR18bits.U1RXR = 0b11001; //Rx du PIC sur RP25 ()
    RPOR12bits.RP24R = 0b00011; //Tx du PIC sur RP25 
    //TX/RX 2
    RPINR19bits.U2RXR = 16; //Rx2 du PIC sur RP16 (SPI MISO sur la carte)
    //RPOR5bits.RP10R= 0b00101;
        
    //Configuration des pins remappables
    UnlockIO();
    //TX/RX 1
    RPINR18bits.U1RXR = 0b11001;          //Rx du PIC sur RP25 ()
    RPOR12bits.RP24R = 0b00011;       //Tx du PIC sur RP24 ()
        //TX/RX 2
    RPINR19bits.U2RXR= 22;          //Rx2 du PIC sur RP16 ()RP22
    RPOR11bits.RP23R= 0b00101;  //config du TX2 sur la pin RP23

    //QEI
    RPINR14bits.QEA1R = 20;
    RPINR14bits.QEB1R = 19;
    RPINR15bits.INDX1R = 21;

    RPINR16bits.QEA2R = 8;
    RPINR16bits.QEB2R = 7;
    RPINR17bits.INDX2R = 9;
    LockIO();



}

void LockIO() {
    asm volatile ("mov #OSCCON, w1 \n"
            "mov #0x46, w2 \n"
            "mov #0x57, w3 \n"
            "mov.b w2,[w1] \n"
            "mov.b w3,[w1] \n"
            "bset OSCCON, #6":: : "w1", "w2", "w3");
}

void UnlockIO() {
    asm volatile ("mov #OSCCON,w1 \n"
            "mov #0x46, w2 \n"
            "mov #0x57, w3 \n"
            "mov.b w2,[w1] \n"
            "mov.b w3,[w1] \n"
            "bclr OSCCON, #6":: : "w1", "w2", "w3");
}
