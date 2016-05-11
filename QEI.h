/* 
 * File:   QEI.h
 * Author: E105-POSTE6
 *
 * Created on 25 janvier 2016, 15:05
 */
#include "main.h"

#ifndef QEI_H
#define	QEI_H


#define QEI_DATA 0x0060
#define POSITION_DATA 0x0061

#define FREQ_ECH_QEI 250.0

#ifdef PETIT_ROBOT
    #define distroues 181.6 //186.6
#elif defined(GROS_ROBOT)
    #define distroues 338//342.3
#endif

extern double QeiDroitPosition;
extern double QeiGauchePosition;

void InitQEI1();
void __attribute__((interrupt, no_auto_psv)) _QEI1Interrupt(void);
void InitQEI2();
void __attribute__((interrupt, no_auto_psv)) _QEI2Interrupt(void);
void QEIUpdateData();
void QEIReset();
void QEISetPosition(int posX, int posY, int angleDegre);
void SendQeiData();
void SendPositionData();


#endif	/* QEI_H */

