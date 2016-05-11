#include <math.h>
#include "QEI.h"
#include "UART_Protocol.h"
#include "timer.h"
#include "AsservissementPolaire.h"
#include "AsservissementVitesse.h"
#include "Toolbox.h"
#include "robot.h"
#include "events.h"
#include "Toolbox.h"

#define DELTA_VITESSE_MAX 10
#define DELTA_ANGLE_MAX 4
#define POS_VITESSE_MAX 8


#define SEUIL_OBJECTIF_EN_VUE 50
#define SEUIL_OBJECTIF_PROCHE 60
#define SEUIL_OBJECTIF_ATTEINT 20
#define SEUIL_ANGLE_ATTEINT 2

//#ifdef GROS_ROBOT
//#define GAIN_VITESSE_LINEAIRE 8
//#define GAIN_VITESSE_ROTATION 5
//#define TURN_SPEED_LIMIT 26
//#define FORWARD_SPEED_LIMIT 250
//#elif defined(PETIT_ROBOT)
//#define GAIN_VITESSE_LINEAIRE 3
//#define TURN_SPEED_LIMIT 30
//#define FORWARD_SPEED_LIMIT 100
//#endif

double KpDistance = 0.04;
//double KpAngle = 7.5;

double vitesseMoyenne_T1 = 0;
double vitesseMoyenne = 0;
double deltaVitesse_T1 = 0;
double deltaVitesse = 0;

int counter = 0;

double consigneDeltaVitesse;
double consigneVitesseMoyenne;
double deltaAngleDegre;
double distanceColineaire;

int* AsservissementPosition()
{
    counter++;
    double distanceCible = sqrt(pow(robotState.xPosFromOdometry - robotState.xPosTarget, 2) + pow(robotState.yPosFromOdometry - robotState.yPosTarget, 2));

    double angleCibleRadian = -atan2((robotState.xPosTarget - robotState.xPosFromOdometry), (robotState.yPosTarget - robotState.yPosFromOdometry)); //attention : en radian
    distanceColineaire = distanceCible * cos(angleCibleRadian - robotState.angleRadianFromOdometry);


    if ((distanceColineaire < SEUIL_OBJECTIF_PROCHE) && (distanceCible < SEUIL_OBJECTIF_EN_VUE) && (distanceColineaire > -SEUIL_OBJECTIF_PROCHE) && (distanceCible > -SEUIL_OBJECTIF_EN_VUE))
        sysEvents.TargetProcheEvent = 1;
    if ((distanceColineaire < SEUIL_OBJECTIF_ATTEINT) && (distanceCible < SEUIL_OBJECTIF_EN_VUE) && (distanceColineaire > -SEUIL_OBJECTIF_ATTEINT) && (distanceCible > -SEUIL_OBJECTIF_EN_VUE))
        sysEvents.TargetAtteinteEvent = 1;
    double distanceColineaireLimitee = Limiteur(robotState.GainVitesseLineaire*distanceColineaire, 
                                                -robotState.VitesseLimiteLineaire, 
                                                robotState.VitesseLimiteLineaire);

    double vitesseGauche;
    double vitesseDroite;

    double deltaAngleRadian = angleCibleRadian - robotState.angleRadianFromOdometry;
    deltaAngleRadian = Modulo2PIAngleRadian(deltaAngleRadian);

    if (robotState.AligneAvantVersDirectionMouvement)
    {
        //On ne fait rien de plus
    }
    else
    {
        deltaAngleRadian = ModuloPIAngleRadian(deltaAngleRadian);
    }

    deltaAngleDegre = RadianToDegree(deltaAngleRadian);
    double deltaAngleRadianLimite = Limiteur(robotState.GainVitesseRotation *RadianToMmInRotation(deltaAngleRadian), 
                                             -robotState.VitesseLimiteRotation, 
                                             robotState.VitesseLimiteRotation);

    //consigneDeltaVitesse = 0;
    consigneVitesseMoyenne = 0;


    //Gestion du déplacement angulaire
    double consigneAngulaire = CorrecteurAngulaire(deltaAngleRadianLimite);
    deltaVitesse = consigneAngulaire;

    vitesseGauche = deltaVitesse;
    vitesseDroite = -deltaVitesse;
    
    
    double diffAngle=0;
    if ((deltaAngleDegre < DELTA_ANGLE_MAX) && (deltaAngleDegre > -DELTA_ANGLE_MAX))
        diffAngle = DELTA_ANGLE_MAX - Abs(deltaAngleDegre);
    else
        diffAngle = DELTA_ANGLE_MAX;
    
    double coeffTransition = diffAngle / DELTA_ANGLE_MAX;
    
    if ((deltaAngleDegre < DELTA_ANGLE_MAX) && (deltaAngleDegre > -DELTA_ANGLE_MAX))
    {
        //Gestion du déplacement longitudinal
        consigneVitesseMoyenne = CorrecteurLineaire(distanceColineaireLimitee);
        vitesseMoyenne = consigneVitesseMoyenne;

        //Calcul des vitesse sur chacun des moteurs
        vitesseGauche += coeffTransition * vitesseMoyenne;
        vitesseDroite += coeffTransition * vitesseMoyenne;
    }

    static int consignesVitesse[2];
    consignesVitesse[0] = (int) vitesseGauche;
    consignesVitesse[1] = (int) vitesseDroite;

    sysEvents.AsservissementPositionEvent = 1;
    return consignesVitesse;
}

