#include <p33FJ128MC804.h>
#include <stdio.h>
#include <stdlib.h>
#include "QEI.h"
#include "Asservissement.h"
#include <math.h>

double Position_finale_X;
double Position_finale_Y;
double Angle_final=atan((Position_finale_Y-PositionEnYQEI)/(Position_finale_X-PositionEnXQEI));
double new_X,new_Y,new_angle;

double erreur_pos_X = sqrt(pow(PositionEnXQEI-Position_finale_X,2));
double erreur_pos_Y = sqrt(pow(PositionEnYQEI-Position_finale_Y,2));
double erreur_angle = Angle_final-AngleQEI;

void calcul_trajectoire(void)
{
new_X=CorrecteurPosition(erreur_pos_X);
new_Y=CorrecteurPosition(erreur_pos_Y);
new_angle=CorrecteurAngle(erreur_angle);
}