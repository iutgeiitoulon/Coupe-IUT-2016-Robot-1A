/* 
 * File:   main.h
 * Author: E105-POSTE2
 *
 * Created on 21 septembre 2015, 10:56
 */

#ifndef MAIN_H
#define	MAIN_H

#define GROS_ROBOT
//#define PETIT_ROBOT

#define VIOLET 0
#define VERT 1

#if defined(GROS_ROBOT) && defined(PETIT_ROBOT)
#error "On doit selectionner gros robot OU petit robot"
#endif

#define MOTEUR_DROIT 1
#define MOTEUR_GAUCHE 2
#define MOTEUR_ROULEAU 3
#define MOTEUR_3 3
#define MOTEUR_4 4

#define STATE_ATTENTE 0
#define STATE_ATTENTE_EN_COURS 1
#define STATE_AVANCE 2
#define STATE_AVANCE_EN_COURS 3
#define STATE_TOURNE_GAUCHE 4
#define STATE_TOURNE_GAUCHE_EN_COURS 5
#define STATE_TOURNE_DROITE 6
#define STATE_TOURNE_DROITE_EN_COURS 7
#define STATE_TOURNE_SUR_PLACE_GAUCHE 8
#define STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS 9
#define STATE_TOURNE_SUR_PLACE_DROITE 10
#define STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS 11
#define STATE_ARRET 12
#define STATE_ARRET_EN_COURS 13
#define STATE_RECULE 14
#define STATE_RECULE_EN_COURS 15
#define STATE_MATCH 90
#define STATE_MATCH_EN_COURS 91
#define STATE_FIN_MATCH 100
#define STATE_FIN_MATCH_EN_COURS 101

#define PAS_D_OBSTACLE 0
#define OBSTACLE_A_GAUCHE 1
#define OBSTACLE_A_DROITE 2
#define OBSTACLE_EN_FACE 3

#define DUREE_MATCH 90000

extern volatile unsigned char stateRobot;

void SystemStateMachine(void);
void SetNextRobotStateInAutomaticMode();
void SendState();

void SetRobotAutoControlState(char);
void SetRobotState(char);
void SendQeiData();

#endif	/* MAIN_H */

