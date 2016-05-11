#include <p33FJ128MC804.h>
#include <stdio.h>
#include "PWM.h"
#include "IO.h"
#include "QEI.h"
#include "AsservissementVitesse.h"
#include "AsservissementPolaire.h"
#include "events.h"
#include "timer.h"
#include "UART_Protocol.h"
#include "robot.h"
#include "Toolbox.h"

#ifdef GROS_ROBOT
#define SPEED_FACTOR 30.0  //25.0 au départ
#elif defined(PETIT_ROBOT)
#define SPEED_FACTOR 10.0  //25.0 au départ
#endif

long timestampSansCollisionDroit = 0;
long timestampSansCollisionGauche = 0;

//unsigned char acceleration = 1;
//signed char PWMSpeedConsigne[4] = {0, 0, 0, 0};
//double PWMSpeedCorrectorOutput[4] = {0, 0, 0, 0};

void InitPWM(void)
{
    P1TCONbits.PTMOD = 0b00;
    P1TCONbits.PTCKPS = 0b00;
    P1TCONbits.PTOPS = 0b00;
    P1TPER = 1000;

    PWM1CON1bits.PMOD1 = 1;
    PWM1CON1bits.PMOD2 = 1;
    PWM1CON1bits.PMOD3 = 1;
    PWM1CON2bits.IUE = 1;

    P1OVDCONbits.POVD1H = 1;
    P1OVDCONbits.POVD1L = 1;
    P1OVDCONbits.POVD2H = 1;
    P1OVDCONbits.POVD2L = 1;
    P1OVDCONbits.POVD3H = 1;
    P1OVDCONbits.POVD3L = 1;

    P1TCONbits.PTEN = 1;
}

void PWMSetSpeedConsigne(double vitesseEnPourcents, char moteur)
{
    //On borne les valeurs d'entrée des vitesse en pourcentage de la vitesse max.
    vitesseEnPourcents = Min(vitesseEnPourcents, 100);
    vitesseEnPourcents = Max(vitesseEnPourcents, -100);

    switch (moteur)
    {
        case MOTEUR_DROIT:
            robotState.vitesseDroitConsigne = vitesseEnPourcents;
            break;
        case MOTEUR_GAUCHE:
            robotState.vitesseGaucheConsigne = vitesseEnPourcents;
            break;
        case MOTEUR_3:
            robotState.vitesseMoteur3Consigne = vitesseEnPourcents;
            break;
        case MOTEUR_4:
            robotState.vitesseMoteur4Consigne = vitesseEnPourcents;
            break;
    }
}

