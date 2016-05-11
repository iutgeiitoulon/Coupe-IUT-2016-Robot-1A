#include <p33fj128mc804.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "main.h"
#include "QEI.h"
#include "PWM.h"
#include "IO.h"
#include "AsservissementPolaire.h"
#include "RobotManagement.h"
#include "robot.h"

#define RECEPTION_MAX_PAYLOAD_LENGTH 128

/*******************************************************************************
 * UART 1                       
 *******************************************************************************/

unsigned char rx1ReceptionState = RECEPTION_WAIT;
unsigned char rx1ReceivedFunction;
unsigned int rx1ReceivedPayloadLength;
unsigned char rx1ReceivedPayload[RECEPTION_MAX_PAYLOAD_LENGTH];
unsigned char rx1ReceivedPayloadIndex = 0;

void Uart1EncodeAndSendMessage(int msgFunction,
        int msgPayloadLength, unsigned char* msgPayload)
{
    unsigned char trame[msgPayloadLength + 6];
    int pos = 0, i = 0;
    trame[pos++] = 0xFE;
    trame[pos++] = (char) (msgFunction >> 8);
    trame[pos++] = (char) (msgFunction);
    trame[pos++] = (char) (msgPayloadLength >> 8);
    trame[pos++] = (char) (msgPayloadLength);
    for (i = 0; i < msgPayloadLength; i++)
    {
        trame[pos++] = msgPayload[i];
    }
    trame[pos] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
    SendMessage(trame, msgPayloadLength + 6);
}

void Uart1DecodeMessage(unsigned char c)
{
    switch (rx1ReceptionState)
    {
            char calculatedChecksum;
            char receivedChecksum;
        case RECEPTION_WAIT:
            if (c == 0xFE)
                rx1ReceptionState = RECEPTION_FUNCTION_MSB;
            break;
        case RECEPTION_FUNCTION_MSB:
            rx1ReceivedFunction = (int) (c << 8);
            rx1ReceptionState = RECEPTION_FUNCTION_LSB;
            break;
        case RECEPTION_FUNCTION_LSB:
            rx1ReceivedFunction += (int) c;
            rx1ReceptionState = RECEPTION_PAYLOAD_LENGTH_MSB;
            break;
        case RECEPTION_PAYLOAD_LENGTH_MSB:
            rx1ReceivedPayloadLength = (int) (c << 8);
            rx1ReceptionState = RECEPTION_PAYLOAD_LENGTH_LSB;
            break;
        case RECEPTION_PAYLOAD_LENGTH_LSB:
            rx1ReceivedPayloadLength += (int) c;
            if (rx1ReceivedPayloadLength > RECEPTION_MAX_PAYLOAD_LENGTH)
                rx1ReceptionState = RECEPTION_WAIT;
            else if (rx1ReceivedPayloadLength == 0)
                rx1ReceptionState = RECEPTION_CHECKSUM;
            else
                rx1ReceptionState = RECEPTION_PAYLOAD;
            break;
        case RECEPTION_PAYLOAD:
            rx1ReceivedPayload[rx1ReceivedPayloadIndex] = c;
            rx1ReceivedPayloadIndex++;
            if (rx1ReceivedPayloadIndex == rx1ReceivedPayloadLength)
            {
                rx1ReceivedPayloadIndex = 0;
                rx1ReceptionState = RECEPTION_CHECKSUM;
            }
            break;
        case RECEPTION_CHECKSUM:
            calculatedChecksum = UartCalculateChecksum(rx1ReceivedFunction,
                    rx1ReceivedPayloadLength, rx1ReceivedPayload);
            receivedChecksum = c;
            if (calculatedChecksum == receivedChecksum)
            {
                UartProcessMessage(rx1ReceivedFunction, rx1ReceivedPayloadLength,
                        rx1ReceivedPayload);
            }
            rx1ReceptionState = RECEPTION_WAIT;
            break;
        default:
            rx1ReceptionState = RECEPTION_WAIT;
            break;
    }
}

/*******************************************************************************
 * UART 2                            
 *******************************************************************************/

unsigned char rx2ReceptionState = RECEPTION_WAIT;
unsigned char rx2ReceivedFunction;
unsigned int rx2ReceivedPayloadLength;
unsigned char rx2ReceivedPayload[RECEPTION_MAX_PAYLOAD_LENGTH];
unsigned char rx2ReceivedPayloadIndex = 0;

