#include <p33FJ128MC804.h>
#include "IO.h"
#include "Servo.h"
#include "main.h"

volatile unsigned int countServo = 0;
unsigned int consigneServo1 = 0;
unsigned int consigneServo2 = 0;
unsigned int consigneServo3 = 0;
unsigned int consigneServo4 = 0;
unsigned int consigneServo5 = 0;

void ManageServos(void)
{
    countServo+=1;
    if(countServo<consigneServo1)
        COMMAND_SERVO_1 = 1;
    else
        COMMAND_SERVO_1 = 0;
    if(countServo<consigneServo2)
        COMMAND_SERVO_2 = 1;
    else
        COMMAND_SERVO_2 = 0;
    if(countServo<consigneServo3)
        COMMAND_SERVO_3 = 1;
    else
        COMMAND_SERVO_3 = 0;
    if(countServo<consigneServo4)
        COMMAND_SERVO_4 = 1;
    else
        COMMAND_SERVO_4 = 0;
    if(countServo<consigneServo5)
        COMMAND_SERVO_5 = 1;
    else
        COMMAND_SERVO_5 = 0;

    if(countServo>=200)
    {
        countServo = 0;
    }
}

void SetConsigneServoBrasGauche(unsigned int angle)
{
    if(angle > 95)
        angle = 95;
    if(angle < 0)
        angle = 0;
    consigneServo1 = -0.086*angle+24.2; // -0.43*angle+114;
}


void SetConsigneServoBrasDroit(unsigned int angle)
{
    if(angle > 95)
        angle = 95;
    if(angle < 0)
        angle = 0;
    consigneServo2 = 0.088*angle +9.5;// 0.43*angle + 40
}

void SetConsigneServoBras(unsigned int angle)
{
    if(ENTREE_SELECTEUR_COULEUR == VIOLET)
        SetConsigneServoBrasDroit(angle);
    else
        SetConsigneServoBrasGauche(angle);
}

void SetConsigneServoBrasInactif(unsigned int angle)
{
    if(ENTREE_SELECTEUR_COULEUR == VIOLET)
        SetConsigneServoBrasGauche(angle);
    else
        SetConsigneServoBrasDroit(angle);
}


void SetConsigneServoPinceGauche(unsigned int angle)
{
    if(angle < 0)
        angle = 0;
    if(angle > 180)
        angle = 180;
    consigneServo3 = 0.1*angle + 15;// -0.5*angle + 75;
}

void SetConsigneServoPinceDroit(unsigned int angle)
{
    if(angle < 0)
        angle = 0;
    if(angle > 180)
        angle = 180;
    consigneServo4 = -0.1*angle + 16;  //0.5*angle + 75;
}

void SetConsigneServoPince(unsigned int angle)
{
    if(ENTREE_SELECTEUR_COULEUR == VIOLET)
        SetConsigneServoPinceDroit(angle);
    else
        SetConsigneServoPinceGauche(angle);
}

void SetConsigneServoPinceInactif(unsigned int angle)
{
    if(ENTREE_SELECTEUR_COULEUR == VIOLET)
        SetConsigneServoPinceGauche(angle);
    else
        SetConsigneServoPinceDroit(angle);
}

void SetConsigneServo5(unsigned int angle)
{
    if(angle < 0)
        angle = 0;
    if(angle > 180)
        angle = 180;
    consigneServo5 = (1+angle/180.0)*50;
}