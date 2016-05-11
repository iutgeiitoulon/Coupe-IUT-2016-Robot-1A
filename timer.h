/* 
 * File:   timer.h
 * Author: E105-POSTE2
 *
 * Created on 8 septembre 2015, 14:22
 */

#ifndef TIMER_H
#define	TIMER_H

extern unsigned long timestamp ;

void InitTimer1(void);
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void);
void SetFreqTimer1(float);
void InitTimer2(void);
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);
void SetFreqTimer2(float);
void InitTimer3(void);
void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void);
void SetFreqTimer3(float);
void InitTimer4(void);
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void);
void SetFreqTimer4(float);

#endif	/* TIMER_H */

