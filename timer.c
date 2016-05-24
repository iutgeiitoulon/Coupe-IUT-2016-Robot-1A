#include <stdio.h>
#include <stdlib.h>
#include "p33Fxxxx.h"
#include <libpic30.h>
#include "main.h"
#include "timer.h"
#include "oscillator.h"
#include "IO.h"
#include "ADC.h"
#include "PWM.h"
#include "QEI.h"
#include "AsservissementPolaire.h"
#include "ustv_i2c.h"
#include "Servo.h"
#include "events.h"
#include "robot.h"
#include "TaskVidageChateau.h"
#include "TaskFermeturePortes.h"
#include "TaskAvalageChateau.h"
#include "TaskPechePoisson.h"
#include "RobotManagement.h"

char consignePWM = 0;
unsigned long timestamp;

/*******************************************************************************
 * TIMER1                            
 *******************************************************************************/
//Initialisation d'un timer 16 bits
void InitTimer1(void)
{
    T1CONbits.TON = 0;
    T1CONbits.TSIDL = 0;
    T1CONbits.TGATE = 0;
    T1CONbits.TCS = 0;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    SetFreqTimer1(FREQ_ECH_QEI);
    T1CONbits.TON = 1;
}

//Interruption du timer 1

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;    
    QEIUpdateData(); //avant c'était en dernier...
    if (robotState.ModeAsservissement == ASSERVISSEMENT_POSITION)
    {
        //On calcule les nouvelles consignes de vitesse
        int* consigneSpeed = AsservissementPosition();
        PWMSetSpeedConsigne(consigneSpeed[0], MOTEUR_DROIT);
        PWMSetSpeedConsigne(consigneSpeed[1], MOTEUR_GAUCHE);
    }
    if (robotState.ModeAsservissement == ASSERVISSEMENT_ANGLE)
    {
        //On calcule les nouvelles consignes de vitesse
        int* consigneSpeed = AsservissementAngle();
        PWMSetSpeedConsigne(consigneSpeed[0], MOTEUR_DROIT);
        PWMSetSpeedConsigne(consigneSpeed[1], MOTEUR_GAUCHE);
    }
    PWMUpdateSpeed(1);
    PWMUpdateSpeed(2);
    PWMUpdateSpeed(3);
}

void SetFreqTimer1(float freq)
{
    T1CONbits.TCKPS = 0b00;
    if (FCY / freq > 65535)
    {
        T1CONbits.TCKPS = 0b01;
        if (FCY / freq / 8 > 65535)
        {
            T1CONbits.TCKPS = 0b10;
            if (FCY / freq / 64 > 65535)
            {
                T1CONbits.TCKPS = 0b11;
                PR1 = (int) (FCY / freq / 256);
            }
            else
                PR1 = (int) (FCY / freq / 64);
        }
        else
            PR1 = (int) (FCY / freq / 8);
    }
    else
        PR1 = (int) (FCY / freq);
}


/*******************************************************************************
 * TIMER2                            
 *******************************************************************************/
//Initialisation d'un timer 16 bits

void InitTimer2(void)
{
    //Timer2
    T2CONbits.TON = 0; // Disable Timer
    T2CONbits.TSIDL = 0; //continue in idle mode
    T2CONbits.TGATE = 0; //Accumulation disabled
    T2CONbits.TCS = 0; //clock source = internal clock
    SetFreqTimer2(10);

    IFS0bits.T2IF = 0; // Clear Timer Interrupt Flag
    IEC0bits.T2IE = 1; // Enable Timer interrupt
    T2CONbits.TON = 1; // Enable Timer
}

void SetFreqTimer2(float freq)
{
    T2CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535)
    {
        T2CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535)
        {
            T2CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535)
            {
                T2CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR2 = (int) (FCY / freq / 256);
            }
            else
                PR2 = (int) (FCY / freq / 64);
        }
        else
            PR2 = (int) (FCY / freq / 8);
    }
    else
        PR2 = (int) (FCY / freq);
}

int countTimer2 = 0;
//Interruption du timer 2

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;
    sysEvents.UltrasonicRangeFinderEvent = 1;
}


/*******************************************************************************
 * TIMER2                            
 *******************************************************************************/
//Initialisation d'un timer 16 bits

