#include "QEI.h"
#include <p33FJ128MC804.h>
#include "IO.h"
#include <math.h>
#include "UART_Protocol.h"
#include "events.h"
#include "robot.h"
#include "Toolbox.h"

signed int QeiDroitPosMSbytes = 0;
double QeiDroitPosition = 0;
double QeiDroitPosition_T_1 = 0;

void InitQEI1()
{
    QEI1CONbits.QEIM = 0; // Disable QEI Module
    QEI1CONbits.CNTERR = 0; // Clear any count error
    QEI1CONbits.SWPAB = 0; // QEA and QEB not swapped
    QEI1CONbits.PCDOUT = 0; // Normal I/O pin operation
    QEI1CONbits.POSRES = 0; // Index pulse resets position counter
    POS1CNT = 0; // Reset position counter
    MAX1CNT = 0xFFFF;

    QEI1CONbits.QEIM = 0b111; // X4 mode with position counter reset by index
    // 111 pour pas de reset sinon 110

    IFS3bits.QEI1IF = 0;
    IEC3bits.QEI1IE = 1; // On a une interruption quand on vient saturer le 
    // le compteur de top codeur à 65535    
}

void __attribute__((interrupt, no_auto_psv)) _QEI1Interrupt(void)
{
    IFS3bits.QEI1IF = 0;    

    if (QEI1CONbits.UPDN == 1)
        QeiDroitPosMSbytes++;
    else
        QeiDroitPosMSbytes--;
}
signed int QeiGauchePosMSbytes = 0;
double QeiGauchePosition = 0;
double QeiGauchePosition_T_1 = 0;

void InitQEI2()
{
    QEI2CONbits.QEIM = 0; // Disable QEI Module
    QEI2CONbits.CNTERR = 0; // Clear any count error
    QEI2CONbits.SWPAB = 0; // QEA and QEB not swapped
    QEI2CONbits.PCDOUT = 0; // Normal I/O pin operation
    QEI2CONbits.POSRES = 0; // Index pulse resets position counter
    POS2CNT = 0; // Reset position counter
    MAX2CNT = 0xFFFF;

    QEI2CONbits.QEIM = 0b111; // X4 mode with position counter reset by index
    // 111 pour pas de reset sinon 110

    IFS4bits.QEI2IF = 0;
    IEC4bits.QEI2IE = 1; // On a une interruption quand on vient saturer le 
    // le compteur de top codeur à 65535

    QeiGauchePosition = QeiGauchePosMSbytes;
}

void __attribute__((interrupt, no_auto_psv)) _QEI2Interrupt(void)
{
    IFS4bits.QEI2IF = 0;

    if (QEI2CONbits.UPDN == 1)
        QeiGauchePosMSbytes++;
    else
        QeiGauchePosMSbytes--;
}

double delta_d = 0, delta_g = 0, delta_theta = 0, dx = 0;

