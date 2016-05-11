/* 
 * File:   TaskVidageChateau.h
 * Author: Valentin
 *
 * Created on 3 mai 2016, 23:02
 */

#ifndef TASKAvalageChateau_H
#define	TASKAvalageChateau_H

#include "Task.h"

extern volatile TASK_BITS taskAvalageChateau;

void TaskAvalageChateauInit();
void TaskAvalageChateauStart(int positionY);
void TaskAvalageChateau(void);
unsigned char TaskAvalageChateauIsFinished();

#endif	/* TASKVIDAGECHATEAU_H */