void InitTimer3(void)
{
    //Timer3
    T3CONbits.TON = 0; // Disable Timer
    T3CONbits.TSIDL = 0; //continue in idle mode
    T3CONbits.TGATE = 0; //Accumulation disabled
    T3CONbits.TCS = 0; //clock source = internal clock
    SetFreqTimer3(10000);

    IFS0bits.T3IF = 0; // Clear Timer Interrupt Flag
    IEC0bits.T3IE = 1; // Enable Timer interrupt
    T3CONbits.TON = 1; // Enable Timer
}

void SetFreqTimer3(float freq)
{
    T3CONbits.TCKPS = 0b00; //00 = 1:1 prescaler value
    if (FCY / freq > 65535)
    {
        T3CONbits.TCKPS = 0b01; //01 = 1:8 prescaler value
        if (FCY / freq / 8 > 65535)
        {
            T3CONbits.TCKPS = 0b10; //10 = 1:64 prescaler value
            if (FCY / freq / 64 > 65535)
            {
                T3CONbits.TCKPS = 0b11; //11 = 1:256 prescaler value
                PR3 = (int) (FCY / freq / 256);
            }
            else
                PR3 = (int) (FCY / freq / 64);
        }
        else
            PR3 = (int) (FCY / freq / 8);
    }
    else
        PR3 = (int) (FCY / freq);
}

//Interruption du timer 3
unsigned char countT2=0;

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void)
{
    IFS0bits.T3IF = 0;
    ManageServos();
    //IncrementSPIAntiBlockCounter();
    IncrementI2CAntiBlockCounter();
    
    //division par 10 de la fréquence à 10kHz pour horodatage à 1 ms
    countT2++;
    if (countT2 >= 10)
    {
        timestamp++;   
        if (timestamp > DUREE_MATCH)
        {
//            stateRobot = STATE_FIN_MATCH;            
//            robotState.taskEnCours = NO_TASK;
//            //SetRobotVitesseLibre(0, 0);
//           robotState.ModeAsservissement = ASSERVISSEMENT_DESACTIVE;
//            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
//            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
//            PWMSetSpeedConsigne(0, MOTEUR_ROULEAU);
//            
//            SetRobotTargetPosition(robotState.xPosFromOdometry, robotState.yPosFromOdometry);
//            SetRobotVitesseLibre(0, 0);       
//            SetConsigneServoPinceDroit(30);
        }
        countT2 = 0;
    }
}


/*******************************************************************************
 * TIMER 4                            
 *******************************************************************************/
//Initialisation d'un timer 16 bits
void InitTimer4(void)
{
    //Timer4 pour horodater les mesures (1ms)
    T4CONbits.TON = 0;
    T4CONbits.TSIDL = 0;
    T4CONbits.TGATE = 0;
    T4CONbits.TCS = 0;
    IFS1bits.T4IF = 0;
    IEC1bits.T4IE = 1;
    SetFreqTimer4(1000);
    T4CONbits.TON = 1;
}

void SetFreqTimer4(float freq)
{
    T4CONbits.TCKPS = 0b00;
    if (FCY / freq > 65535)
    {
        T4CONbits.TCKPS = 0b01;
        if (FCY / freq / 8 > 65535)
        {
            T4CONbits.TCKPS = 0b10;
            if (FCY / freq / 64 > 65535)
            {
                T4CONbits.TCKPS = 0b11;
                PR4 = (int) (FCY / freq / 256);
            }
            else
                PR4 = (int) (FCY / freq / 64);
        }
        else
            PR4 = (int) (FCY / freq / 8);
    }
    else
        PR4 = (int) (FCY / freq);
}

//Interruption du timer 4
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void)
{
    IFS1bits.T4IF = 0;
    SystemStateMachine();
    
//    LED_BLANCHE = !LED_BLANCHE;
//    
//    switch(robotState.taskEnCours)
//    {
//        case NO_TASK:
//            break;
//        case TASK_VIDAGE_CHATEAU:
//            TaskVidageChateau();
//            break;            
//        case TASK_FERMETURE_PORTES:
//            TaskFermeturePortes();
//            break; 
//        case TASK_AVALAGE_CHATEAU:
//            TaskAvalageChateau();
//            break;
//        case TASK_PECHE_POISSON:
//            TaskPechePoisson();
//            break;
//        default:
//            break;
//    }
}