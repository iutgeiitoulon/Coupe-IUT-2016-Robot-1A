#include "Task.h"
#include "TaskPechePoisson.h"
#include "RobotManagement.h"
#include "main.h"
#include "robot.h"
#include "IO.h"
#include "PWM.h"
#include "events.h"
#include "timer.h"
#include "Toolbox.h"
#include "Servo.h"

#define NOMBRE_ALLER_RETOUR 2

volatile TASK_BITS taskPechePoisson;

void TaskPechePoissonInit()
{
    taskPechePoisson.startingPosX = 1550;
    taskPechePoisson.startingPosY = 200;
}

unsigned char TaskPechePoissonIsFinished()
{
    if (robotState.taskState == T_PP_FIN_EN_COURS)
        return 1;
    else
        return 0;
}

void TaskPechePoissonStart()
{
    robotState.taskEnCours = TASK_PECHE_POISSON;
    robotState.taskState = T_PP_INIT;
}

int counterTimes = 0;

void TaskPechePoisson()
{
    switch (robotState.taskState)
    {
        case T_PP_INIT:
            //On dit au robot d'aller à la position initiale   
            StartMove(T_PP_INIT_EN_COURS, 1550, 200, ALIGNED_WITH_MOVE);
            SetConsigneServoBras(0);
            SetConsigneServoBrasInactif(0);
            SetConsigneServoPince(0);
            SetConsigneServoPinceInactif(0);
            break;

        case T_PP_INIT_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_INIT_BIS, 10000);
            break;

        case T_PP_INIT_BIS:
            StartMove(T_PP_INIT_BIS_EN_COURS, 1550, 450, ALIGNED_WITH_MOVE);
            break;

        case T_PP_INIT_BIS_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_INIT_TER, 10000);

        case T_PP_INIT_TER:
            counterTimes ++;
            StartMove(T_PP_INIT_TER_EN_COURS, 1885, 700, ALIGNED_WITH_MOVE);
            SetConsigneServoBras(0);
            SetConsigneServoBrasInactif(0);
            SetConsigneServoPince(0);
            SetConsigneServoPinceInactif(0);
            break;
        case T_PP_INIT_TER_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS0, 10000);
            break;

        case T_PP_POS0:
            SetRobotTargetAngle(0);
            StartTempo(T_PP_POS0_EN_COURS);
            break;

        case T_PP_POS0_EN_COURS: //bord de table devant le bac à poissons
            IsCurrentMoveFinishedGoToState(T_PP_POS1, 2000);
            break;

        case T_PP_POS1:
            StartMove(T_PP_POS1_EN_COURS, 1885, 530, ALIGNED_WITH_MOVE);
            SetConsigneServoBras(95);
            break;

        case T_PP_POS1_EN_COURS: //bord de table devant le bac à poissons
            IsCurrentMoveFinishedGoToState(T_PP_POS2, 2000);
            break;

        case T_PP_POS2:
            StartMove(T_PP_POS2_EN_COURS, 1870, 885, ALIGNED_WITH_MOVE);
            break;

        case T_PP_POS2_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS3, 3000);
            break;

        case T_PP_POS3:
            StartMove(T_PP_POS3_EN_COURS, 1885, 700, ALIGNED_WITH_MOVE);
            break;

        case T_PP_POS3_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS3_TER,3000);
            break;

        case T_PP_POS3_TER: //remontée du bras
            StartTempo(T_PP_POS3_TER_EN_COURS);
            SetConsigneServoBras(50);
            break;

        case T_PP_POS3_TER_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS4, 500);
            break;
            //
            //        case T_PP_POS3_BIS:
            //            SetRobotTargetAngle(-90); //esquive du potelet
            //            SendState();
            //            SetConsigneServoBras(50);
            //            robotState.taskState = T_PP_POS3_BIS_EN_COURS;
            //            robotState.timestampTaskStart = timestamp;
            //            break;
            //
            //        case T_PP_POS3_BIS_EN_COURS:
            //            if (sysEvents.TargetProcheEvent || (timestamp - robotState.timestampTaskStart) > 3000)
            //                robotState.taskState = T_PP_POS4;
            //            break;

        case T_PP_POS4:
            StartMove(T_PP_POS4_EN_COURS, 1700, 700, ALIGNED_WITH_MOVE);
            break;

        case T_PP_POS4_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS5, 3000);
            break;

        case T_PP_POS5:
            StartMove(T_PP_POS5_EN_COURS, 1700, 1150, NOT_ALIGNED);
            break;

        case T_PP_POS5_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS6, 5000);
            break;

        case T_PP_POS6:
            StartMove(T_PP_POS6_EN_COURS, 1870 + (counterTimes - 1)*6, 1150,ALIGNED_WITH_MOVE);
            break;

        case T_PP_POS6_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS7, 3000);
            break;

        case T_PP_POS7:
            StartTempo(T_PP_POS7_EN_COURS);
            break;

        case T_PP_POS7_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS8, 3000);
            break;

        case T_PP_POS8:
            SetConsigneServoBras(50);
            StartTempo(T_PP_POS8_EN_COURS);
            break;

        case T_PP_POS8_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS9, 500);
            break;


        case T_PP_POS9:
            SetConsigneServoPince(50);
            StartTempo(T_PP_POS9_EN_COURS);
            break;

        case T_PP_POS9_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_POS10, 500);
            break;

        case T_PP_POS10:
            SetConsigneServoBras(0);
            SetConsigneServoBrasInactif(0);
            SetConsigneServoPince(0);
            SetConsigneServoPinceInactif(0);
            StartMove(T_PP_POS10_EN_COURS,1700,1150,ALIGNED_WITH_MOVE);
            break;

        case T_PP_POS10_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_INIT_TER,3000);
            break;

        case T_PP_POS12:
            StartMove(T_PP_POS10_EN_COURS,1700,700,ALIGNED_WITH_MOVE);
            break;

        case T_PP_POS11_EN_COURS:
            IsCurrentMoveFinishedGoToState(T_PP_INIT_TER,3000);
            break;

        case T_PP_FIN:
            SendState();
            break;

        case T_PP_FIN_EN_COURS:
            break;

        default:
            robotState.taskState = T_PP_FIN;
            break;
    }
}

void BaisseBras()
{
    if (ENTREE_SELECTEUR_COULEUR == VIOLET)
    {
        SetConsigneServoBrasGauche(95);
    }
    else
    {
        SetConsigneServoBrasDroit(95);
    }
}

void MonteBras()
{
    if (ENTREE_SELECTEUR_COULEUR == VIOLET)
    {
        SetConsigneServoBrasGauche(0);
    }
    else
    {
        SetConsigneServoBrasDroit(0);
    }
}