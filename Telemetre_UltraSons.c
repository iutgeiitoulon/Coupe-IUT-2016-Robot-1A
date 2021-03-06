#include "Telemetre_UltraSons.h"
#include "ustv_i2c.h"
#include "timer.h"
#include "UART_Protocol.h"
#include "events.h"
#include "IO.h"
#include "robot.h"
#include "main.h"
#include "Toolbox.h"

#define SEUIL_DETECTION_OBSTACLE 15


unsigned int seuilDetectionTelemetreCentre;
unsigned int seuilDetectionTelemetreDroit;
unsigned int seuilDetectionTelemetreGauche;

void InitTelemetres(void)
{
    //Configuration des t�l�m�tres
    //I2C1Write1(0xE0, RANGE_REG, 0x20); //Configuration du range maximal = value*43mm + 43mm
    
    //Distances maximales de d�tection
    I2C1Write1(0xE2, RANGE_REG, 0xFF); //Configuration du range maximal = value*43mm + 43mm
    I2C1Write1(0xE4, RANGE_REG, 0xFF); //Configuration du range maximal = value*43mm + 43mm
    I2C1Write1(0xFE, RANGE_REG, 0xFF); //Configuration du range maximal = value*43mm + 43mm
    
    //Gain analogique
    I2C1Write1(0xE2, ANALOG_GAIN_REG, 0x00); //
    I2C1Write1(0xE4, ANALOG_GAIN_REG, 0x00); //
    I2C1Write1(0xFE, ANALOG_GAIN_REG, 0x00); //
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
    unsigned int distanceGaucheRecu = 0;
    unsigned int distanceDroitRecu = 0;
    unsigned int distanceCentreRecu = 0;
    //Cas Lecture et �criture synchrone
    I2C1ReadN(TELEMETRE_GAUCHE, CMD_REG, data, 10);
    distanceGaucheRecu = ((unsigned int) data[2])*256 + ((unsigned int) data[3]);
    I2C1ReadN(TELEMETRE_DROIT, CMD_REG, data, 10);
    distanceDroitRecu = ((unsigned int) data[2])*256 + ((unsigned int) data[3]);
    I2C1ReadN(TELEMETRE_CENTRE, CMD_REG, data, 10);
    distanceCentreRecu = ((unsigned int) data[2])*256 + ((unsigned int) data[3]);
    
    if(distanceGaucheRecu <=1 )
    {
        distanceGaucheRecu = 60 ;
    }
    if(distanceCentreRecu <=1 )
    {
        distanceCentreRecu = 60 ;
    }
    if(distanceDroitRecu <=1 )
    {
        distanceDroitRecu = 60 ;
    }
    
    robotState.distanceTelemetreGauche=Limiteur(distanceGaucheRecu,3,60);
    robotState.distanceTelemetreDroit=Limiteur(distanceDroitRecu,3,60);
    robotState.distanceTelemetreCentre=Limiteur(distanceCentreRecu,3,60);
    I2C1Write1(ALL_I2C, CMD_REG, 0x51); // Lecture en broadcast avec retour 
    // distance en cm - cf datasheet page 4

    //Post-processing des distances ultrason mesur�es
    if (((robotState.distanceTelemetreGauche < SEUIL_DETECTION_OBSTACLE) || (robotState.distanceTelemetreDroit < SEUIL_DETECTION_OBSTACLE)|| (robotState.distanceTelemetreCentre < SEUIL_DETECTION_OBSTACLE)) && sysEvents.DisableDetectionObstacle == 0)
    {
#if defined(ROBOT_EUROBOT)
#endif
        sysEvents.UltrasonicObjectDetectionEvent = 1;
    }
    else
    {
    }


}

void SendUltrasonicMeasure()
{
    unsigned char payload[10];
    int pos = 0;
//    payload[pos++] = (unsigned char) ((timestamp) >> 24);
//    payload[pos++] = (unsigned char) ((timestamp) >> 16);
//    payload[pos++] = (unsigned char) ((timestamp) >> 8);
//    payload[pos++] = (unsigned char) ((timestamp) >> 0);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreGauche) >> 8);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreGauche) >> 0);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreDroit) >> 8);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreDroit) >> 0);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreCentre) >> 8);
    payload[pos++] = (unsigned char) (((int) robotState.distanceTelemetreCentre) >> 0);

    Uart1EncodeAndSendMessage(TELEMETRE_ULTRASONIC_DATA, pos, payload);
}

void InitSeuilDetectionSRF08(void)
{
    seuilDetectionTelemetreGauche = 2;
    seuilDetectionTelemetreDroit = 2;
    seuilDetectionTelemetreCentre = 2;
}

