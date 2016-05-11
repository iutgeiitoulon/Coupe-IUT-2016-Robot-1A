/* 
 * File:   RobotManagement.h
 * Author: Valentin
 *
 * Created on 2 mai 2016, 15:14
 */

#ifndef ROBOTMANAGEMENT_H
#define	ROBOTMANAGEMENT_H

void SetRobotTargetPosition(int X, int Y);
void SetRobotTargetAngle(double angleDegre);
void SetRobotVitesseAsservie(signed char vitesseGauche, signed char vitesseDroit);
void SetRobotVitesseLibre(signed char vitesseGauche, signed char vitesseDroit);

#endif	/* ROBOTMANAGEMENT_H */

