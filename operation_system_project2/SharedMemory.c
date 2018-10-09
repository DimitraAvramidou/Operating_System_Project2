#include "SharedMemory.h"

int createShmem(int size)
{
	int shmid = shmget(KEY_SHMEM, size, 0600 | IPC_CREAT);
	if (shmid < 0)
	{
		perror("shmget");
		exit(0);
	}
	return shmid;
}

int accessShmem(int size)
{
	int shmid = shmget(KEY_SHMEM, size, 0600);
	if (shmid < 0)
	{
		perror("shmget");
		exit(0);
	}
	return shmid;
}

void destroyShmem(int shmid)
{
	int result = shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);
	if (result < 0)
	{
		perror("shmctl");
		exit(0);
	}
}

char* attachSharedMemory(int shmid)
{
	char* shmem = shmat(shmid, (char *) 0, 0);
	if (shmem == (char*)-1)
	{
		perror("shmat");
		exit(0);
	}
	return shmem;
}
