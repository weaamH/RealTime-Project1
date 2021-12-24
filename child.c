#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define NUMBER_OF_CHILDREN 2

int randomNumber = 0;
char *writeFiles[NUMBER_OF_CHILDREN] = {"child_1.txt", "child_2.txt"};

void writeToFile(char *WriteFile, int sig)
{
	FILE *fileptr;
	fileptr = fopen(WriteFile, "w");

	// Catching file error.
	if (fileptr == NULL)
	{
		perror("fopen error\n");
	}
	
	// Generate random numbers.
	srand(time(NULL) % getpid());

	
	for (int i = 0; i < 10; i++)
	{
		randomNumber = rand() % 101;
		fprintf(fileptr, "%d\n", randomNumber);
	}

	if (fclose(fileptr))
	{
		perror("fclose error\n");
	}
	
	// Inform the parent.
	randomNumber = -1;
	kill(getppid(), sig);
}

void new_round(int signum)
{
	signal(SIGUSR1, new_round);
	randomNumber = 0;
}

int main(int argc, char **argv)
{
	if (signal(SIGUSR1, new_round) == SIG_ERR)
	{
		perror("handle error\n");
	}
	sleep(1);

	// Get arguments from parent.
	int childNum = atoi(argv[1]);
	while (1)
	{
		if (randomNumber != -1)
		{
			if (childNum == 1)
			{
				writeToFile(writeFiles[0], 2);
			}
			else if (childNum == 2)
			{
				writeToFile(writeFiles[1], 3);
			}
		}
		return;
	}
	return 0;
}
