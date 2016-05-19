#ifndef ROBOT_H
#define	ROBOT_H

#define VITESSE_ROULEAU_RECRACHE -75
#define VITESSE_ROULEAU_AVALE 100
#define VITESSE_ROULEAU_ARRET 0

#define DEFAULT_TASK_TIMOUT 4000

typedef struct robotStateBITS {

    union {

        struct {
            unsigned char taskEnCours;
            long timestampTaskStart;
            unsigned ModeAsservissement : 2;
            unsigned char matchState;
            unsigned char taskState;

            //Asservissement en vitesse
            double vitesseDroitFromOdometry;
            double vitesseDroitConsigne;
            double vitesseDroitSortieCorrecteur;

            double vitesseGaucheFromOdometry;
            double vitesseGaucheConsigne;
            double vitesseGaucheSortieCorrecteur;

            double vitesseMoteur3Consigne;
            double vitesseMoteur3SortieCorrecteur;
            double vitesseMoteur4Consigne;
            double vitesseMoteur4SortieCorrecteur;



            signed xPosTarget : 16;
            signed yPosTarget : 16;
            double angleRadianTarget;

            unsigned LidarSensitivity : 8;

            double xPosFromOdometry;
            double yPosFromOdometry;
            double xPosFromOdometry_1;
            double yPosFromOdometry_1;
            double angleRadianFromOdometry;
            double angleRadianFromOdometry_1;



            signed xPosFromBalise : 16;
            signed yPosFromBalise : 16;
            signed angleRadianFromBalise : 16;

            unsigned AligneAvantVersDirectionMouvement : 1;

            double GainVitesseLineaire;
            double GainVitesseRotation;
            double VitesseLimiteRotation;
            double VitesseLimiteLineaire;

            unsigned int distanceTelemetreCentre;
            unsigned int distanceTelemetreGauche;
            unsigned int distanceTelemetreDroit;
        };
    };
} ROBOT_STATE_BITS;
extern volatile ROBOT_STATE_BITS robotState;

#endif	/* ROBOT_H */

