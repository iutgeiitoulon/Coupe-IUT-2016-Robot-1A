/* 
 * File:   TaskVidageChateau.h
 * Author: Valentin
 *
 * Created on 3 mai 2016, 23:02
 */

#ifndef TASKPECHEPOISSON_H
#define	TASKPECHEPOISSON_H
#include "Task.h"
extern volatile TASK_BITS taskPechePoisson;
void TaskPechePoisson(void);
void TaskPechePoissonInit();
void TaskPechePoissonStart();
unsigned char TaskPechePoissonIsFinished();

void BaisseBras();

#endif	/* TASKPECHEPOISSON_H */