int* AsservissementAngle()
{
    counter++;
    
    double angleCibleRadian = robotState.angleRadianTarget; //attention : en radian

    double vitesseGauche;
    double vitesseDroite;

    double deltaAngleRadian = angleCibleRadian - robotState.angleRadianFromOdometry;
    deltaAngleRadian = Modulo2PIAngleRadian(deltaAngleRadian);

    deltaAngleDegre = RadianToDegree(deltaAngleRadian);
    double deltaAngleRadianLimite = Limiteur(robotState.GainVitesseRotation *RadianToMmInRotation(deltaAngleRadian), 
                                             -robotState.VitesseLimiteRotation, 
                                             robotState.VitesseLimiteRotation);
        
    if (deltaAngleDegre < SEUIL_ANGLE_ATTEINT && deltaAngleDegre > -SEUIL_ANGLE_ATTEINT)
        sysEvents.TargetAtteinteEvent = 1;

    consigneVitesseMoyenne = 0;

    //Gestion du déplacement angulaire
    double consigneAngulaire = CorrecteurAngulaire(deltaAngleRadianLimite);
    deltaVitesse = consigneAngulaire;

    vitesseGauche = deltaVitesse;
    vitesseDroite = -deltaVitesse;

    static int consignesVitesse[2];
    consignesVitesse[0] = (int) vitesseGauche;
    consignesVitesse[1] = (int) vitesseDroite;

    sysEvents.AsservissementAngleEvent = 1;
    return consignesVitesse;
}

void SendTelemetrieAsservissementAngulaire()
{
    unsigned char payload[8];
    int pos = 0;
    payload[pos++] = (unsigned char) ((timestamp) >> 24);
    payload[pos++] = (unsigned char) ((timestamp) >> 16);
    payload[pos++] = (unsigned char) ((timestamp) >> 8);
    payload[pos++] = (unsigned char) ((timestamp) >> 0);
    payload[pos++] = (unsigned char) (((int) deltaAngleDegre) >> 8);
    payload[pos++] = (unsigned char) (((int) deltaAngleDegre) >> 0);
    payload[pos++] = (unsigned char) (((int) distanceColineaire) >> 8);
    payload[pos++] = (unsigned char) (((int) distanceColineaire) >> 0);
    payload[pos++] = (unsigned char) (((int) consigneVitesseMoyenne) >> 8);
    payload[pos++] = (unsigned char) (((int) consigneVitesseMoyenne) >> 0);
    payload[pos++] = (unsigned char) (((int) consigneDeltaVitesse) >> 8);
    payload[pos++] = (unsigned char) (((int) consigneDeltaVitesse) >> 0);

    Uart1EncodeAndSendMessage
            (ASSERVISSEMENT_POSITION_TELEMETRIE, pos, payload);
}


double ea0, ea1, ea2; //valeurs de l'entrée du correcteur angulaire à t, t-1, t-2
double sa0, sa1, sa2; //valeurs de la sortie du correcteur angulaire à t, t-1, t-2

double el0, el1, el2; //valeurs de l'entrée du correcteur lineaire à t, t-1, t-2
double sl0, sl1, sl2; //valeurs de la sortie du correcteur lineaie à t, t-1, t-2

double alpha_a;
double beta_a;
double delta_a;

double alpha_l;
double beta_l;
double delta_l;


