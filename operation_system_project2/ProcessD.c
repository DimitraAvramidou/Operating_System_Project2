#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "SharedMemory.h"
#include "Semaphores.h"

void checkEnded(int pids[], int size);
int  checkAllEnded(int pids[], int size);

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("Wrong arguments. Give totalTicks and inputFilename.\n");
		return 0;
	}

	int N = 3;
	int i;
	int pids[N];
	char buf[10];

	//Create shared resources
	int semid = createSem(6, 1);
	for (i = 0; i < N; i++)
		down(semid, i);
	int shmid = createShmem(sizeof(SharedSlot) * 6);
	char* shmem = attachSharedMemory(shmid);
	bzero(shmem, sizeof(SharedSlot) * 6);
	printf("Created shmem and semaphore set.\n");

	//Create 3 child processes (A,B,C)
	for (i = 0; i < N; i++)
	{
		pids[i] = fork();
		switch(pids[i])
		{
		case -1:
			perror("fork");
			exit(0);
		case 0: //Child process
			sprintf(buf, "%d", i);
			execlp("./Processes", "Processes", argv[1], buf, argv[2], NULL);
			perror("execlp");
			exit(0);
		}
	}

	//Allow them to start
	for (i = 0; i < N; i++)
		up(semid, i);

	SharedSlot slot1, slot2;
	while(checkAllEnded(pids, N))
	{
		for (i = 0; i < N; i++)
		{
			if (pids[i] == 0)
				continue;
			down(semid, i+N);
			memcpy(&slot1, shmem+(i+N)*sizeof(slot1), sizeof(slot1));
			if (strlen(slot1.buf) != 0 && slot1.dest >= 0 && slot1.dest <= 2)
			{
				int flag = 1;
				while (flag)
				{
					down(semid, slot1.dest); //down j1
					memcpy(&slot2, shmem+(slot1.dest)*sizeof(slot2), sizeof(slot2)); //get j1 to slot2
					if (strcmp(slot2.buf, "end") == 0)
					{
						pids[slot1.dest] = 0;
						flag = 0;
					}
					else if (strlen(slot2.buf) == 0)
					{
						flag = 0;
						strcpy(slot2.buf, slot1.buf);
						memcpy(shmem+(slot1.dest)*sizeof(slot1), &slot2, sizeof(slot2));
					}
					up(semid, slot1.dest);
				}
				bzero(shmem+(i+N)*sizeof(slot1), sizeof(slot1));
			}
			up(semid, i+N);

			//Check exited processes
			checkEnded(pids, N);
		}
	}

	shmdt(shmem);
	destroyShmem(shmid);
	destroySem(semid);
	printf("Destroyed shmem and semaphore set.\n");

	return 0;
}

void checkEnded(int pids[], int size)
{
	int i, pid;
	for (i = 0; i < size; i++)
	{
		pid = waitpid(-1, NULL, WNOHANG);
		if (pid >= 0)
		{
			int j;
			for (j = 0; j < size; j++)
			{
				if (pids[j] == pid)
				{
					pids[j] = 0;
					return;
				}
			}
		}
	}
}

int checkAllEnded(int pids[], int size)
{
	int i, total=0;
	for (i = 0; i < size; i++)
		total += pids[i];
	return total;
}