void Uart2DecodeMessage(unsigned char c)
{
    switch (rx2ReceptionState)
    {
            char calculatedChecksum;
            char receivedChecksum;
        case RECEPTION_WAIT:
            if (c == 0xFE)
                rx2ReceptionState = RECEPTION_FUNCTION_MSB;
            break;
        case RECEPTION_FUNCTION_MSB:
            rx2ReceivedFunction = (int) (c << 8);
            rx2ReceptionState = RECEPTION_FUNCTION_LSB;
            break;
        case RECEPTION_FUNCTION_LSB:
            rx2ReceivedFunction += (int) c;
            rx2ReceptionState = RECEPTION_PAYLOAD_LENGTH_MSB;
            break;
        case RECEPTION_PAYLOAD_LENGTH_MSB:
            rx2ReceivedPayloadLength = (int) (c << 8);
            rx2ReceptionState = RECEPTION_PAYLOAD_LENGTH_LSB;
            break;
        case RECEPTION_PAYLOAD_LENGTH_LSB:
            rx2ReceivedPayloadLength += (int) c;
            if (rx2ReceivedPayloadLength > RECEPTION_MAX_PAYLOAD_LENGTH)
                rx2ReceptionState = RECEPTION_WAIT;
            else if (rx2ReceivedPayloadLength == 0)
                rx2ReceptionState = RECEPTION_CHECKSUM;
            else
                rx2ReceptionState = RECEPTION_PAYLOAD;
            break;
        case RECEPTION_PAYLOAD:
            rx2ReceivedPayload[rx2ReceivedPayloadIndex] = c;
            rx2ReceivedPayloadIndex++;
            if (rx2ReceivedPayloadIndex == rx2ReceivedPayloadLength)
            {
                rx2ReceivedPayloadIndex = 0;
                rx2ReceptionState = RECEPTION_CHECKSUM;
            }
            break;
        case RECEPTION_CHECKSUM:
            calculatedChecksum = UartCalculateChecksum(rx2ReceivedFunction,
                    rx2ReceivedPayloadLength, rx2ReceivedPayload);
            receivedChecksum = c;
            if (calculatedChecksum == receivedChecksum)
            {
                UartProcessMessage(rx2ReceivedFunction, rx2ReceivedPayloadLength,
                        rx2ReceivedPayload);
            }
            rx2ReceptionState = RECEPTION_WAIT;
            break;
        default:
            rx2ReceptionState = RECEPTION_WAIT;
            break;
    }
}

/*******************************************************************************
 * UART COMMON FUNCTIONS
 *******************************************************************************/


unsigned char UartCalculateChecksum(int msgFunction,
        int msgPayloadLength, unsigned char * msgPayload)
{
    char checksum = 0;
    int i = 0;
    checksum ^= 0xFE;
    checksum ^= (char) (msgFunction >> 8);
    checksum ^= (char) (msgFunction);
    checksum ^= (char) (msgPayloadLength >> 8);
    checksum ^= (char) (msgPayloadLength);
    for (i = 0; i < msgPayloadLength; i++)
    {
        checksum ^= msgPayload[i];
    }
    return checksum;
}
void UartProcessMessage(unsigned char function,
        unsigned char payloadLength, unsigned char* payload)
{
    switch (function)
    {
        case SET_ROBOT_STATE:
            SetRobotState(payload[0]);
            break;
        case SET_ROBOT_MANUAL_CONTROL:
            SetRobotAutoControlState(payload[0]);
            break;
        case QEI_SET_POSITION:
            if (payloadLength == 6)
            {
                int xPos = 0;
                int yPos = 0;
                int angleDegre = 0;
                xPos = (((int) payload[0]) << 8) + ((int) payload[1]);
                yPos = (((int) payload[2]) << 8) + ((int) payload[3]);
                angleDegre = (((int) payload[4]) << 8) + ((int) payload[5]);
                QEISetPosition(xPos, yPos, angleDegre);
            }
            break;
        case SET_MOTOR_SPEED:
            
            if (payloadLength == 2)
            {
                //On bascule l'asservissement en vitesse
                robotState.ModeAsservissement = ASSERVISSEMENT_VITESSE;
                PWMSetSpeedConsigne(payload[0], 1);
                PWMSetSpeedConsigne(payload[1], 2);
            }        
            break;
        case SET_POSITION:
            if (payloadLength == 4)
            {
                int X = (((int)payload[0])<<8) + ((int)payload[1]);
                int Y = (((int)payload[2])<<8) + ((int)payload[3]);
                SetRobotTargetPosition(X, Y);
            }
            break;
            
        case SET_LIDAR_SENSITIVITY:
            if(payloadLength==1)
            {
                robotState.LidarSensitivity = payload[0];
            }
            break;
            
        case BALISE_POSITION:
            if (payloadLength == 10)
            {
                //int X = (((int)payload[0])<<8) + ((int)payload[1]);
                //int Y = (((int)payload[2])<<8) + ((int)payload[3]);
                int Xfiltered = (((int)payload[4])<<8) + ((int)payload[5]);
                int Yfiltered = (((int)payload[6])<<8) + ((int)payload[7]);
                int angle = (((int)payload[8])<<8) + ((int)payload[9]);
                
                robotState.xPosFromBalise = Xfiltered;
                robotState.yPosFromBalise = Yfiltered;
                robotState.angleRadianFromBalise = angle;
            }
            break;
            
        default:
            break;
    }
}