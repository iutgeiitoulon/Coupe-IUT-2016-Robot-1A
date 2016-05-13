#include "TaskFermeturePortes.h"
#include "Task.h"
#include "RobotManagement.h"
#include "main.h"
#include "robot.h"
#include "IO.h"
#include "PWM.h"
#include "events.h"
#include "timer.h"
#include "Toolbox.h"

volatile TASK_BITS taskFermeturePortes;

void TaskFermeturePortesInit()
{
    taskFermeturePortes.startingPosX = 800; //800
    taskFermeturePortes.startingPosY = 470;
}

void TaskFermeturePortesStart()
{
    robotState.taskEnCours = TASK_FERMETURE_PORTES;
    robotState.taskState = T_FP_INIT;
    //taskFermeturePortes.nbCall++;
}

unsigned char TaskFermeturePortesIsFinished()
{
    if (robotState.taskState == T_FP_FIN_EN_COURS)
        return 1;
    else
        return 0;
}

void TaskFermeturePortes(void)
{
    switch (robotState.taskState)
    {
        case T_FP_INIT:
            StartMove(T_FP_INIT_EN_COURS,800,350,1);
            PWMSetSpeedConsigne(VITESSE_ROULEAU_ARRET, MOTEUR_ROULEAU);
            break;

        case T_FP_INIT_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_FP_1, DEFAULT_TASK_TIMOUT);
            break;


        case T_FP_1:
            StartMove(T_FP_1_EN_COURS,1020,360,1);
            break;

        case T_FP_1_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_FP_2, DEFAULT_TASK_TIMOUT);
            break;


        case T_FP_2:
            SendState();
            SetRobotTargetAngle(180);
            StartTempo(T_FP_2_EN_COURS);
            break;

        case T_FP_2_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_FP_3, DEFAULT_TASK_TIMOUT);
            break;


        case T_FP_3:
            StartMove(T_FP_3_EN_COURS,1020,870,1);
            break;

        case T_FP_3_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_FP_4, DEFAULT_TASK_TIMOUT);
            break;


        case T_FP_4:
            StartMove(T_FP_4_EN_COURS,1020,400,1);
            break;

        case T_FP_4_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_FP_5, DEFAULT_TASK_TIMOUT);
            break;

        case T_FP_5:
            SetRobotTargetAngle(90);
            StartTempo(T_FP_5_EN_COURS);
            break;

        case T_FP_5_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_FP_6, DEFAULT_TASK_TIMOUT); 
            break;

            
        case T_FP_6:
            StartMove(T_FP_6_EN_COURS,80,440,0);
            PWMSetSpeedConsigne(VITESSE_ROULEAU_ARRET, MOTEUR_ROULEAU); //On recrache (pas)
            break;

        case T_FP_6_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_FP_7, 2500);
            break;

        case T_FP_7:
            StartMove(T_FP_7_EN_COURS,460,440,0);
            break;
            
        case T_FP_7_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_FP_FIN, DEFAULT_TASK_TIMOUT);
            break;

        case T_FP_FIN:
//            SendState();
            robotState.taskState = T_FP_FIN_EN_COURS;
            PWMSetSpeedConsigne(VITESSE_ROULEAU_ARRET, MOTEUR_ROULEAU);
            break;

        case T_FP_FIN_EN_COURS:
            break;

        default:
            robotState.taskState = T_FP_FIN;
            break;
    }
}
