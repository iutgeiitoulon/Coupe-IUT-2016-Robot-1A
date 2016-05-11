#include "TaskAvalageChateau.h"
#include "Task.h"
#include "RobotManagement.h"
#include "main.h"
#include "robot.h"
#include "IO.h"
#include "PWM.h"
#include "events.h"
#include "timer.h"
#include "Toolbox.h"


#define NOMBRE_ALLER_RETOUR 2


volatile TASK_BITS taskAvalageChateau;

unsigned char nbAllerRetour = 0;
unsigned char nbTourne = 0;

void TaskAvalageChateauInit()
{
    taskAvalageChateau.startingPosX = 460;
    taskAvalageChateau.startingPosY = 900;
}

unsigned char TaskAvalageChateauIsFinished()
{
    if (robotState.taskState == T_AC_FIN_EN_COURS)
    {
        return 1;
    }
    else
        return 0;
}

void TaskAvalageChateauStart(int positionY)
{
    robotState.taskEnCours = TASK_AVALAGE_CHATEAU;
    robotState.taskState = T_AC_INIT;
    taskAvalageChateau.startingPosY = positionY;
    nbAllerRetour = 0;
    //taskAvalageChateau.nbCall++;
}

void TaskAvalageChateau()
{
    switch (robotState.taskState)
    {
        case T_AC_INIT:
            //On dit au robot d'aller à la position initiale
            PWMSetSpeedConsigne(VITESSE_ROULEAU_ARRET, MOTEUR_ROULEAU);
            StartMove(T_AC_INIT_EN_COURS, taskAvalageChateau.startingPosX, taskAvalageChateau.startingPosY, ALIGNED_WITH_MOVE);
            break;

        case T_AC_INIT_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_AC_1, 5000);
            break;

        case T_AC_1:
            if (nbAllerRetour < NOMBRE_ALLER_RETOUR)
            {
                nbAllerRetour++;
                nbTourne = 0;
                PWMSetSpeedConsigne(VITESSE_ROULEAU_AVALE, MOTEUR_ROULEAU); //On avale
                if (taskAvalageChateau.startingPosY >= 1500)
                    StartMove(T_AC_1_EN_COURS, 220, taskAvalageChateau.startingPosY - 30, ALIGNED_WITH_MOVE);
                else
                    StartMove(T_AC_1_EN_COURS, 220, taskAvalageChateau.startingPosY - 30, ALIGNED_WITH_MOVE);
            }
            else
            {
                robotState.taskState = T_AC_FIN;
            }
            break;

        case T_AC_1_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_AC_2, 1500);
            break;

        case T_AC_2://ATTENTE
            StartTempo(T_AC_2_EN_COURS);
            break;
        case T_AC_2_EN_COURS:
            IsCurrentTempoFinished(T_AC_3, 300);
            break;
        case T_AC_3:
            if (nbTourne < 2)
            {
                nbTourne++;
                SendState();
                if (taskAvalageChateau.startingPosY >= 1500)
                    SetRobotTargetAngle(-80);
                else
                    SetRobotTargetAngle(-100);
                StartTempo(T_AC_3_EN_COURS);
            }
            else
            {
                robotState.taskState = T_AC_5;
            }
            break;
            
        case T_AC_3_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_AC_4, 400);
            break;

        case T_AC_4:
            SendState();

            if (taskAvalageChateau.startingPosY >= 1500)
                SetRobotTargetAngle(-110);
            else
                SetRobotTargetAngle(-70);
            SetRobotTargetAngle(-120);
            StartTempo(T_AC_4_EN_COURS);
            break;
            
        case T_AC_4_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_AC_3, 800);
            break;

        case T_AC_5:
            SendState();
            StartMove(T_AC_5_EN_COURS,370,taskAvalageChateau.startingPosY,ALIGNED_WITH_MOVE);
            break;
            
        case T_AC_5_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_AC_1, 1800);
            break;

            //        case T_AC_RECULE_2:
            //            SendState();
            //            SetRobotTargetPosition(400, taskAvalageChateau.startingPosY);
            //            robotState.timestampTaskStart = timestamp;
            //            robotState.taskState = T_AC_RECULE_2_EN_COURS;
            //            break;
            //        case T_AC_RECULE_2_EN_COURS:
            //            if (sysEvents.TargetAtteinteEvent || ((timestamp - robotState.timestampTaskStart) > 1000))
            //            {
            //                robotState.taskState = T_AC_ATTAQUE;
            //            }
            //            break;

        case T_AC_FIN:
            robotState.taskState = T_AC_FIN_EN_COURS;
            //taskAvalageChateau.nbTaskCompleted++;
            break;

        case T_AC_FIN_EN_COURS:
            break;
            
        default:
            robotState.taskState = T_AC_FIN;
            break;
    }
}