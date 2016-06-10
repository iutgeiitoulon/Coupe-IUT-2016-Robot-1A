/*
 * File:   main.c
 * Author: E105-POSTE2
 *
 * Created on 7 septembre 2015, 11:53
 */


#include <stdio.h>
#include <stdlib.h>
#include "p33Fxxxx.h"
#include <libpic30.h>
#include "pragma_config.h"
#include "oscillator.h"
#include "main.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "UART.h"
#include "CB_TX1.h"
#include "CB_RX1.h"
#include "UART_Protocol.h"
#include "QEI.h"
#include "AsservissementVitesse.h"
#include "Lidar.h"
#include "ustv_i2c.h"
#include "Telemetre_UltraSons.h"
#include "CB_TX2.h"
#include "CB_RX2.h"
#include "AsservissementPolaire.h"
#include "events.h"
#include "Servo.h"
#include "interrupt.h"
#include "RobotManagement.h"
#include "robot.h"
#include "Toolbox.h"


#include "TaskVidageChateau.h"
#include "TaskFermeturePortes.h"
#include "TaskAvalageChateau.h"
#include "TaskPechePoisson.h"

//Configuration des paramètres du chip

// Configuration du quartz sans PLL
//
//_FOSCSEL(FNOSC_PRI & IESO_OFF)
//// Clock switching disables
//_FOSC(FCKSM_CSECMD & OSCIOFNC_ON)
//// Watchdog disabled
//_FWDT(FWDTEN_OFF)
//// Disable Code Protection
//_FGS(GWRP_OFF & GSS_OFF & GCP_OFF)
//// JTAG disabled
//_FICD(JTAGEN_OFF)

volatile unsigned char stateRobot;
unsigned char nextStateRobot, robotAutoControlActivated;
unsigned char consigneVitesseMoteurDroit, consigneVitesseMoteurGauche;
unsigned char valeursTelemetres[3];
unsigned char valeursMoteurs[2];
char autoControlActivated = 0;

