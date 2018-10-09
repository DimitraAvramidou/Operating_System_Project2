#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SharedMemory.h"
#include "Semaphores.h"

void writeRandLine(char* buf, FILE* out, int);
int totalLinesOfFiles(FILE* in);

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		printf("Don't execute this program, only processD.\n");
		return 0;
	}

	int n;
	int sent=0, received=0;
	FILE *in, *out;
	char buf[10];
	int totalTicks = atoi(argv[1]);
	n = atoi(argv[2]);
	char* filename = argv[3];

	//Access shared resources
	int semid = accessSem(6);
	int shmid = accessShmem(sizeof(SharedSlot) * 6);
	char* shmem = attachSharedMemory(shmid);
	printf("Accessed shmem and semaphore set.\n");

	printf("Opening file %s for read...\n", filename);
	in = fopen(filename, "r");
	if (!in)
	{
		perror("fopen in");
		return 0;
	}
	int totalLines = totalLinesOfFiles(in);
	
	sprintf(buf, "out%d.txt", n);
	printf("Opening file %s for write...\n", buf);
	out = fopen(buf, "w");
	if (!out)
	{
		perror("fopen out");
		return 0;
	}

	srand(getpid() * n);
	SharedSlot slot;
	int currentClock = 0;
	while(currentClock++ < totalTicks)
	{
		//check for received message
		down(semid, n);
		memcpy(&slot, shmem+(n)*sizeof(slot), sizeof(slot));
		if (strlen(slot.buf) != 0)
		{
			fprintf(out, "Received: %s\n", slot.buf);
			received+=1;
			bzero(shmem+(n)*sizeof(slot), sizeof(slot));
		}
		up(semid, n);

		//rand probability
		int r = rand() % 100 + 1;
		if (r < 30) //sent message with probability 30%
		{
			down(semid, n+3);
			memcpy(&slot, shmem+(n+3)*sizeof(slot), sizeof(slot));
			if (strlen(slot.buf) == 0)
			{
				sent++;
				writeRandLine(slot.buf, in, totalLines);
				slot.dest = rand() % 3;
				fprintf(out, "Sent: %s\n", slot.buf);
				memcpy(shmem+(n+3)*sizeof(slot), &slot, sizeof(slot));
			}
			up(semid, n+3);
		}

		usleep(rand() % 100);
	}

	//Write end to 1st part
	down(semid, n);
	strcpy(slot.buf, "end");
	memcpy(shmem+(n)*sizeof(slot), &slot, sizeof(slot));
	up(semid, n);

	fclose(in);
	fclose(out);
	shmdt(shmem);
	printf("Destroyed shmem and semaphore set.\n");
	printf("Process %d ends, sent: %d, received: %d\n", n, sent, received);
	return 0;
}

int totalLinesOfFiles(FILE* in)
{
	int counter = 0;
	char buf[SIZE];
	while(!feof(in))
	{
		counter++;
		fgets(buf, SIZE, in);
	}
	return counter;
}

void writeRandLine(char* buf, FILE* in, int totalLines)
{
	rewind(in);
	int i;
	int lineToGet = rand() % totalLines + 1;
	for (i = 0; i < lineToGet; i++)
	{
		bzero(buf, SIZE);
		fgets(buf, SIZE, in);
	}
	int len = strlen(buf);
	if (len)
		buf[len-1] = '\0';
}