void PWMUpdateSpeed(unsigned char pwmNumber)
{
    //Correcteur PID
    double error = 0;
    if (robotState.ModeAsservissement == ASSERVISSEMENT_DESACTIVE)
    {
        switch (pwmNumber)
        {
            case MOTEUR_DROIT:
                robotState.vitesseDroitSortieCorrecteur = robotState.vitesseDroitConsigne;
                break;
            case MOTEUR_GAUCHE:
                robotState.vitesseGaucheSortieCorrecteur = robotState.vitesseGaucheConsigne;
                break;
            case MOTEUR_3:
                robotState.vitesseMoteur3SortieCorrecteur = robotState.vitesseMoteur3Consigne;
                break;
            case MOTEUR_4:
                robotState.vitesseMoteur4SortieCorrecteur = robotState.vitesseMoteur4Consigne;
                break;
        }
    }
    else
    {
        switch (pwmNumber)
        {
            case MOTEUR_DROIT:
                error = robotState.vitesseDroitConsigne - robotState.vitesseDroitFromOdometry / SPEED_FACTOR;
                robotState.vitesseDroitSortieCorrecteur = CorrecteurVitesseDroit(error);

                //Limitation du range de consignes à + ou - 100 %
                robotState.vitesseDroitSortieCorrecteur = Min(robotState.vitesseDroitSortieCorrecteur, 100);
                robotState.vitesseDroitSortieCorrecteur = Max(robotState.vitesseDroitSortieCorrecteur, -100);

                //Détection des blocage roue en observant la valeur de l'erreurdu correcteur
                if (robotState.vitesseDroitSortieCorrecteur > 70)
                {
                    //On détecte une erreur importante sur les correcteurs qui indique que 
                    //on est en blocage de roues
                    if ((timestamp - timestampSansCollisionDroit) > 500) //0.5 seconde
                        sysEvents.CollisionMoteurDroitEvent = COLLISION_AVANT;
                }
                else if (robotState.vitesseDroitSortieCorrecteur<-70)
                {
                    if ((timestamp - timestampSansCollisionDroit) > 500) //0.5 seconde                    
                        sysEvents.CollisionMoteurDroitEvent = COLLISION_ARRIERE;
                }
                else
                    timestampSansCollisionDroit = timestamp;

                //Mise à jour des consignes des hacheurs
                if (robotState.vitesseDroitSortieCorrecteur > 0)
                {
                    PWM1CON1bits.PEN1L = 0;
                    MOTOR_DROIT_IN2 = 1;
                    PWM1CON1bits.PEN1H = 1;
                }
                else
                {
                    PWM1CON1bits.PEN1H = 0;
                    MOTOR_DROIT_IN1 = 1;
                    PWM1CON1bits.PEN1L = 1;
                }
                P1DC1 = (unsigned int)(100 - Abs(robotState.vitesseDroitSortieCorrecteur)*20);
                break;

            case MOTEUR_GAUCHE:
                error = robotState.vitesseGaucheConsigne - robotState.vitesseGaucheFromOdometry / SPEED_FACTOR;
                robotState.vitesseGaucheSortieCorrecteur = CorrecteurVitesseGauche(error);

                //Limitation du range de consignes à + ou - 100 %
                robotState.vitesseGaucheSortieCorrecteur = Min(robotState.vitesseGaucheSortieCorrecteur, 100);
                robotState.vitesseGaucheSortieCorrecteur = Max(robotState.vitesseGaucheSortieCorrecteur, -100);

                //Détection des blocage roue en observant la valeur de l'erreurdu correcteur
                if (robotState.vitesseGaucheSortieCorrecteur > 70)
                {
                    //On détecte une erreur importante sur les correcteurs qui indique que 
                    //on est en blocage de roues
                    if ((timestamp - timestampSansCollisionGauche) > 500) //0.5 seconde
                        sysEvents.CollisionMoteurGaucheEvent = COLLISION_AVANT;
                }
                else if (robotState.vitesseGaucheSortieCorrecteur<-70)
                {
                    if ((timestamp - timestampSansCollisionGauche) > 500) //0.5 seconde                    
                        sysEvents.CollisionMoteurGaucheEvent = COLLISION_ARRIERE;
                }
                else
                    timestampSansCollisionGauche = timestamp;

                //Mise à jour des consignes des hacheurs
                if (robotState.vitesseGaucheSortieCorrecteur > 0)
                {
                    PWM1CON1bits.PEN2H = 0;
                    MOTOR_GAUCHE_IN1 = 1;
                    PWM1CON1bits.PEN2L = 1;
                }
                else
                {
                    PWM1CON1bits.PEN2L = 0;
                    MOTOR_GAUCHE_IN2 = 1;
                    PWM1CON1bits.PEN2H = 1;
                }
                P1DC2 = (unsigned int)(100 - Abs(robotState.vitesseGaucheSortieCorrecteur)*20);

                break;
            case MOTEUR_3:
                //Pas de QEI sur le moteur 3
                robotState.vitesseMoteur3SortieCorrecteur = robotState.vitesseMoteur3Consigne;

                //Limitation du range de consignes à + ou - 100 %
                robotState.vitesseMoteur3SortieCorrecteur = Min(robotState.vitesseMoteur3SortieCorrecteur, 100);
                robotState.vitesseMoteur3SortieCorrecteur = Max(robotState.vitesseMoteur3SortieCorrecteur, -100);

                //Mise à jour des consignes du moteur 3
                if (robotState.vitesseMoteur3SortieCorrecteur > 0)
                {
                    PWM1CON1bits.PEN3H = 0;
                    MOTOR_3_IN1 = 1;
                    PWM1CON1bits.PEN3L = 1;
                }
                else
                {
                    PWM1CON1bits.PEN3L = 0;
                    MOTOR_3_IN2 = 1;
                    PWM1CON1bits.PEN3H = 1;
                }
                P1DC3 = (unsigned int)(100 - Abs(robotState.vitesseMoteur3SortieCorrecteur)*20);

                break;
            case MOTEUR_4:
                //Pas de QEI sur le moteur 4
                robotState.vitesseMoteur4SortieCorrecteur = robotState.vitesseMoteur4Consigne;

                //Limitation du range de consignes à + ou - 100 %
                robotState.vitesseMoteur4SortieCorrecteur = Min(robotState.vitesseMoteur4SortieCorrecteur, 100);
                robotState.vitesseMoteur4SortieCorrecteur = Max(robotState.vitesseMoteur4SortieCorrecteur, -100);

                //TODO : Mise à jour des consignes des hacheurs du moteur 4                
                break;
        }
    }

    sysEvents.AsservissementVitesseEvent = 1;
}

void SendTelemetrieAsservissementVitesse()
{
    unsigned char payload[16];
    int pos = 0;
    payload[pos++] = (unsigned char) ((timestamp) >> 24);
    payload[pos++] = (unsigned char) ((timestamp) >> 16);
    payload[pos++] = (unsigned char) ((timestamp) >> 8);
    payload[pos++] = (unsigned char) ((timestamp) >> 0);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseDroitConsigne)) >> 8); //Droit - QEI1
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseDroitConsigne)) >> 0);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseDroitFromOdometry / SPEED_FACTOR)) >> 8);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseDroitFromOdometry / SPEED_FACTOR)) >> 0);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseDroitSortieCorrecteur)) >> 8);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseDroitSortieCorrecteur)) >> 0);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseGaucheConsigne)) >> 8); //1 -> Gauche - QEI2
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseGaucheConsigne)) >> 0);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseGaucheFromOdometry / SPEED_FACTOR)) >> 8);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseGaucheFromOdometry / SPEED_FACTOR)) >> 0);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseGaucheSortieCorrecteur)) >> 8);
    payload[pos++] = (unsigned char) (((int) (robotState.vitesseGaucheSortieCorrecteur)) >> 0);

    Uart1EncodeAndSendMessage(ASSERVISSEMENT_VITESSE_TELEMETRIE, pos, payload);
}