int main(void)
{

    // Configuration des entrées sorties
    InitOscillator();
    InitIO();
    InitTimer1();
    InitTimer2();
    InitTimer3();
    InitTimer4();
    InitPWM();
    InitUART1();
    InitUART2();
    InitQEI1();
    InitQEI2();
    InitI2C1();
    SetInterruptPriority();

    InitSeuilDetectionSRF08();
    InitTelemetres();

    //A DEFINIR!!
    stateRobot = STATE_ATTENTE;
    //Valeurs mesurées robot :  Ku=? - Tu=?
    SetUpPidAsservissementVitesseMoteur(3, 0.05);
    //Valeurs mesurées oscillations asserv angulaire :  Ku=100 - Tu=0.6    
    robotState.GainVitesseLineaire = 10; //10
    robotState.VitesseLimiteLineaire = 2000; //1000
    robotState.GainVitesseRotation = 16; //10
    robotState.VitesseLimiteRotation = 1400; //600
    //SetUpPiAsservissementAngulaire(0.1, 0.4); // réglage du gain   
    SetUpPAsservissementAngulaire(0.008); // 0.012
    //SetUpPiAsservissementLineaire(0.1, 0.4);
    SetUpPAsservissementLineaire(0.010); // 0.016
    robotState.distanceCentreFinMatch = 45;
    robotState.DistanceDetectionFace = 50;
    robotState.DistanceDetectionCote = 30;
    robotState.acceleration = 0.4;


    robotState.rampeAccelerationActive = 1; // a voir si à remettre ! peut etre utile pour les virage!

    LED_BLANCHE = 0;
    LED_BLEUE = 0;
    LED_ORANGE = 0;

    //Boucle principale
    while (1)
    {
        //On process un éventuel crash I2C
        if (IsI2CCrashed() == 1)
        {
            InitI2C1();
            __delay32(4000000);
            InitTelemetres();
            ResetI2CCrash();
        }
        if (sysEvents.BaliseEvent == 1)
        {
            EnvoiAngleInterface();
            sysEvents.BaliseEvent = 0;
        }

        if ((ENTREE_CAPTEUR_COULEUR_1 == 0) && (!ENTREE_CAPTEUR_COULEUR_2 == 0) && (timestamp > 4000))
        {
            LED_BLANCHE = 1;
            sysEvents.ColorCapture = 1;
        }
        else
            LED_BLANCHE = 0;



        if (BUMPERS == 1)
        {
            sysEvents.BumpersDetection = 1;
        }

        if (sysEvents.ColorCapture == 1)
        {
            //LED_BLANCHE = 1;
            stateRobot = COLOR_CONFIRM;
            sysEvents.ColorCapture = 0;
        }
        else
        {
            //LED_BLANCHE = 0;
        }

        if (ENTREE_JACK == 0)
        {
            //Dès lors que le jack est mis on revient à l'étape d'attente initiale
            LED_BLEUE = 1;
            stateRobot = STATE_ATTENTE;
            timestamp = 0;
            SetConsigneServoCreveBallon(0);
        }
        else
        {
            LED_BLEUE = 0;
        }

        if (timestamp > 9000 && robotState.distanceModifie == 0) // reduit la distance max du capteur central afin de rentrer dans la zone blanche
        {
            robotState.distanceModifie = 1;
            robotState.distanceTelemetreCentre = robotState.distanceCentreFinMatch;
            robotState.DistanceDetectionCote = 25;
        }
        //
        //        //        if (sysEvents.QeiDataEvent)
        //        //        {
        //        //            sysEvents.QeiDataEvent = 0;
        //        //            if (counterQeiData++ >= 25)
        //        //            {
        //        //                SendQeiData();
        //        //                counterQeiData = 0;
        //        //            }
        //        //        }
        //        //
        //        //        if (sysEvents.PositionDataEvent)
        //        //        {
        //        //            sysEvents.PositionDataEvent = 0;
        //        //            if (counterPositionData++ >= 25)
        //        //            {
        //        //                SendPositionData();
        //        //                counterPositionData = 0;
        //        //            }
        //        //        }
        //        //
        //        //        if (sysEvents.AsservissementVitesseEvent)
        //        //        {
        //        //            sysEvents.AsservissementVitesseEvent = 0;
        //        //            if (counterAsservissementVitesseData++ >= 25)
        //        //            {
        //        //                SendTelemetrieAsservissementVitesse();
        //        //                counterAsservissementVitesseData = 0;
        //        //            }
        //        //        }
        //        //
        //        //        if (sysEvents.AsservissementPositionEvent)
        //        //        {
        //        //            sysEvents.AsservissementPositionEvent = 0;
        //        //            if (counterAsservissementAngulaireData++ >= 25)
        //        //            {
        //        //                SendTelemetrieAsservissementAngulaire();
        //        //                counterAsservissementAngulaireData = 0;
        //        //            }
        //        //        }
        //
        if (sysEvents.UltrasonicRangeFinderEvent)
        {
            sysEvents.UltrasonicRangeFinderEvent = 0;
            StartNewUltrasonicMeasure();
            EnvoiAngleInterface();
        }

        if (sysEvents.BumpersDetection == 1)
        {
            stateRobot = RECULE_OBSTACLE;
            sysEvents.BumpersDetection = 0;
        }

        //
        //
        //
        //        //        if (sysEvents.UltrasonicObjectDetectionEvent)
        //        //        {
        //        //            sysEvents.UltrasonicObjectDetectionEvent = 0;
        //        //            if ((robotState.vitesseDroitConsigne > 0) || (robotState.vitesseGaucheConsigne > 0))
        //        //                SetRobotVitesseAsservie(0, 0);
        //        //            robotState.AligneAvantVersDirectionMouvement = 0;
        //        //        }
        //
        //        //        if (sysEvents.CollisionMoteurGaucheEvent != 0)
        //        //        {
        //        //            ResetCorrecteurVitesseGauche();
        //        //            SetRobotVitesseLibre(0, 0);
        //        //            robotState.AligneAvantVersDirectionMouvement = 0;
        //        //            if (sysEvents.CollisionMoteurGaucheEvent == COLLISION_AVANT)
        //        //                SetRobotVitesseAsservie(0, 0);
        //        //            else if (sysEvents.CollisionMoteurGaucheEvent == COLLISION_ARRIERE)
        //        //                SetRobotVitesseAsservie(0, 0);
        //        //            sysEvents.CollisionMoteurGaucheEvent = 0b00;
        //
        //
        //        //        if (sysEvents.CollisionMoteurDroitEvent != 0)
        //        //        {
        //        //            ResetCorrecteurVitesseDroit();
        //        //            SetRobotVitesseLibre(0, 0);
        //        //            robotState.AligneAvantVersDirectionMouvement = 0;
        //        //            if (sysEvents.CollisionMoteurDroitEvent == COLLISION_AVANT)
        //        //            {
        //        //                SetRobotVitesseAsservie(0, 0);
        //        //            }
        //        //            else if (sysEvents.CollisionMoteurDroitEvent == COLLISION_ARRIERE)
        //        //            {
        //        //                SetRobotVitesseAsservie(0, 0);
        //        //            }
        //        //            sysEvents.CollisionMoteurDroitEvent = 0b00;
        //        //        }
        //
        //
        SystemStateMachine();
        //
        ////        while (CB_RX1_IsDataAvailable())
        // //           Uart1DecodeMessage(CB_RX1_Get());

        while (CB_RX2_IsDataAvailable())
            Uart2DecodeMessage(CB_RX2_Get());
    }
}

long timestampStateStart = 0;
long timestampColor = 0;

