/* 
 * File:   TaskVidageChateau.h
 * Author: Valentin
 *
 * Created on 3 mai 2016, 23:02
 */

#ifndef TASKVIDAGECHATEAU_H
#define	TASKVIDAGECHATEAU_H

#include "Task.h"

extern volatile TASK_BITS taskVidageChateau;

void TaskVidageChateauInit();
void TaskVidageChateau(void);
void TaskVidageChateauStart();
unsigned char TaskVidageChateauIsFinished();

#endif	/* TASKVIDAGECHATEAU_H */

