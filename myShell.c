#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
//includes the c libraries

int main (int argc, char ** argv){
	int loop = 0;
	char * input = malloc(sizeof(char) * 256); //allocates memory for the inputted command
	char * currToken; //will hold the current token
	char ** splitInput;
	int i = 0;
	int j = 0; //iterators
	int pid;
	int pid2; //will hold the process id
	int processStatus = 0; //will hold the status
	int commandNum = 0; //holds the number to be appended to the history file
	int size = 0; //holds the size of the file in bytes
	int fileOut = 0;
	int fileIn = 0;
	int pipeMe = 0;
	int cd = 0;
	int history = 0;
	int wait = 0; //flag that will determine if the process should wait
	char * fileOutName = malloc(sizeof(char) * 50);
	char * fileInName = malloc(sizeof(char) * 50);
	char ** pipeCommands = malloc(sizeof(char*) * 256); //allocates memory for commands after the pipe ("|")
	char * directory = malloc(sizeof(char) * 512);
	char * myHist = malloc(sizeof(char) * 256);
	char * currLine = malloc(sizeof(char) * 256);
	
	FILE* fp = fopen("CIS3110_history.txt", "a+"); //opens the history file for appending
	if (fp == NULL){
		printf("Error opening file\n");
		exit(-1);
	} //prints an error and exits if there was an error opening the file

	fseek (fp, 0, SEEK_END); //points to the end of the file
    size = ftell(fp); //stores the size of the file in bytes
	fseek(fp, 0, SEEK_SET); //points to the beginning of the file

    if (size != 0){ //if the file was not empty

    	while(!feof(fp)){
			fscanf(fp, "%[^\n]\n", currLine);
		} //scans the last line

		commandNum = atoi(strtok(currLine, " ")); //sets the command number to the number from the last line
    }

	getcwd(directory, 512); //gets the path of the current directory
	printf("%s> ", directory); //prints the current directory
	fgets(input, 256, stdin); //gets the command line input
	commandNum++;
	fprintf(fp, " %d  %s", commandNum, input);
	
	while (strcmp("exit\n", input) != 0){ //runs until the user exits

		input[strlen(input) - 1] = '\0'; //adds the null terminating character to the string
		currToken = strtok(input, " "); //tokenizes the input by the first space
		splitInput = malloc(sizeof(char*)); //allocates memory for one char*

		for(i = 0; currToken != NULL; i++){ //loops until there are no more commands
			splitInput = realloc(splitInput, sizeof(char*) * (i+2));
			splitInput[i] = malloc(sizeof(char) * strlen(currToken) + 1);
			//reallocates memory for the additional command
			splitInput[i] = currToken; //adds the command to the array
			currToken = strtok(NULL, " "); //gets the next inputted command
		}
		splitInput[i] = NULL; //sets the last space to null
		
		if ((strcmp("&", splitInput[i-1])) == 0) { //if the user wants to run a background command
            splitInput[i-1] = NULL; //replaces the "&" character with null
			wait = 1; //sets a wait flag to indicate this command is going to run
        }
		if(strcmp("cd", splitInput[0]) == 0){
			cd = 1;
		} //sets a flag if the user wants to change directory
		if(strcmp("history", splitInput[0]) == 0){
			history = 1;
		}

		for (i = 0; splitInput[i] != NULL; i++){ //loops until the end of the commands array
			if(strcmp(">", splitInput[i]) == 0){ //if the user wants to redirect output to a file
				fileOut = 1;
				strcpy(fileOutName, splitInput[i+1]); //copies the next string in the command array (which would be the file name)
				splitInput[i] = NULL;

				if(splitInput[i+2] != NULL){  //if there is more commands in the array
					continue; //continues running the loop
				}	
				else{ //if there is no more commands in the array
					break; //breaks out of the loop
				}
			}
			if(strcmp("<", splitInput[i]) == 0){
				fileIn = 1;
				strcpy(fileInName, splitInput[i+1]);
				splitInput[i] = NULL;
				if(splitInput[i+2] != NULL){
					continue;
				}	
				else{
					break;
				}
			}
			if(strcmp("|", splitInput[i]) == 0){ //if the user wants to pipe the output
				pipeMe = 1;
				int k = 0;
				for (j = i+1; splitInput[j] != NULL; j++){ //beginning at the command after the "|", loops until the end of the array
					pipeCommands[k] = malloc(sizeof(char) * strlen(splitInput[j]) + 1); //allocates memory for the command
					strcpy(pipeCommands[k], splitInput[j]);
					k++;
				}
				pipeCommands[k] = NULL;
				splitInput[i] = NULL;
			}
		}

		for (i = 0; pipeCommands[i] != NULL; i++){ 
			if(strcmp(">", pipeCommands[i]) == 0){
				fileOut = 1;
				strcpy(fileOutName, pipeCommands[i+1]);
				pipeCommands[i] = NULL;
				break;
			}
			if(strcmp("<", pipeCommands[i]) == 0){
				fileIn = 1;
				strcpy(fileInName, pipeCommands[i+1]);
				pipeCommands[i] = NULL;
				break;
			}
		} /*if there is another symbol after the pipe, repeats the process of replacing 
		the symbol, setting the flag, and copying the file name */

		if(cd == 1){ //if the user wants to change directory
			if(splitInput[1] == NULL){ //if just "cd" is typed
				chdir(getenv("HOME")); //switches the current directory to home
			}
			else if(strcmp(splitInput[1], "..") == 0 || strcmp(splitInput[1], "-") == 0){ //if the user wants to move up a directory
				chdir("..");
			}
			else{
				chdir(splitInput[1]); //if the user specifies a directory
			}
		}
		else if(history == 1){
			fclose(fp); //closes the file

			if(splitInput[1] == NULL){ //if they want to print the contents of the history file

				fp = fopen("CIS3110_history.txt", "r+"); //opens the file for reading
				int c = getc(fp); //gets the first character

				while (c != EOF) { //loop until the end of the file
					putchar(c); //prints the character
					c = getc(fp);
				}
				fclose(fp);
			}
			else if(strcmp(splitInput[1], "-c") == 0){
				fp = fopen("CIS3110_history.txt", "w+");
				fprintf(fp, " ");
				commandNum = 0;
				fclose(fp);
			}
			else{
				char * currLine = malloc(sizeof(char) * 256);
				fp = fopen("CIS3110_history.txt", "r+");
				int n = atoi(splitInput[1]); //converts the inputted line number to an int and stores it
				int lines = 0; //holds the number of lines

				fgets(currLine, 256, fp); //gets the next first line of the file
				while (!feof(fp)) { //loops until the end of the file
					lines++; //increments the line
					fgets(currLine, 256, fp); //gets the next line
				}

				int offset = lines - n; //subtracts the inputted number from the number of lines to get the offset
				if(offset < 0){ //if the number inputted exeeds the number of lines
					offset = 0; //sets the offset to 0 (beginning of the file)
				}
					fseek(fp, 0, SEEK_SET); //points to the begging of the file
					i = 0;
					fgets(currLine, 256, fp);	
					while (!feof(fp)) {
						if(i >= offset){ //if the current line is within the offset
							printf("%s", currLine); //prints it out
						}
						fgets(currLine, 256, fp);
						i++;
					}

				fclose(fp);
			}

			fp = fopen("CIS3110_history.txt", "a+");
		}
		else{
			pid = fork(); //forks a new process
			if(pid >= 0){
				if(pid == 0){
					if(pipeMe == 1){ //if the user wants to pipe the output
						int pipeArr[2]; //file descriptors array
						if(pipe(pipeArr) == -1){
							fprintf(stderr, "Error occured with opening the pipe\n");
							exit(-1);
						} //exits if there was an error opening the pipe
						pipe(pipeArr); //creates a pipe
						pid2 = fork();

						if(pid2 >= 0){
							if (pid2 == 0){
								close(pipeArr[0]);
								dup2(pipeArr[1], STDOUT_FILENO); //sends stdout to the write end of the pipe
								close(pipeArr[1]); //closes the write end of the pipe
								processStatus = execvp(splitInput[0], splitInput); //executes the file
								exit(processStatus); //exits the process
							}
						}
						else{
							perror("fork error\n");
							exit(-1);
						} //exits if there was a error forking the process
						
						FILE* fp;
						FILE* fp2; //creates a file pointer
						if(fileOut == 1){ //if the user wants to redirect output to a file
							fp = freopen(fileOutName, "w+", stdout); //opens the file inputted for writing the terminal output
							if (fp == NULL){
								printf("Error opening file\n");
								exit(-1);
							} 
						}

						if(fileIn == 1){
							fp2 = freopen(fileInName, "r+", stdin);
							if (fp == NULL){
								printf("Error opening file\n");
								exit(-1);
							} 
						}
						
						close(pipeArr[1]); //closes the write end of the pipe
						dup2(pipeArr[0], STDIN_FILENO); //creates a copy of the read file descriptor
						close(pipeArr[0]); //creates the write end of the pipe
						processStatus = execvp(pipeCommands[0], pipeCommands);
						fclose(fp);
						fclose(fp2); //closes the file pointer
						exit(processStatus);
					}
					else{
						FILE* fp;
						FILE* fp2;
						if(fileOut == 1){
							fp = freopen(fileOutName, "w+", stdout);
						}

						if(fileIn == 1){
							fp2 = freopen(fileInName, "r+", stdin);
						}
						
						processStatus = execvp(splitInput[0], splitInput);
						fclose(fp);
						fclose(fp2);
						exit(processStatus);		
					}
				}
				else{
					if(wait == 1){
						waitpid(0, &processStatus, WNOHANG);
					}
					else{
						waitpid(pid, &processStatus, 0);
						if (WEXITSTATUS(processStatus) == -1){
							fprintf(stderr, "Error running %s\n", argv[0]);
						}
					}
				}
			}
			else{
				perror("fork error\n");
				exit(-1);
			}
		}

		for(i = 0; splitInput[i] == NULL; i++){
			free(splitInput[i]);
		}
		free(splitInput); //frees each command in the array and the array itself

		free(input);

		input = malloc(sizeof(char *) * 256);
		getcwd(directory, 512);
		printf("%s> ", directory);
		fgets(input, 256, stdin);

		commandNum++;
		fprintf(fp, " %d  %s", commandNum, input);

		fileIn = 0;
		fileOut = 0;
		pipeMe = 0;
		wait = 0;
		cd = 0;
		history = 0; //resets the flags
	}//while

    printf("myShell Terminating...\n");
	
	for(i = 0; pipeCommands[i] == NULL; i++){
		free(pipeCommands[i]);
	}
	free(pipeCommands);

	free(input);
	free(directory);
	free(fileOutName);
	free(fileInName);
	free(myHist);
	free(currLine); //frees the allocated memory

	fclose(fp);

	kill(0, SIGTERM); //kills the processes
	exit(0); //exits
	
	return 0;
}
