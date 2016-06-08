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

#define COMMAND_SERVO_1 _LATA8
#define COMMAND_SERVO_2 _LATB4
#define COMMAND_SERVO_4 _LATC8
#define COMMAND_SERVO_5 _LATC6

#define  ENTREE_CAPTEUR_COULEUR_1 PORTBbits.RB6
#define  ENTREE_CAPTEUR_COULEUR_2 PORTBbits.RB5
#define  BUMPERS PORTBbits.RB4
#define ENTREE_SELECTEUR_COULEUR 42
#define ENTREE_JACK PORTAbits.RA4

// Prototypes fonctions
void InitIO();
void LockIO();
void UnlockIO();

#endif /* IO_H */