void QEIUpdateData()
{
    //On sauvegarde les anciennes valeurs
    QeiDroitPosition_T_1 = QeiDroitPosition;
    QeiGauchePosition_T_1 = QeiGauchePosition;

    //On réactualise les valeurs des positions    
    QeiDroitPosition = ((long) QeiDroitPosMSbytes << 16) + POS1CNT;
    QeiGauchePosition = ((long) QeiGauchePosMSbytes << 16) + POS2CNT;

    //Conversion en mm
    QeiDroitPosition *= 0.01620;
    QeiGauchePosition *= -0.01620;
    
    //Attention : code de correction d'une erreur de synchronisme entre la récupération de 
    //POS1CNT et QEI1posMSbytes : la valeur du 2e peut avoir changé entre le moment où 
    //on lit POSCNT et le moment on lit QEI1posMSbytes -> valeur immense qui plante les algos
    //Se produit rarement, uniquement vers les passages par 0
    if((QeiDroitPosition-QeiDroitPosition_T_1>100) || (QeiDroitPosition-QeiDroitPosition_T_1<-100))
        QeiDroitPosition = QeiDroitPosition_T_1;
    if((QeiGauchePosition-QeiGauchePosition_T_1>100) || (QeiGauchePosition-QeiGauchePosition_T_1<-100))
        QeiGauchePosition = QeiGauchePosition_T_1;
   //Fin de la correction

    //Calcul de la vitesse
    robotState.vitesseDroitFromOdometry = (QeiDroitPosition - QeiDroitPosition_T_1)*FREQ_ECH_QEI; //attention à remultiplier par la fréquence d'échantillonnage (ici 50 Hz))
    robotState.vitesseGaucheFromOdometry = (QeiGauchePosition - QeiGauchePosition_T_1)*FREQ_ECH_QEI;

    //Calcul des deltas de position
    delta_d = QeiDroitPosition - QeiDroitPosition_T_1;
    delta_g = QeiGauchePosition - QeiGauchePosition_T_1;
    delta_theta = atan((delta_d - delta_g) / distroues);
    dx = (delta_d + delta_g) / 2;
    
    if(dx > 30)
    {
        dx=0;
    }

    //Mise à jour des positions à t-1
    robotState.xPosFromOdometry_1 = robotState.xPosFromOdometry;
    robotState.yPosFromOdometry_1 = robotState.yPosFromOdometry;
    robotState.angleRadianFromOdometry_1 = robotState.angleRadianFromOdometry;
        
    //Calcul des positions dans le référentiel du terrain
    robotState.xPosFromOdometry = robotState.xPosFromOdometry_1 + dx*-sin(robotState.angleRadianFromOdometry);
    robotState.yPosFromOdometry = robotState.yPosFromOdometry_1 + dx * cos(robotState.angleRadianFromOdometry);
    robotState.angleRadianFromOdometry = robotState.angleRadianFromOdometry_1 + delta_theta;
    if(robotState.angleRadianFromOdometry > PI)
        robotState.angleRadianFromOdometry -= 2*PI;
    if(robotState.angleRadianFromOdometry < -PI)
        robotState.angleRadianFromOdometry += 2*PI;

    sysEvents.QeiDataEvent = 1;
    sysEvents.PositionDataEvent = 1;
}

void QEIReset()
{
    QEISetPosition(0, 0, 0);
}

void QEISetPosition(int posX, int posY, int angleDegre)
{    
    QeiDroitPosMSbytes = 0;
    QeiGauchePosMSbytes = 0;

    POS1CNT = 0;
    POS2CNT = 0;

    QeiDroitPosition_T_1 = 0;
    QeiGauchePosition_T_1 = 0;  
    QeiDroitPosition = 0;
    QeiGauchePosition = 0;    
    
    robotState.xPosFromOdometry = posX;
    if(ENTREE_SELECTEUR_COULEUR == VERT)
        robotState.xPosFromOdometry = 2000-robotState.xPosFromOdometry;
    robotState.yPosFromOdometry = posY;
    robotState.angleRadianFromOdometry = DegreeToRadian(angleDegre);
    if(ENTREE_SELECTEUR_COULEUR == VERT)
        robotState.angleRadianFromOdometry = -robotState.angleRadianFromOdometry;
}


void SendQeiData() {
    unsigned char qeiPayload[16];
    int i;
    for (i = 0; i < 4; i++) {
        qeiPayload[3 - i] = (char) ((int) QeiDroitPosition >> (8 * i));
        qeiPayload[7 - i] = (char) ((int) QeiGauchePosition >> (8 * i));
        qeiPayload[11 - i] = (char) ((int) robotState.vitesseDroitFromOdometry >> (8 * i));
        qeiPayload[15 - i] = (char) ((int) robotState.vitesseGaucheFromOdometry >> (8 * i));
    }

    Uart1EncodeAndSendMessage(QEI_DATA, 16, qeiPayload);
}

void SendPositionData() {
    unsigned char positionPayload[12];
    int i;
    for (i = 0; i < 4; i++) {
        positionPayload[3 - i] = (char) (((int) robotState.xPosFromOdometry)>>(8 * i));
        positionPayload[7 - i] = (char) (((int) robotState.yPosFromOdometry)>>(8 * i));
        positionPayload[11 - i] = (char) (((int) (RadianToDegree(robotState.angleRadianFromOdometry)))>>(8 * i));
    }
    Uart1EncodeAndSendMessage(POSITION_DATA, 12, positionPayload);
}