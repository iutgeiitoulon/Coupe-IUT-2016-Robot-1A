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

int main(void) {
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

    robotState.acceleration = 0.4;
    robotState.rampeAccelerationActive = 1;

    LED_BLANCHE = 0;
    LED_BLEUE = 0;
    LED_ORANGE = 0;

    int counterQeiData = 0;
    int counterPositionData = 0;
    int counterAsservissementAngulaireData = 0;
    int counterAsservissementVitesseData = 0;

    //Boucle principale
    while (1) {
        
        LED_BLEUE = 0;
       
        //        if (ENTREE_SELECTEUR_COULEUR == VIOLET)
        //     LED_ORANGE = 1;
        //  else
        //    LED_ORANGE = 0;

        //On process un éventuel crash I2C
        if (IsI2CCrashed() == 1) {
            InitI2C1();
            __delay32(4000000);
            InitTelemetres();
            ResetI2CCrash();
        }

        if ((!ENTREE_Color_Capture_2) && (!ENTREE_Color_Capture_1)) {
            sysEvents.ColorCapture = 1;
        }



        if (BUMPERS == 1) {
            sysEvents.BumpersDetection = 1;
        }

        if (ENTREE_JACK == 0) {
            //Dès lors que le jack est mis on revient à l'étape d'attente initiale
            SetRobotVitesseLibre(0, 0);
        }

        SystemStateMachine();

        if (sysEvents.QeiDataEvent) {
            sysEvents.QeiDataEvent = 0;
            if (counterQeiData++ >= 25) {
                SendQeiData();
                counterQeiData = 0;
            }
        }

        if (sysEvents.PositionDataEvent) {
            sysEvents.PositionDataEvent = 0;
            if (counterPositionData++ >= 25) {
                SendPositionData();
                counterPositionData = 0;
            }
        }

        if (sysEvents.AsservissementVitesseEvent) {
            sysEvents.AsservissementVitesseEvent = 0;
            if (counterAsservissementVitesseData++ >= 25) {
                SendTelemetrieAsservissementVitesse();
                counterAsservissementVitesseData = 0;
            }
        }

        if (sysEvents.AsservissementPositionEvent) {
            sysEvents.AsservissementPositionEvent = 0;
            if (counterAsservissementAngulaireData++ >= 25) {
                SendTelemetrieAsservissementAngulaire();
                counterAsservissementAngulaireData = 0;
            }
        }

        if (sysEvents.UltrasonicRangeFinderEvent) {
            sysEvents.UltrasonicRangeFinderEvent = 0;
            StartNewUltrasonicMeasure();
            SendUltrasonicMeasure();
        }

        if (sysEvents.BumpersDetection == 1) {
            stateRobot = RECULE_OBSTACLE;
            LED_BLANCHE = 1;
            sysEvents.BumpersDetection = 0;
        }
        else
             LED_BLANCHE = 0;


        if (sysEvents.ColorCapture) {
            LED_ORANGE = 1;
            sysEvents.ColorCapture = 0;
        }
        else
            LED_ORANGE = 0;
        //        if (sysEvents.UltrasonicObjectDetectionEvent)
        //        {
        //            sysEvents.UltrasonicObjectDetectionEvent = 0;
        //            if ((robotState.vitesseDroitConsigne > 0) || (robotState.vitesseGaucheConsigne > 0))
        //                SetRobotVitesseAsservie(0, 0);
        //            robotState.AligneAvantVersDirectionMouvement = 0;
        //        }

        //        if (sysEvents.CollisionMoteurGaucheEvent != 0)
        //        {
        //            ResetCorrecteurVitesseGauche();
        //            SetRobotVitesseLibre(0, 0);
        //            robotState.AligneAvantVersDirectionMouvement = 0;
        //            if (sysEvents.CollisionMoteurGaucheEvent == COLLISION_AVANT)
        //                SetRobotVitesseAsservie(0, 0);
        //            else if (sysEvents.CollisionMoteurGaucheEvent == COLLISION_ARRIERE)
        //                SetRobotVitesseAsservie(0, 0);
        //            sysEvents.CollisionMoteurGaucheEvent = 0b00;


        //        if (sysEvents.CollisionMoteurDroitEvent != 0)
        //        {
        //            ResetCorrecteurVitesseDroit();
        //            SetRobotVitesseLibre(0, 0);
        //            robotState.AligneAvantVersDirectionMouvement = 0;
        //            if (sysEvents.CollisionMoteurDroitEvent == COLLISION_AVANT)
        //            {
        //                SetRobotVitesseAsservie(0, 0);
        //            }
        //            else if (sysEvents.CollisionMoteurDroitEvent == COLLISION_ARRIERE)
        //            {
        //                SetRobotVitesseAsservie(0, 0);
        //            }
        //            sysEvents.CollisionMoteurDroitEvent = 0b00;
        //        }




        while (CB_RX1_IsDataAvailable())
            Uart1DecodeMessage(CB_RX1_Get());

        while (CB_RX2_IsDataAvailable())
            Uart2DecodeMessage(CB_RX2_Get());
    }
}

long timestampStateStart;

void SystemStateMachine(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            SetRobotVitesseAsservie(0, 0); //Gauche puis droite
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;
        case STATE_AVANCE:
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
        {
            //Exceptionnellement on recharge à chaque passage les vitesses moteurs pour les modifier en fonction des distances mesurées
            int angleOptimal = ModuloAngleDegre(robotState.angleDegreeFromBalise);
            signed char vitesseAvance = Max(10, Min(70, robotState.distanceTelemetreCentre * 3 - 10));
            SetRobotVitesseAsservie(vitesseAvance - (angleOptimal * 0.5), vitesseAvance + (angleOptimal * 0.5)); //Gauche puis droite coefficient à déterminer
            SetNextRobotStateInAutomaticMode();
            break;
        }
        case STATE_TOURNE_GAUCHE:
            SetRobotVitesseAsservie(0, 30); //Gauche puis droite
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            SetRobotVitesseAsservie(30, 0); //Gauche puis droite
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            SetRobotVitesseAsservie(-30, 30); //Gauche puis droite
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case RECULE_OBSTACLE:
            SetRobotVitesseAsservie(-30, -30);
            stateRobot = RECULE_OBSTACLE_EN_COURS;
            timestampStateStart = timestamp;
            break;
        case RECULE_OBSTACLE_EN_COURS:
            if ((timestamp - timestampStateStart) > 500)
                stateRobot = STATE_AVANCE;
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    if (robotState.distanceTelemetreDroit < 30 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 30)
        positionObstacle = OBSTACLE_A_DROITE;
    else if (robotState.distanceTelemetreDroit > 30 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche < 30)
        positionObstacle = OBSTACLE_A_GAUCHE;
    else if (robotState.distanceTelemetreCentre < 30)
        positionObstacle = OBSTACLE_EN_FACE;
    else if (robotState.distanceTelemetreDroit > 30 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 30)
        positionObstacle = PAS_D_OBSTACLE;

    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    else if (positionObstacle == OBSTACLE_EN_FACE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;

    if (nextStateRobot != stateRobot - 1)
        stateRobot = nextStateRobot;

}

void SendState() {
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

void SetRobotAutoControlState(char state) {
    autoControlActivated = state ? 1 : 0;
    if (autoControlActivated) {
        SetRobotState(STATE_ATTENTE);
        timestamp = 0;
    }
}

void SetRobotState(char state) {
    stateRobot = state;
}
