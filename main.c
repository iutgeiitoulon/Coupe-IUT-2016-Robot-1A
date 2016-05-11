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

float valeurTelemetreGauche, valeurTelemetreCentre, valeurTelemetreDroit;
volatile unsigned char stateRobot;
unsigned char nextStateRobot, robotAutoControlActivated;
unsigned char consigneVitesseMoteurDroit, consigneVitesseMoteurGauche;
unsigned char valeursTelemetres[3];
unsigned char valeursMoteurs[2];
unsigned char EssaiMangeChateau = 0;
char autoControlActivated = 0;
int AxeY = 900;

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

    __delay32(32000000); //Nécessaire pour attendre le démarrage du LIDAR

    //    unsigned char startlidar[] = {0xA5, 0x20};
    //    SendMessage2(startlidar, 2);

    InitSeuilDetectionSRF08();
    InitTelemetres();
    SetConsigneServoBrasGauche(100);
    SetConsigneServoBrasDroit(100);
    SetConsigneServoPinceGauche(100);
    SetConsigneServoPinceDroit(100);
    SetConsigneServo5(100);
    PWMSetSpeedConsigne(0, 3);

#ifdef PETIT_ROBOT
    //Valeurs mesurées petit robot :  Ku=3 - Tu=0.1
    SetUpPidAsservissementVitesseMoteur(3, 0.05);
    //Valeurs mesurées oscillations asserv angulaire :  Ku=100 - Tu=0.6   
    double KpAngulaire = 0.0012;
    double KpLineaire = 0.00009;
    SetUpPAsservissementAngulaire(0.004); // 0.012
    SetUpPAsservissementLineaire(0.003); // 0.016
    robotState.GainVitesseLineaire = 0.03 / KpLineaire; //10
    robotState.VitesseLimiteLineaire = 6000; //1000
    robotState.GainVitesseRotation = 0.12 / KpAngulaire; //10
    robotState.VitesseLimiteRotation = 3500; //600
    //SetUpPiAsservissementAngulaire(0.1, 0.4); // réglage du gain 
#elif defined(GROS_ROBOT)    
    //Valeurs mesurées gros robot :  Ku=3 - Tu=0.1
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
#endif

    LED_BLANCHE = 0;
    LED_BLEUE = 0;
    LED_ORANGE = 0;

    int counterQeiData = 0;
    int counterPositionData = 0;
    int counterAsservissementAngulaireData = 0;
    int counterAsservissementVitesseData = 0;





    //Boucle principale
    while (1)
    {
        //        LED_BLANCHE = ENTREE_JACK;
        //        if (ENTREE_SELECTEUR_COULEUR == VIOLET)
        //            LED_ORANGE = 1;
        //        else
        //            LED_ORANGE = 0;

        //On process un éventuel crash I2C
        //        if(IsI2CCrashed()==1)
        //        {            
        //            InitI2C1();
        //            __delay32(4000000);
        //            InitTelemetres();
        //            ResetI2CCrash();
        //        }     

        if (ENTREE_JACK == 0)
        {
            //Dès lors que le jack est mis on revient à l'étape d'attente initiale
            SetConsigneServoBras(0);
            SetConsigneServoBrasInactif(0);
            SetConsigneServoPince(0);
            SetConsigneServoPinceInactif(0);            
            PWMSetSpeedConsigne(0, MOTEUR_ROULEAU);
            stateRobot = STATE_ATTENTE;
        }

        if (timestamp > DUREE_MATCH)
        {
            stateRobot = STATE_FIN_MATCH;
        }

        if (sysEvents.QeiDataEvent)
        {
            sysEvents.QeiDataEvent = 0;
            if (counterQeiData++ >= 25)
            {
                SendQeiData();
                counterQeiData = 0;
            }
        }

        if (sysEvents.PositionDataEvent)
        {
            sysEvents.PositionDataEvent = 0;
            if (counterPositionData++ >= 25)
            {
                SendPositionData();
                counterPositionData = 0;
            }
        }

        if (sysEvents.AsservissementVitesseEvent)
        {
            sysEvents.AsservissementVitesseEvent = 0;
            if (counterAsservissementVitesseData++ >= 25)
            {
                SendTelemetrieAsservissementVitesse();
                counterAsservissementVitesseData = 0;
            }
        }

        if (sysEvents.AsservissementPositionEvent)
        {
            sysEvents.AsservissementPositionEvent = 0;
            if (counterAsservissementAngulaireData++ >= 25)
            {
                SendTelemetrieAsservissementAngulaire();
                counterAsservissementAngulaireData = 0;
            }
        }

        if (sysEvents.UltrasonicRangeFinderEvent)
        {
            sysEvents.UltrasonicRangeFinderEvent = 0;
            StartNewUltrasonicMeasure();
            SendUltrasonicMeasure();
        }


        //TODO à rétablir absolument !!!!
        //                if (sysEvents.UltrasonicObjectDetectionEvent)
        //                {
        //                    sysEvents.UltrasonicObjectDetectionEvent = 0;
        //                    if ((PWMSpeedConsigne[MOTEUR_GAUCHE] > 0) || (PWMSpeedConsigne[MOTEUR_DROIT] > 0))
        //                        SetRobotVitesseAsservie(0, 0);
        //                    robotState.AligneAvantVersDirectionMouvement = 0;
        //                }
        //
        //        if (sysEvents.CollisionMoteurGaucheEvent != 0)
        //        {
        //            ResetCorrecteurVitesseGauche();
        //            SetRobotVitesseLibre(0, 0);
        //            robotState.AligneAvantVersDirectionMouvement = 0;
        //            //            if (sysEvents.CollisionMoteurGaucheEvent == COLLISION_AVANT)
        //            //                SetRobotVitesseAsservie(0, 0);
        //            //            else if (sysEvents.CollisionMoteurGaucheEvent == COLLISION_ARRIERE)
        //            //                SetRobotVitesseAsservie(0, 0);
        //            sysEvents.CollisionMoteurGaucheEvent = 0b00;
        //        }
        //
        //        if (sysEvents.CollisionMoteurDroitEvent != 0)
        //        {
        //            ResetCorrecteurVitesseDroit();
        //            SetRobotVitesseLibre(0, 0);
        //            robotState.AligneAvantVersDirectionMouvement = 0;
        //            //            if (sysEvents.CollisionMoteurDroitEvent == COLLISION_AVANT)
        //            //            {
        //            //                SetRobotVitesseAsservie(0, 0);
        //            //            }
        //            //            else if (sysEvents.CollisionMoteurDroitEvent == COLLISION_ARRIERE)
        //            //            {
        //            //                SetRobotVitesseAsservie(0, 0);
        //            //            }
        //            sysEvents.CollisionMoteurDroitEvent = 0b00;
        //        }

        //        if (flagLidarFrameReady) {
        //            flagLidarFrameReady = 0;
        //            UartEncodeAndSendMessage(LIDAR_PROTOCOL, ANGLE_MAX - ANGLE_MIN, DistanceTableToSend);
        //            valeurTelemetreGauche = DistanceTableToSend[70];
        //            valeurTelemetreCentre = DistanceTableToSend[100];
        //            valeurTelemetreDroit = DistanceTableToSend[130];
        //            valeursTelemetres[0] = (unsigned char) valeurTelemetreGauche;
        //            valeursTelemetres[1] = (unsigned char) valeurTelemetreCentre;
        //            valeursTelemetres[2] = (unsigned char) valeurTelemetreDroit;
        //            UartEncodeAndSendMessage(TELEMETRE_PROTOCOL, 3, valeursTelemetres);
        //        }

        while (CB_RX1_IsDataAvailable())
            Uart1DecodeMessage(CB_RX1_Get());

        while (CB_RX2_IsDataAvailable())
            Uart2DecodeMessage(CB_RX2_Get());
    }

}

