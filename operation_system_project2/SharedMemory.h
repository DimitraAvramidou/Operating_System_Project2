#ifndef SHAREDMEMORY_H_
#define SHAREDMEMORY_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define KEY_SHMEM 4444
#define SIZE 2048

typedef struct SharedSlot
{
	int dest;
	char buf[SIZE];
}SharedSlot;

int createShmem(int size);
int accessShmem(int size);
void destroyShmem(int shmid);
char* attachSharedMemory(int shmid);

#endif
