#include "robot.h"
#include "UART_Protocol.h"

volatile ROBOT_STATE_BITS robotState;

void EnvoiAngleInterface()
{
    unsigned char payload[8];
    int pos = 0;
    payload[pos++] = 0;
    payload[pos++] = 0;
    payload[pos++] = 0;
    payload[pos++] = 0;
    payload[pos++] = 0;
    payload[pos++] = 0;
    payload[pos++] = (unsigned char) (((int) (robotState.AngleDegreFromBalise)) >> 8);
    payload[pos++] = (unsigned char) (((int) (robotState.AngleDegreFromBalise)) >> 0);

    Uart1EncodeAndSendMessage(BALISE_ANGLES, pos, payload);
}

