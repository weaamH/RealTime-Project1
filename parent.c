#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define NUM_OF_CHILDREN 3
#define MSGSIZE 30

int recieved_responses = 0;

int children_PID[NUM_OF_CHILDREN];

int f_des_Parent[2];  // pipe of parent
int f_des_Referee[2]; // pipe of referee

char refereeInformationMessage[MSGSIZE] = "child_1.txt-child_2.txt";
char roundResult[MSGSIZE];

int roundScore_1;
int roundScore_2;

int bigScore_1 = 0;
int bigScore_2 = 0;

int roundNumber = 0;

char winner[20] = "Draw.";

void signalINT(int sig)
{
	signal(SIGINT, signalINT);
	recieved_responses++;
}

void signalQUIT(int sig)
{
	signal(SIGQUIT, signalQUIT);
	recieved_responses++;
}

void informReferee()
{
	if (write(f_des_Parent[1], refereeInformationMessage, MSGSIZE) != -1)
	{
		printf("\033[0;36mMessage sent by parent:\033[0m[%s]\n", refereeInformationMessage);
		fflush(stdout);
	}
	else
	{
		perror("Write");
		exit(5);
	}
}

void readRoundResultFromRefereePipe()
{

	if (read(f_des_Referee[0], roundResult, MSGSIZE) != -1)
	{
		printf("\033[1;33mParent Recieved the round result from the referee:\033[0m[%s]\n", roundResult);
		fflush(stdout);
	}
	else
	{
		perror("Read");
		exit(4);
	}
	// Fill round scores.
	parseRoundResults();
}

void parseRoundResults()
{
	const char DELIMA[2] = "-";
	roundScore_1 = atoi(strtok(roundResult, DELIMA));
	roundScore_2 = atoi(strtok(NULL, DELIMA));
}

void increaseBigScores()
{
	bigScore_1 += roundScore_1;
	bigScore_2 += roundScore_2;
	printf("Big score 1 = %d \t Big score 2 = %d\n", bigScore_1, bigScore_2);
}

void checkWinner()
{
	if ((bigScore_1 == 50 && bigScore_2 == 50) ||
		((bigScore_1 == bigScore_2) && (bigScore_1 > 50) && (bigScore_2 > 50)))
	{
		printf("\n\t\t**********************************");
		printf("\n\t\t\t\t\t%s\n", winner);
		printf("\n\t\t**********************************");
		printAuthors();
		terminate();
	}
	else if (bigScore_1 >= 50)
	{
		strcpy(winner, "Player 1");
		// exit referee
		printf("\n\t\t**********************************");
		printf("\n\t\t\tThe winner is:\033[0;32m%s\033[0m", winner);
		printf("\n\t\t**********************************");
		printAuthors();
		terminate();
	}
	else if (bigScore_2 >= 50)
	{
		strcpy(winner, "Player 2");
		// exit referee.
		printf("\n\t\t**********************************");
		printf("\n\t\t\tThe winner is:\033[0;32m%s\033[0m", winner);
		printf("\n\t\t**********************************\n");
		printAuthors();
		terminate();
		
	}
	else
	{
		roundNumber++;
		recieved_responses = 0;

		kill(children_PID[0], SIGUSR1);
		kill(children_PID[1], SIGUSR1);
	}
}

void printAuthors(){
printf("\n\nAuthors:\n\nIsraa Haseeba - 1182467\nWeaam Hjijah - 1181988\nSiham Abu Rmaileh - 1180548\n");
}

void terminate()
{
	for (int i = 0; i < NUM_OF_CHILDREN; i++)
	{
		kill(children_PID[i], SIGKILL);
	}
	exit(EXIT_SUCCESS);
}

int main()
{
	if (signal(SIGQUIT, signalQUIT) == -1)
	{
		perror("Error in catching SIGQUIT\n");
	}
	if (signal(SIGINT, signalINT) == -1)
	{
		perror("Error in catching SIGINT\n");
	}
	// Parent PIPE.
	if (pipe(f_des_Parent) == -1)
	{
		perror("Parent Pipe ERR\n");
		exit(2);
	}
	// Referee PIPE.
	if (pipe(f_des_Referee) == -1)
	{
		perror("Referee Pipe ERR\n");
		exit(2);
	}
	while (1)
	{
		// Create players (children)
		for (int i = 0; i < NUM_OF_CHILDREN; i++)
		{
			pid_t pid = fork();
			children_PID[i] = pid;

			if (pid == 0)
			{
				if (i == 0)
				{
					execlp("./child", "./child", "1", NULL);
				}
				else if (i == 1)
				{
					execlp("./child", "./child", "2", NULL);
				}
				else if (i == 2)
				{
					// Convert integers into strings.
					char read_des_Parent[2];
					sprintf(read_des_Parent, "%d", f_des_Parent[0]);
					char write_des_Parent[2];
					sprintf(write_des_Parent, "%d", f_des_Parent[1]);
					char read_des_Referee[2];
					sprintf(read_des_Referee, "%d", f_des_Referee[0]);
					char write_des_Referee[2];
					sprintf(write_des_Referee, "%d", f_des_Referee[1]);

					execlp("./referee", "ls", read_des_Parent, write_des_Parent,
						   read_des_Referee, write_des_Referee, NULL);
				}
			}
			else
			{
				sleep(2);
			}
		}
		if (recieved_responses == 2)
		{
			// Players are done.
			printf("\n\n\n\033[0;34mPlayers are ready.\033[0m\n");
			printf("This is round (%d)\n", roundNumber);
			printf("Inform the referee.\n");
			informReferee(); // Writes on parent pipe.
			readRoundResultFromRefereePipe();
			increaseBigScores();
			checkWinner();
		}
	}
	return 0;
}
