all:	ProcessD Processes

ProcessD: ProcessD.o SharedMemory.o Semaphores.o
	gcc -o ProcessD ProcessD.o SharedMemory.o Semaphores.o

ProcessD.o: ProcessD.c
	gcc -c ProcessD.c

SharedMemory.o: SharedMemory.c
	gcc -c SharedMemory.c

Semaphores.o: Semaphores.c
	gcc -c Semaphores.c

Processes.o: Processes.c
	gcc -c Processes.c

Processes: Processes.o SharedMemory.o Semaphores.o
	gcc -o Processes Processes.o SharedMemory.o Semaphores.o

clean:
	rm ProcessD Processes SharedMemory.o Semaphores.o ProcessD.o Processes.o