//double eg0, eg1, eg2; //valeurs de l'entrée du correcteur gauche à t, t-1, t-2
//double sg0, sg1, sg2; //valeurs de la sortie du correcteur gauche à t, t-1, t-2

void SetUpPidAsservissementAngulaire(double Ku, double Tu)
{
    //Réglage de Ziegler Nichols classique
    //    double Kp = 0.6 * Ku;
    //    double Ti = 0.5 * Tu;
    //    double Td = 0.125 * Tu;

    //Réglage de Ziegler Nichols sans dépassements : un tout petit peu mou
    double Kp = 0.2 * Ku;
    double Ti = 0.5 * Tu;
    double Td = 0.33 * Tu;

    //Réglage de Ziegler Nichols faible dépassements : limite de stabilité
    //    double Kp = 0.33 * Ku;
    //    double Ti = 0.5 * Tu;
    //    double Td = 0.33 * Tu;

    //Réglage de Ziegler Nichols par la règle de Pessen intégrale : très peu stable
    //    double Kp = 0.7 * Ku;
    //    double Ti = 0.4 * Tu;
    //    double Td = 0.15 * Tu;

    double Ki = Kp / Ti;
    double Kd = Kp * Td;
    alpha_a = Kp + Ki * Te + Kd / Te;
    beta_a = -Kp - 2 * Kd / Te;
    delta_a = Kd / Te;

    //Autre formulation équivalente
    //    alpha = Kp*(1+Te/Ti+Td/Te);
    //    beta = Kp*(-1-2*Td/Te);
    //    delta = Kp*Td/Te;
}

void SetUpPiAsservissementAngulaire(double Ku, double Tu)
{

    //Réglage de Ziegler Nichols :
    //    double Kp = Ku / 2.2;
    //    double Ti = Tu / 1.2;
    //    double Td = 0;

    double Kp = Ku;
    double Ti = Tu;
    double Td = 0;

    double Ki = Kp / Ti;
    double Kd = Kp * Td;
    alpha_a = Kp + Ki * Te + Kd / Te;
    beta_a = -Kp - 2 * Kd / Te;
    delta_a = Kd / Te;
}

void SetUpPAsservissementAngulaire(double K)
{
    //Réglage de Ziegler Nichols sans dépassements : un tout petit peu mou
    double Kp = K;
    double Ki = 0;
    double Kd = 0;
    alpha_a = Kp + Ki * Te + Kd / Te;
    beta_a = -Kp - 2 * Kd / Te;
    delta_a = Kd / Te;

    //Autre formulation équivalente
    //    alpha = Kp*(1+Te/Ti+Td/Te);
    //    beta = Kp*(-1-2*Td/Te);
    //    delta = Kp*Td/Te;
}

double CorrecteurAngulaire(double e)
{
    ea2 = ea1;
    ea1 = ea0;
    ea0 = e;
    sa1 = sa0;
    sa0 = sa1 + ea0 * alpha_a + ea1 * beta_a + ea2 * delta_a;
    return sa0;
}

void SetUpPAsservissementLineaire(double K)
{
    //Réglage de Ziegler Nichols sans dépassements : un tout petit peu mou
    double Kp = K;
    double Ki = 0;
    double Kd = 0;
    alpha_l = Kp + Ki * Te + Kd / Te;
    beta_l = -Kp - 2 * Kd / Te;
    delta_l = Kd / Te;

    //Autre formulation équivalente
    //    alpha = Kp*(1+Te/Ti+Td/Te);
    //    beta = Kp*(-1-2*Td/Te);
    //    delta = Kp*Td/Te;
}


void SetUpPiAsservissementLineaire(double Ku, double Tu)
{

    //Réglage de Ziegler Nichols :
    //    double Kp = Ku / 2.2;
    //    double Ti = Tu / 1.2;
    //    double Td = 0;

    double Kp = Ku;
    double Ti = Tu;
    double Td = 0;

    double Ki = Kp / Ti;
    double Kd = Kp * Td;
    alpha_l = Kp + Ki * Te + Kd / Te;
    beta_l = -Kp - 2 * Kd / Te;
    delta_l = Kd / Te;
}

double CorrecteurLineaire(double e)
{
    el2 = el1;
    el1 = el0;
    el0 = e;
    sl1 = sl0;
    sl0 = sl1 + el0 * alpha_l + el1 * beta_l + el2 * delta_l;
    return sl0;
}
