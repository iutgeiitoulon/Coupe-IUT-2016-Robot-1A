/* 
 * File:   PWM.h
 * Author: E105-POSTE2
 *
 * Created on 14 septembre 2015, 11:55
 */

#ifndef PWM_H
#define	PWM_H

//#define MOTEUR_DROIT 1
//#define MOTEUR_GAUCHE 2

//extern signed char PWMSpeedConsigne[4];
//extern double PWMSpeedCorrectorOutput[4];
//extern signed char PWMSpeedCurrentCommand[4];

void InitPWM(void);
void PWMSetSpeedConsigne(double, char);
void PWMUpdateSpeed(unsigned char);
void SendTelemetrieAsservissementVitesse();

#endif	/* PWM_H */