volatile int counterAvalage = 0;


#ifdef GROS_ROBOT
void SystemStateMachine(void)
{
    switch (stateRobot)
    {
        case STATE_ATTENTE:
            timestamp = 0;
            robotState.taskEnCours = NO_TASK;
            //SetRobotVitesseLibre(0, 0);
            SetRobotVitesseAsservie(0, 0);
            //PWMSetSpeedConsigne(VITESSE_ROULEAU_ARRET, MOTEUR_ROULEAU);
            PWMSetSpeedConsigne(0, MOTEUR_ROULEAU);

            //Initialisation des tasks !!!!!!!!!!!!!!!!!!!!!!
            TaskVidageChateauInit();
            TaskFermeturePortesInit();
            TaskAvalageChateauInit();
            TaskPechePoissonInit();
            SendState();
            stateRobot = STATE_ATTENTE_EN_COURS;
            break;

        case STATE_ATTENTE_EN_COURS:
            if (ENTREE_JACK == 1)
                stateRobot = STATE_MATCH;
            SetConsigneServoPinceDroit(90);

            robotState.matchState = MATCH_INIT;
            break;

        case STATE_MATCH:
            SendState();
            stateRobot = STATE_MATCH_EN_COURS;
            break;

        case STATE_MATCH_EN_COURS:
            switch (robotState.matchState)
            {
                case MATCH_INIT:
                    QEISetPosition(800, 60, 0);
                    SendState();
                    robotState.matchState = MATCH_INIT_EN_COURS;
                    break;

                case MATCH_INIT_EN_COURS:
                    robotState.matchState = MATCH_FERMETURE_PORTES;
                    break;
                    
                    
                case MATCH_FERMETURE_PORTES:
                    TaskFermeturePortesStart();
                    robotState.matchState = MATCH_FERMETURE_PORTES_EN_COURS;
                    break;

                case MATCH_FERMETURE_PORTES_EN_COURS:
                    if (TaskFermeturePortesIsFinished())
                    {
                        robotState.matchState = MATCH_AVALAGE_CHATEAU;
                    }
                    break;

                case MATCH_AVALAGE_CHATEAU:
                    if (counterAvalage % 5 == 0)
                        AxeY = 1650;
                    else if (counterAvalage % 5 == 1)
                        AxeY = 1300;
                    else if (counterAvalage % 5 == 2)
                        AxeY = 1580;                    
                    else if (counterAvalage % 5 == 3)
                        AxeY = 1500;

                    robotState.matchState = MATCH_AVALAGE_CHATEAU_EN_COURS;
                    TaskAvalageChateauStart(AxeY); //-counterAvalage*150);                        
                    counterAvalage++;
                    break;

                case MATCH_AVALAGE_CHATEAU_EN_COURS:
                    if (TaskAvalageChateauIsFinished())
                    {
                        robotState.matchState = MATCH_VIDAGE_CHATEAU;
                    }
                    break;

                case MATCH_VIDAGE_CHATEAU:
                    TaskVidageChateauStart();
                    robotState.matchState = MATCH_VIDAGE_CHATEAU_EN_COURS;
                    break;

                case MATCH_VIDAGE_CHATEAU_EN_COURS:
                    if (TaskVidageChateauIsFinished())
                    {
                        LED_BLANCHE = 1;
                        LED_BLEUE = 1;
                        LED_ORANGE = 1;
                        if (EssaiMangeChateau)
                            robotState.matchState = MATCH_FERMETURE_PORTES;
                        else
                            robotState.matchState = MATCH_AVALAGE_CHATEAU;
                    }
                    break;



                case MATCH_NO_TASK:
                    break;

                default:
                    robotState.matchState = MATCH_AVALAGE_CHATEAU;
                    break;
            }
            break;

        case STATE_FIN_MATCH:
            robotState.taskEnCours = NO_TASK;
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            SendState();
            stateRobot = STATE_FIN_MATCH_EN_COURS;
            break;

        case STATE_FIN_MATCH_EN_COURS:
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

#endif

#ifdef PETIT_ROBOT
void SystemStateMachine(void)
{
    switch (stateRobot)
    {
        case STATE_ATTENTE:
            timestamp = 0;
            robotState.taskEnCours = NO_TASK;
            //SetRobotVitesseLibre(0, 0);
            SetRobotVitesseAsservie(0, 0);
            //PWMSetSpeedConsigne(VITESSE_ROULEAU_ARRET, MOTEUR_ROULEAU);
            PWMSetSpeedConsigne(0, MOTEUR_ROULEAU);

            //Initialisation des tasks !!!!!!!!!!!!!!!!!!!!!!
            TaskVidageChateauInit();
            TaskFermeturePortesInit();
            TaskAvalageChateauInit();
            TaskPechePoissonInit();
            SendState();
            stateRobot = STATE_ATTENTE_EN_COURS;
            break;

        case STATE_ATTENTE_EN_COURS:
            if (ENTREE_JACK == 1)
                stateRobot = STATE_MATCH;
            SetConsigneServoPinceDroit(0);
            SetConsigneServoPinceGauche(0);
            SetConsigneServoBrasDroit(0);
            SetConsigneServoBrasGauche(0);

            robotState.matchState = MATCH_INIT;
            break;

        case STATE_MATCH:
            SendState();
            stateRobot = STATE_MATCH_EN_COURS;
            break;

        case STATE_MATCH_EN_COURS:
            switch (robotState.matchState)
            {
                case MATCH_INIT:
                    QEISetPosition(1045, 200, -90);
                    SendState();
                    robotState.matchState = MATCH_INIT_EN_COURS;
                    break;

                case MATCH_INIT_EN_COURS:
                    robotState.matchState = MATCH_PECHE_POISSON;
                    break;

                case MATCH_PECHE_POISSON:
                    TaskPechePoissonStart();
                    robotState.matchState = MATCH_PECHE_POISSON_EN_COURS;
                    break;

                case MATCH_PECHE_POISSON_EN_COURS:
                    if (TaskPechePoissonIsFinished())
                    {
                        robotState.matchState = MATCH_NO_TASK;
                        counterAvalage = 0;
                    }
                    break;

                case MATCH_NO_TASK:
                    break;

                default:
                    robotState.matchState = MATCH_AVALAGE_CHATEAU;
                    break;
            }
            break;

        case STATE_FIN_MATCH:
            robotState.taskEnCours = NO_TASK;
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            SendState();
            stateRobot = STATE_FIN_MATCH_EN_COURS;
            break;

        case STATE_FIN_MATCH_EN_COURS:
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

#endif

void SetNextRobotStateInAutomaticMode()
{
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    if (valeurTelemetreDroit < 30 && valeurTelemetreCentre > 30 && valeurTelemetreGauche > 30)
        positionObstacle = OBSTACLE_A_DROITE;

    else if (valeurTelemetreDroit > 30 && valeurTelemetreCentre > 30 && valeurTelemetreGauche < 30)
        positionObstacle = OBSTACLE_A_GAUCHE;

    else if (valeurTelemetreCentre < 30)
        positionObstacle = OBSTACLE_EN_FACE;

    else if (valeurTelemetreDroit > 30 && valeurTelemetreCentre > 30 && valeurTelemetreGauche > 30)
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