void SystemStateMachine(void)
{
    switch (stateRobot)
    {
        case STATE_ATTENTE:
            SetRobotVitesseLibre(0, 0); //Gauche puis droite
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (ENTREE_JACK == 1)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
        {
            //Exceptionnellement on recharge à chaque passage les vitesses moteurs pour les modifier en fonction des distances mesurées
            int angleOptimal = ModuloAngleDegre(robotState.AngleDegreFromBalise);

            if (robotState.distanceTelemetreCentre < robotState.distanceTelemetreCentrePrecedent)
            {
                robotState.distanceTelemetreCentre *= Min((0.02 * robotState.distanceTelemetreCentre), 1); // rapproche les objets afin de tourner avant 
            }
            signed char vitesseAvantConsigne = Max(10, Min(70, robotState.distanceTelemetreCentre)); // minimum 10 de vitesse et 
            //            if (timestamp < 2000)
            //                Min(50, vitesseAvantConsigne);

            robotState.vitesseGaucheConsigne = vitesseAvantConsigne - (angleOptimal * 0.20);
            robotState.vitesseDroitConsigne = vitesseAvantConsigne + (angleOptimal * 0.20);
            SetRobotVitesseLibre(robotState.vitesseGaucheConsigne, robotState.vitesseDroitConsigne); //Gauche puis droite coefficient à déterminer
            robotState.distanceTelemetreCentrePrecedent = robotState.distanceTelemetreCentre;
            SetNextRobotStateInAutomaticMode();
            break;
        }
        case STATE_TOURNE_GAUCHE:
            SetRobotVitesseLibre(0, 20); //Gauche puis droite
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            SetRobotVitesseLibre(20, 0); //Gauche puis droite
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            SetRobotVitesseLibre(-40, 40); //Gauche puis droite
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            SetRobotVitesseLibre(40, -40); //Gauche puis droite
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case RECULE_OBSTACLE:
            if (robotState.distanceTelemetreDroit > robotState.distanceTelemetreGauche)
                SetRobotVitesseLibre(-35, -40);
            else
                SetRobotVitesseLibre(-40, -35);
            stateRobot = RECULE_OBSTACLE_EN_COURS;
            timestampStateStart = timestamp;
            break;
        case RECULE_OBSTACLE_EN_COURS:
            if ((timestamp - timestampStateStart) > 500)
                stateRobot = STATE_AVANCE;
            break;
        case COLOR_CONFIRM:
            timestampColor = timestamp;
            stateRobot = COLOR_CONFIRM_EN_COURS;
            break;
        case COLOR_CONFIRM_EN_COURS:
            if ((timestamp - timestampColor) > 5) // sur du blanc pendant 5 ms
                stateRobot = STATE_FIN_MATCH;
            break;
        case STATE_FIN_MATCH:
            timestampStateStart = timestamp;
            SetRobotVitesseLibre(0, 0); //Gauche puis droite
            SetConsigneServoCreveBallon(1);
            stateRobot = STATE_FIN_MATCH_EN_COURS;
            break;
        case STATE_FIN_MATCH_EN_COURS:
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

void SetNextRobotStateInAutomaticMode()
{
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    if ((robotState.distanceTelemetreDroit < robotState.DistanceDetectionCote) && (robotState.distanceTelemetreCentre > robotState.DistanceDetectionFace) && (robotState.distanceTelemetreGauche > robotState.DistanceDetectionCote))
        positionObstacle = OBSTACLE_A_DROITE;
    else if ((robotState.distanceTelemetreDroit > robotState.DistanceDetectionCote) && (robotState.distanceTelemetreCentre > robotState.DistanceDetectionFace) && (robotState.distanceTelemetreGauche < robotState.DistanceDetectionCote))
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (robotState.distanceTelemetreCentre < robotState.DistanceDetectionFace)
        positionObstacle = OBSTACLE_EN_FACE;
    else if ((robotState.distanceTelemetreDroit > robotState.DistanceDetectionCote) && (robotState.distanceTelemetreCentre > robotState.DistanceDetectionFace) && (robotState.distanceTelemetreGauche > robotState.DistanceDetectionCote))
        positionObstacle = PAS_D_OBSTACLE;

    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
    {
        if (robotState.distanceTelemetreDroit > robotState.distanceTelemetreGauche)
        {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        }
        else
        {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        }
    }

    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;

}

void SendState()
{
    unsigned char stateMessagePayload[8];
    unsigned char pos = 0;
    int i;

    for (i = 0; i < 4; i++)
        stateMessagePayload[pos++] = (char) (timestamp >> (8 * i));

    stateMessagePayload[pos++] = stateRobot;
    stateMessagePayload[pos++] = robotState.taskState;
    stateMessagePayload[pos++] = robotState.taskState;
    stateMessagePayload[pos++] = robotState.taskState;

    Uart1EncodeAndSendMessage(STATE_PROTOCOL, 5, stateMessagePayload);
}

void SetRobotAutoControlState(char state)
{
    autoControlActivated = state ? 1 : 0;
    if (autoControlActivated)
    {
        SetRobotState(STATE_ATTENTE);
        timestamp = 0;
    }
}

void SetRobotState(char state)
{
    stateRobot = state;
}
