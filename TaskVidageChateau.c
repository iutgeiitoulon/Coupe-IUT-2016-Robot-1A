#include "TaskVidageChateau.h"
#include "Task.h"
#include "RobotManagement.h"
#include "main.h"
#include "robot.h"
#include "IO.h"
#include "PWM.h"
#include "events.h"
#include "timer.h"
#include "Toolbox.h"

volatile TASK_BITS taskVidageChateau;
int allerRetourCounter = 0;

void TaskVidageChateauInit()
{
    taskVidageChateau.startingPosX = 380;
    taskVidageChateau.startingPosY = 1150;
}

void TaskVidageChateauStart()
{
    robotState.taskEnCours = TASK_VIDAGE_CHATEAU;
    robotState.taskState = T_VC_INIT;
    allerRetourCounter = 0;
}

unsigned char TaskVidageChateauIsFinished()
{
    if (robotState.taskState == T_VC_FIN_EN_COURS)
        return 1;
    else
        return 0;
}

void TaskVidageChateau(void)
{
    switch (robotState.taskState)
    {
        case T_VC_INIT:
            //On dit au robot d'aller à la position initiale
            allerRetourCounter=0;
            StartMove(T_VC_INIT_EN_COURS,380,taskVidageChateau.startingPosY,ALIGNED_WITH_MOVE);
            PWMSetSpeedConsigne(VITESSE_ROULEAU_AVALE, MOTEUR_ROULEAU);
            break;
        case T_VC_INIT_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_VC_AVANCE_0,4000);
                robotState.AligneAvantVersDirectionMouvement = 0;
            break;

        case T_VC_AVANCE_0:
            if (allerRetourCounter < 2)
            {
                allerRetourCounter++;
                SetRobotTargetAngle(90);
                StartTempo(T_VC_AVANCE_0_EN_COURS);
                PWMSetSpeedConsigne(VITESSE_ROULEAU_ARRET, MOTEUR_ROULEAU); //On recrache
            }
            else
            {
                robotState.taskState = T_VC_FIN;
            }
            break;
            
        case T_VC_AVANCE_0_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_VC_AVANCE_1,2500);
            break;

        case T_VC_AVANCE_1:
            SendState();
            StartMove(T_VC_AVANCE_1_EN_COURS,550,taskVidageChateau.startingPosY,ALIGNED_WITH_MOVE);
            PWMSetSpeedConsigne(VITESSE_ROULEAU_RECRACHE, MOTEUR_ROULEAU); //On recrache
            break;
            
        case T_VC_AVANCE_1_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_VC_TOURNE_1,1600);   
            break;

        case T_VC_TOURNE_1:
            StartTempo(T_VC_TOURNE_1_EN_COURS);
            SetRobotTargetAngle(75);
            robotState.AligneAvantVersDirectionMouvement = 1;
            PWMSetSpeedConsigne(VITESSE_ROULEAU_RECRACHE, MOTEUR_ROULEAU); //On recrache
            break;
            
        case T_VC_TOURNE_1_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_VC_TOURNE_2,500);
            break;
            
        case T_VC_TOURNE_2:
            SetRobotTargetAngle(95);
            StartTempo(T_VC_TOURNE_2_EN_COURS);
            PWMSetSpeedConsigne(VITESSE_ROULEAU_RECRACHE, MOTEUR_ROULEAU); //On recrache
            break;
            
        case T_VC_TOURNE_2_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_VC_RECULE_1,700);
            break;

//        case T_VC_AVANCE_1_BIS:
//            SendState();
//            SetRobotTargetPosition(550, robotState.yPosFromOdometry);
//            robotState.taskState = T_VC_AVANCE_1_BIS_EN_COURS;
//            PWMSetSpeedConsigne(VITESSE_ROULEAU_RECRACHE, MOTEUR_ROULEAU); //On recrache
//            robotState.timestampTaskStart = timestamp;
//            robotState.AligneAvantVersDirectionMouvement = 1;
//            break;
//        case T_VC_AVANCE_1_BIS_EN_COURS:
//            if (sysEvents.TargetAtteinteEvent || ((timestamp - robotState.timestampTaskStart) > 1200))
//            {
//                robotState.taskState = T_VC_RECULE_1;
//                robotState.AligneAvantVersDirectionMouvement = 0;
//            }
//            break;

        case T_VC_RECULE_1:
            SendState();
            StartMove(T_VC_RECULE_1_EN_COURS,330,taskVidageChateau.startingPosY,NOT_ALIGNED);
            PWMSetSpeedConsigne(VITESSE_ROULEAU_RECRACHE, MOTEUR_ROULEAU); //On recrache
            break;

        case T_VC_RECULE_1_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_VC_AVANCE_0,1600);
            break;

            //        case T_VC_AVANCE_2:
            //            SendState();
            //            robotState.AligneAvantVersDirectionMouvement = 1;
            //            SetRobotTargetPosition(550, robotState.yPosFromOdometry);
            //            SendState();
            //            robotState.taskState = T_VC_AVANCE_2_EN_COURS;
            //            robotState.timestampTaskStart = timestamp;
            //            break;
            //
            //        case T_VC_AVANCE_2_EN_COURS:
            //            if (sysEvents.TargetAtteinteEvent || ((timestamp - robotState.timestampTaskStart) > 300))
            //            {
            //                robotState.AligneAvantVersDirectionMouvement = 0;
            //                robotState.taskState = T_VC_RECULE_2;
            //            }
            //            break;
            //
            //        case T_VC_RECULE_2:
            //            SendState();
            //            robotState.AligneAvantVersDirectionMouvement = 0;
            //            Recule(180);
            //            robotState.taskState = T_VC_RECULE_2_EN_COURS;
            //            robotState.timestampTaskStart = timestamp;
            //            break;
            //        case T_VC_RECULE_2_EN_COURS:
            //            if (sysEvents.TargetAtteinteEvent || ((timestamp - robotState.timestampTaskStart) > 1500))
            //            {
            //                robotState.AligneAvantVersDirectionMouvement = 0;
            //                robotState.taskState = T_VC_FIN;
            //            }
            //            break;

        case T_VC_FIN:
            SendState();
            robotState.taskState = T_VC_FIN_EN_COURS;
            PWMSetSpeedConsigne(VITESSE_ROULEAU_ARRET, MOTEUR_ROULEAU); //On recrache
            //taskVidageChateau.nbTaskCompleted++;
            break;

        case T_VC_FIN_EN_COURS:
            break;

        default:
            robotState.taskState = T_VC_FIN;
            break;
    }
}