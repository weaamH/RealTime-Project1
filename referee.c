#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define NUM_OF_PLAYERS 2
#define MSGSIZE 30
#define COUNT_OF_RANDOM_NUMBERS 10

char playerFile[NUM_OF_PLAYERS][20];
char strOfFilesNames[MSGSIZE];

int firstPlayerData[COUNT_OF_RANDOM_NUMBERS];
int secondPlayerData[COUNT_OF_RANDOM_NUMBERS];

int score_1 = 0, score_2 = 0;

char roundWinner[4];

// Descriptors of pipes.
int read_des_Parent;
int write_des_Parent;
int read_des_Referee;
int write_des_Referee;

void parseFilesNames()
{
  const char DELIMA[2] = "-";
  strcpy(playerFile[0], strtok(strOfFilesNames, DELIMA));
  strcpy(playerFile[1], strtok(NULL, DELIMA));
}

void readFile(char playerFile[20], int data[10])
{
  FILE *read_file = fopen(playerFile, "r");
  int num, i = 0;

  if (read_file == NULL)
  {
    perror("Can't open file\n");
  }

  while (fscanf(read_file, "%d", &num) == 1)
  {
    data[i] = num;
    i++;
  }
}

void printData(){
    printf("    Player One\t   Playes two:\n");
    for(int i=0; i<10; i++){
       printf("\t%d\t\t%d\n", firstPlayerData[i], secondPlayerData[i]);
    }
    printf("\n");
}
void compareResults()
{
  for (int i = 0; i < 10; i++)
  {
    if (firstPlayerData[i] > secondPlayerData[i])
    {
      score_1++;
    }
    else if (firstPlayerData[i] < secondPlayerData[i])
    {
      score_2++;
    }
  }

  // Convert integers into strings.
  char score1_str[5], score2_str[5];
  sprintf(score1_str, "%d", score_1);
  sprintf(score2_str, "%d", score_2);

  // Create message to send it to parent.
  strcat(score1_str, "-");
  strcat(score1_str, score2_str);
  strcpy(roundWinner, score1_str);
  printf("Round socres:   %d \t %d\n", score_1, score_2);
}

void informParentByResult()
{
  if (write(write_des_Referee, roundWinner, MSGSIZE) != -1)
  {
    printf("\033[0;36mResult sent by referee to the parent:\033[0m [%s]\n", roundWinner);
    fflush(stdout);
  }
  else
  {
    perror("Write\n");
    exit(5);
  }
  
  // Reset scores.
  score_1 = 0;
  score_2 = 0;
}

void readFileNamesFromParentPipe()
{
  if (read(read_des_Parent, strOfFilesNames, MSGSIZE) != -1)
  {
    printf("\033[1;33mReferee received files names from parent:\033[0m [%s]\n", strOfFilesNames);
    fflush(stdout);
  }
  else
  {
    perror("Read\n");
    exit(4);
  }
}

int main(int argc, char **argv)
{
  while (1)
  {
    read_des_Parent = atoi(argv[1]);
    write_des_Parent = atoi(argv[2]);
    read_des_Referee = atoi(argv[3]);
    write_des_Referee = atoi(argv[4]);

    // Get files names.
    readFileNamesFromParentPipe();

    // Seperate names.
    parseFilesNames();

    // Get data from files.
    readFile(playerFile[0], firstPlayerData);
    readFile(playerFile[1], secondPlayerData);
    
    printData();

    // Find the winner of the round.
    compareResults();

    remove(playerFile[0]);
    remove(playerFile[1]);

    // Send result to the parent.
    informParentByResult();
  }

  return 0;
}
