#include "robot.h"
#include "UART_Protocol.h"

volatile ROBOT_STATE_BITS robotState;

void EnvoiAngleInterface()
{
    unsigned char payload[8];
    int pos = 0;
    payload[pos++] = robotState.distanceTelemetreGauche;
    payload[pos++] = robotState.distanceTelemetreCentre;
    payload[pos++] = robotState.vitesseGaucheConsigne;
    payload[pos++] = robotState.vitesseDroitConsigne;
    payload[pos++] = (unsigned char) (((int) (robotState.AngleDegreFromBalise)) >> 8);
    payload[pos++] = (unsigned char) (((int) (robotState.AngleDegreFromBalise)) >> 0);
    payload[pos++] = robotState.distanceTelemetreDroit;
    payload[pos++] = 0;

    Uart1EncodeAndSendMessage(BALISE_ANGLES, pos, payload);
}



