#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
// int sigint=0;
int background_proc[64];

void handle_sig(int sig){
	// continue;
	// printf("$\n");
	// int r = 1;
	printf("\n");
	// printf("$");
	// printf("\n$");
	// sigint=1;
}

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

void handlebg(char* line){
	char* command = strtok(line,"&");
	char** tokens = tokenize(command);
	int ret = fork();
		if(ret < 0) printf("Shell: Error in Fork\n");

		else if(ret==0){
			setpgid(0,0);
			int p = execvp(tokens[0],tokens);
			if(p==-1){
				printf("Shell: Incorrect Command\n");
				exit(0);
			}
		}


		else if(ret>0){
			// int k = waitpid(ret,NULL,0);
			for(int i = 0; i < 64; i++){
				if (background_proc[i] == -1){
					background_proc[i] = ret;
					break;
				}
			}
		}
}


int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
	int i;
	char *lastcommand = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));	
	strcpy(lastcommand,"Shell: Last command not found\n");
	for(i=0; i<64; i++) background_proc[i]=-1;
	signal(SIGINT,handle_sig);

	while(1) {

		for(i=0;i<64;i++){
			if (background_proc[i]>0){
				int l = waitpid(background_proc[i],NULL,WNOHANG);
				if(l==background_proc[i]){
					background_proc[i] = -1;
					printf("Shell: Background process finished\n");
				}
			}
		}			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		// interactive mode
		printf("$ ");
		scanf("%[^\n]", line);
		getchar();
		
		//DEBUG
		// printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; //terminate with new line
		tokens = tokenize(line);
		if(tokens[0]==NULL){
			free(tokens[0]);
			free(tokens);
			continue;
		}

		int background=0;
		for(i=0;tokens[i]!=NULL;i++){
			if(!strcmp(tokens[i],"&")) background=1;
		}

		if (!strcmp(tokens[0],"exit")){
			strcpy(lastcommand,"exit");
			// lastcommand = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
			// for(i=0;line[i]!='\0';i++) lastcommand[i]=line[i];
			for(i=0;i<64;i++){
				if(background_proc[i]>-1){
					kill(background_proc[i],SIGKILL); //Check the order of arguments
					background_proc[i] = -1;
					// printf("Shell: Background process killed\n"); //DEBUG
				}
			}

			for(i=0;tokens[i]!=NULL;i++){
			  free(tokens[i]);
			}
			free(tokens);

			// printf("Exiting\n"); //DEBUG

			exit(0);

		}

		else if(!strcmp(tokens[0],"cd")){
			strcpy(lastcommand,line);
			// lastcommand = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
			// for(i=0;line[i]!='\0';i++) lastcommand[i]=line[i];
			if(tokens[2]!=NULL) printf("Shell: Incorrect command\n");
			else {
				int l = chdir(tokens[1]);
				if(l==-1) printf("Shell: Directory not found\n");
			}
		}

		else if(!strcmp(tokens[0],"lastcmd")){
			printf("%s",lastcommand);
			strcpy(lastcommand,"lastcmd\n");
		}

		else if (background==1){
			strcpy(lastcommand,line);
			// lastcommand = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
			// for(i=0;line[i]!='\0';i++) lastcommand[i]=line[i];
			handlebg(line);
		}

		else{
			strcpy(lastcommand,line);
			// lastcommand = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
			// for(i=0;line[i]!='\0';i++) lastcommand[i]=line[i];
			int ret = fork();
			if(ret < 0) printf("Shell: Error in fork\n");

			else if(ret==0){
				int p = execvp(tokens[0],tokens);
				if(p==-1){
					printf("Shell: Incorrect command\n");
					exit(0);
				}
			}


			else if(ret>0){
				int k = waitpid(ret,NULL,0);
			}

		}


		
		//do whatever you want with the commands, here we just print them
		
		//DEBUG
		// for(i=0;tokens[i]!=NULL;i++){
		//   printf("found token %s (remove this debug output later)\n", tokens[i]);
		// }
		
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++){
		  free(tokens[i]);
		}
		free(tokens);
		
	}
	return 0;
}
