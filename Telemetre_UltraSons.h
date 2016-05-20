/* 
 * File:   Telemetre_UltraSons.h
 * Author: Valentin
 *
 * Created on 15 mai 2014, 19:35
 */

#ifndef TELEMETRE_ULTRASONS_H
#define	TELEMETRE_ULTRASONS_H

#define CMD_REG 0x00
#define LIGHT_SENSOR_REG 0x01
#define RANGE_REG 0x02
#define RANGE_CMD_REG 0x81

#define ALL_I2C 0x00
#define TELEMETRE_GAUCHE 0xE4
#define TELEMETRE_DROIT 0xE2
#define TELEMETRE_CENTRE 0xFE

#define TELEMETRE_ULTRASONIC_DATA 0xB0

extern unsigned int distanceTelemetreDroit;
extern unsigned int distanceTelemetreGauche;

extern unsigned int seuilDetectionTelemetreDroit;
extern unsigned int seuilDetectionTelemetreGauche;

void InitTelemetres(void);
void SetUpAddressTelemetre(unsigned char address);
void InitSeuilDetectionSRF08(void);
void StartNewUltrasonicMeasure();
void SendUltrasonicMeasure();

#endif	/* TELEMETRE_ULTRASONS_H */

