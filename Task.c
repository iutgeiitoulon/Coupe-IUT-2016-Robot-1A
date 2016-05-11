#include "robot.h"
#include "RobotManagement.h"
#include "timer.h"
#include "events.h"

void StartMove(unsigned int nextState, int X, int Y, unsigned char alignMvt)
{
    //SendState();
    robotState.AligneAvantVersDirectionMouvement = alignMvt; //on attaque en marche avant !!!
    SetRobotTargetPosition(X, Y);
    robotState.timestampTaskStart = timestamp;
    robotState.taskState = nextState;
}

void StartTempo(unsigned int nextState)
{
    //SendState();
    robotState.timestampTaskStart = timestamp;
    robotState.taskState = nextState;
}

void IsCurrentMoveFinishedGoToState(unsigned char nextTask, int timeout)
{
    if (sysEvents.TargetAtteinteEvent || ((timestamp - robotState.timestampTaskStart) > timeout))
    {
        robotState.taskState = nextTask;
        robotState.AligneAvantVersDirectionMouvement = 0; //            
    }
}

void IsCurrentTempoFinished(unsigned char nextTask, int timeout)
{
    if ((timestamp - robotState.timestampTaskStart) > timeout)
    {
        robotState.taskState = nextTask;
        robotState.AligneAvantVersDirectionMouvement = 0; //            
    }
}

