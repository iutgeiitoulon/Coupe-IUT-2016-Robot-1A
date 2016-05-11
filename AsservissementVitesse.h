/* 
 * File:   AsservissementVitesse.h
 * Author: E105-POSTE6
 *
 * Created on 3 mars 2016, 17:24
 */

#ifndef ASSERVISSEMENT_VITESSE_H
#define	ASSERVISSEMENT_VITESSE_H

extern double Te;

void SetUpPidAsservissementVitesseMoteur(double Ku, double Tu);
void SetUpPAsservissementVitesseMoteur(double K);
double CorrecteurVitesseDroit(double e);
double CorrecteurVitesseGauche(double e);
void ResetCorrecteurVitesseGauche();
void ResetCorrecteurVitesseDroit();

#endif	/* ASSERVISSEMENT_VITESSE_H */

