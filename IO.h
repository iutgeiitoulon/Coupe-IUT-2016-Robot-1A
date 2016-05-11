#ifndef IO_H
#define IO_H

#include <p33FJ128MC804.h>
// LED PORT MAKING

#define LED_BLANCHE _LATA7             //Led Blanche
#define LED_BLEUE _LATA9             //Led Bleue
#define LED_ORANGE _LATA10            //Led Orange

#define MOTOR_DROIT_IN1 _LATB14      //Moteur Droit
#define MOTOR_DROIT_IN2 _LATB15      //Moteur Droit

#define MOTOR_GAUCHE_IN1 _LATB12      //Moteur Gauche
#define MOTOR_GAUCHE_IN2 _LATB13      //Moteur Gauche

#define MOTOR_3_IN1 _LATB10      //Moteur Rouleau
#define MOTOR_3_IN2 _LATB11      //Moteur Rouleau

#define COMMAND_SERVO_1 _LATA4
#define COMMAND_SERVO_2 _LATB4
#define COMMAND_SERVO_3 _LATA8
#define COMMAND_SERVO_4 _LATC8
#define COMMAND_SERVO_5 _LATC6

#define  ENTREE_SELECTEUR_COULEUR PORTCbits.RC1
#define ENTREE_JACK PORTCbits.RC9

// Prototypes fonctions
void InitIO();
void LockIO();
void UnlockIO();

#endif /* IO_H */
