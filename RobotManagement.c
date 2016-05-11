#include "RobotManagement.h"
#include "main.h"
#include "AsservissementPolaire.h"
#include "QEI.h"
#include "UART_Protocol.h"
#include "PWM.h"
#include "robot.h"
#include "events.h"
#include "IO.h"
#include "Toolbox.h"

void SetRobotTargetPosition(int X, int Y)
{
    //On bascule l'asservissement en position
    robotState.ModeAsservissement = ASSERVISSEMENT_POSITION;
    robotState.xPosTarget = X;
    if(ENTREE_SELECTEUR_COULEUR == VERT)
        robotState.xPosTarget = 2000-robotState.xPosTarget;
    robotState.yPosTarget = Y;
    sysEvents.TargetProcheEvent = 0;
    sysEvents.TargetAtteinteEvent = 0;
    
}

void SetRobotTargetAngle(double angleDegre)
{
    //On bascule l'asservissement en position
    robotState.ModeAsservissement = ASSERVISSEMENT_ANGLE;
    robotState.angleRadianTarget = DegreeToRadian(angleDegre);
    sysEvents.TargetProcheEvent = 0;
    sysEvents.TargetAtteinteEvent = 0;    
}

void SetRobotVitesseAsservie(signed char vitesseGauche, signed char vitesseDroit)
{
    //On bascule l'asservissement en position
    robotState.ModeAsservissement = ASSERVISSEMENT_VITESSE;
    PWMSetSpeedConsigne(vitesseDroit, MOTEUR_DROIT);
    PWMSetSpeedConsigne(vitesseGauche, MOTEUR_GAUCHE);
}

void SetRobotVitesseLibre(signed char vitesseGauche, signed char vitesseDroit)
{
    //On bascule l'asservissement en position
    robotState.ModeAsservissement = ASSERVISSEMENT_DESACTIVE;
    PWMSetSpeedConsigne(vitesseDroit, MOTEUR_DROIT);
    PWMSetSpeedConsigne(vitesseGauche, MOTEUR_GAUCHE);
}