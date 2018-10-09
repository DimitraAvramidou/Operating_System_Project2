#ifndef SEMAPHORES_H_
#define SEMAPHORES_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY_SEM 3333

/* Union for semaphores */
union semun
{
	int val;
	struct semid_ds *buff;
	unsigned short *array;
};

int createSem(int numOfSemaphores, int initValue);
void up(int semid, int numOfSem);
void down(int semid, int numOfSem);
int accessSem(int numOfSemaphores);
void destroySem(int semid);

#endif
