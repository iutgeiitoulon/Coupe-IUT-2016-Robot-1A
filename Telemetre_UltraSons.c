#include "Telemetre_UltraSons.h"
#include "ustv_i2c.h"
#include "timer.h"
#include "UART_Protocol.h"
#include "events.h"
#include "IO.h"
#include "robot.h"
#include "main.h"

#if defined (ROBOT_EUROBOT)
#define SEUIL_DETECTION_OBSTACLE 20
#elif defined(ROBOT_CACHAN) 
#define SEUIL_DETECTION_OBSTACLE 5
#endif

unsigned int seuilDetectionTelemetreCentre;
unsigned int seuilDetectionTelemetreDroit;
unsigned int seuilDetectionTelemetreGauche;

void InitTelemetres(void)
{
    //Configuration des télémètres
    //I2C1Write1(0xE0, RANGE_REG, 0x20); //Configuration du range maximal = value*43mm + 43mm
    I2C1Write1(0xE2, RANGE_REG, 0x20); //Configuration du range maximal = value*43mm + 43mm
    I2C1Write1(0xE4, RANGE_REG, 0x20); //Configuration du range maximal = value*43mm + 43mm
}

void SetUpAddressTelemetre(unsigned char address) //attention reprogramme tout les capteurs !
{
    I2C1Write1(ALL_I2C, CMD_REG, 0xA0); // cf. datasheet SRF08 page 7
    I2C1Write1(ALL_I2C, CMD_REG, 0xAA);
    I2C1Write1(ALL_I2C, CMD_REG, 0xA5);
    I2C1Write1(ALL_I2C, CMD_REG, address);
}

void StartNewUltrasonicMeasure()
{
    unsigned char data[10];

    //Cas Lecture et écriture synchrone
    I2C1ReadN(TELEMETRE_GAUCHE, CMD_REG, data, 10);
    robotState.distanceTelemetreGauche = ((unsigned int) data[2])*256 + ((unsigned int) data[3]);
    I2C1ReadN(TELEMETRE_DROIT, CMD_REG, data, 10);
    robotState.distanceTelemetreDroit = ((unsigned int) data[2])*256 + ((unsigned int) data[3]);

    I2C1Write1(ALL_I2C, CMD_REG, 0x51); // Lecture en broadcast avec retour 
    // distance en cm - cf datasheet page 4

    //Post-processing des distances ultrason mesurées
    if (((robotState.distanceTelemetreGauche < SEUIL_DETECTION_OBSTACLE) || (robotState.distanceTelemetreDroit < SEUIL_DETECTION_OBSTACLE)) && sysEvents.DisableDetectionObstacle == 0)
    {
#if defined(ROBOT_EUROBOT)
        //On regarde si on n'est pas dans une zone d'exclusion
        if ((robotState.xPosFromOdometry < 500) && (robotState.xPosFromOdometry > -500))
        {
            //On ne déclenche pas : proche du chateau
        }
        //        else
#endif
        sysEvents.UltrasonicObjectDetectionEvent = 1;
    }
    else
    {
    }


}

void SendUltrasonicMeasure()
{
    unsigned char payload[8];
    int pos = 0;
    payload[pos++] = (unsigned char) ((timestamp) >> 24);
    payload[pos++] = (unsigned char) ((timestamp) >> 16);
    payload[pos++] = (unsigned char) ((timestamp) >> 8);
    payload[pos++] = (unsigned char) ((timestamp) >> 0);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreGauche) >> 8);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreGauche) >> 0);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreDroit) >> 8);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreDroit) >> 0);

    Uart1EncodeAndSendMessage(TELEMETRE_ULTRASONIC_DATA, pos, payload);
}

void InitSeuilDetectionSRF08(void)
{
    seuilDetectionTelemetreGauche = 20;
    seuilDetectionTelemetreDroit = 20;
}
