/* 
 * File:   TaskFermeturePortes.h
 * Author: Valentin
 *
 * Created on 4 mai 2016, 16:49
 */

#ifndef TASKFERMETUREPORTES_H
#define	TASKFERMETUREPORTES_H
#include "Task.h"
extern volatile TASK_BITS taskFermeturePortes;
void TaskFermeturePortes(void);
void TaskFermeturePortesInit();
void TaskFermeturePortesStart();
unsigned char TaskFermeturePortesIsFinished();

#endif	/* TASKFERMETUREPORTES_H */

