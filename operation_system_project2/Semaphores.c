#include "Semaphores.h"

int createSem(int numOfSemaphores, int initValue)
{
	union semun arg;
	arg.val = initValue;
	int semid = semget(KEY_SEM, numOfSemaphores, 0600 | IPC_CREAT);
	if (semid < 0)
	{
		perror("semget");
		exit(0);
	}
	int i;
	for (i = 0; i < numOfSemaphores; i++)
		if (semctl(semid, i, SETVAL, arg) < 0)
		{
			perror("semctl");
			exit(0);
		}
	return semid;
}

void up(int semid, int numOfSem)
{
	unsigned short sem = numOfSem;
	struct sembuf oper = {sem, 1, 0};
	if (semop(semid, &oper, 1) < 0)
	{
		perror("semop");
		exit(0);
	}
}

void down(int semid, int numOfSem)
{
	unsigned short int sem = numOfSem;
	struct sembuf oper = {sem, -1, 0};
	if (semop(semid, &oper, 1) < 0)
	{
		perror("semop");
		exit(0);
	}
}

int accessSem(int numOfSemaphores)
{
	int semid = semget(KEY_SEM, numOfSemaphores, 0600);
	if (semid < 0)
	{
		perror("semget");
		exit(0);
	}
	return semid;
}

void destroySem(int semid)
{
	if (semctl(semid, 0, IPC_RMID, 0) < 0)
	{
		perror("semctl");
		exit(0);
	}
}


