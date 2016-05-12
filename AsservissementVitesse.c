#include <p33FJ128MC804.h>
#include <stdio.h>
#include <stdlib.h>
#include "QEI.h"

double Te = 1.00 / FREQ_ECH_QEI;

double ed0, ed1, ed2; //valeurs de l'entrée du correcteur droit à t, t-1, t-2
double sd0, sd1, sd2; //valeurs de la sortie du correcteur droit à t, t-1, t-2
double eg0, eg1, eg2; //valeurs de l'entrée du correcteur gauche à t, t-1, t-2
double sg0, sg1, sg2; //valeurs de la sortie du correcteur gauche à t, t-1, t-2

double alpha;
double beta;
double delta;

void SetUpPidAsservissementVitesseMoteur(double Ku, double Tu)
{
    //Réglage de Ziegler Nichols classique
//    double Kp = 0.6 * Ku;
//    double Ti = 0.5 * Tu;
//    double Td = 0.125 * Tu;
    
    //Réglage de Ziegler Nichols sans dépassements : un tout petit peu mou
    double Kp = 0.2 * Ku;
    double Ti = 0.5 * Tu;
    double Td = 0.33 * Tu;
    
    //Réglage de Ziegler Nichols faible dépassements : limite de stabilité
//    double Kp = 0.33 * Ku;
//    double Ti = 0.5 * Tu;
//    double Td = 0.33 * Tu;
    
    //Réglage de Ziegler Nichols par la règle de Pessen intégrale : très peu stable
//    double Kp = 0.7 * Ku;
//    double Ti = 0.4 * Tu;
//    double Td = 0.15 * Tu;
        
    double Ki = Kp / Ti;
    double Kd = Kp * Td;
    alpha = Kp + Ki * Te + Kd / Te;
    beta = -Kp - 2 * Kd / Te;
    delta = Kd / Te;
    
    //Autre formulation équivalente
//    alpha = Kp*(1+Te/Ti+Td/Te);
//    beta = Kp*(-1-2*Td/Te);
//    delta = Kp*Td/Te;
}

void SetUpPAsservissementVitesseMoteur(double K)
{    
    //Réglage de Ziegler Nichols sans dépassements : un tout petit peu mou
    double Kp = K;        
    double Ki = 0;
    double Kd = 0;
    alpha = Kp + Ki * Te + Kd / Te;
    beta = -Kp - 2 * Kd / Te;
    delta = Kd / Te;
    
    //Autre formulation équivalente
//    alpha = Kp*(1+Te/Ti+Td/Te);
//    beta = Kp*(-1-2*Td/Te);
//    delta = Kp*Td/Te;
}

double CorrecteurVitesseDroit(double e)
{
    ed2 = ed1;
    ed1 = ed0;
    ed0 = e;
    sd1 = sd0;
    sd0 = sd1 + ed0 * alpha + ed1 * beta + ed2*delta;
    return sd0;
}

double CorrecteurVitesseGauche(double e)
{
    eg2 = eg1;
    eg1 = eg0;
    eg0 = e;
    sg1 = sg0;
    sg0 = sg1 + eg0 * alpha + eg1 * beta + eg2*delta;
    return sg0;
}

void ResetCorrecteurVitesseGauche()
{
//    eg2 = 0;
//    eg1 = 0;
//    eg0 = 0;
    sg1 = 0;
    sg0 = 0;
}

void ResetCorrecteurVitesseDroit()
{
//    ed2 = 0;
//    ed1 = 0;
//    ed0 = 0;
    sd1 = 0;
    sd0 = 0;
}