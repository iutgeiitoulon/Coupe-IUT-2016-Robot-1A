/* 
 * File:   AsservissementPosition.h
 * Author: E105-POSTE6
 *
 * Created on 17 mars 2016, 14:19
 */

#ifndef ASSERVISSEMENTPOLAIRE_H
#define	ASSERVISSEMENTPOLAIRE_H

#define ASSERVISSEMENT_DESACTIVE 0
#define ASSERVISSEMENT_VITESSE 1
#define ASSERVISSEMENT_POSITION 2
#define ASSERVISSEMENT_ANGLE 3

void SetUpPidAsservissementAngulaire(double Ku, double Tu);
void SetUpPiAsservissementAngulaire(double Ku, double Tu);
void SetUpPAsservissementAngulaire(double K);
double CorrecteurAngulaire(double e);
void SetUpPAsservissementLineaire(double K);
void SetUpPiAsservissementLineaire(double Ku, double Tu);
double CorrecteurLineaire(double e);
int* AsservissementAngle();
int* AsservissementPosition();

void SendTelemetrieAsservissementAngulaire();
#endif	/* ASSERVISSEMENTPOLAIRE_H */

