/****************************************************************************//*

	@file	lawsh.c

	@author	Malav Vyas

	@date	29 January 2019

******************************************************************************/

#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int lawsh_cd(char **args);
int lawsh_help(char **args);
int lawsh_exit(char **args);

char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char **) = {
		&lawsh_cd,
		&lawsh_help,
		&lawsh_exit
};

int lawsh_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

int lawsh_cd(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "lawsh: expected argument to \"cd\"\n");
	} else {
	if (chdir(args[1]) != 0) {
		perror("lawsh");
	}
}
return 1;
}

int lawsh_help(char **args)
{

int i;
printf("Malav Vyas's LawSH\n");
printf("Type program names and arguments, and hit enter.\n");
printf("The following are built in:\n");

for(i =0; i< lawsh_num_builtins(); i++) {
	printf("	%s\n", builtin_str[i]);
}

printf("Use the man for more information on other programs.\n");
return 1;
}

int lawsh_exit(char **args)
{
	return 0;
}

int lawsh_launch(char **args)
{
	pid_t	pid, wpid;
	int status;

	pid = fork();
	if( pid == 0) {
		if(execvp(args[0], args) == -1) {
			perror("lawsh");
	}
	exit(EXIT_FAILURE);
} else if (pid < 0) {
		perror("lawsh");
} else {
	do {
		wpid = waitpid(pid, &status, WUNTRACED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
}

return 1;
}

int lawsh_execute(char **args)
{
	int i;

	if(args[0] == NULL) {
		return 1;
}

for (i =0; i < lawsh_num_builtins(); i++) {
	if (strcmp(args[0], builtin_str[i]) == 0) {
		return (*builtin_func[i])(args);
	}
}

	return lawsh_launch(args);
}

#define LAWSH_RL_BUFSIZE 1024

char *lawsh_read_line(void)
{
	int bufsize = LAWSH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

if(!buffer) {
	fprintf(stderr, "lawsh: allocation error \n");
	exit(EXIT_FAILURE);
}

while(1) {

	c = getchar();

	if(c == EOF) {
		exit(EXIT_SUCCESS);
	} else if( c == '\n') {
		buffer[position] = '\0';
		return buffer;
	} else {
		buffer[position] = c;
	}
	position++;

	if(position >=bufsize) {
		bufsize += LAWSH_RL_BUFSIZE;
		buffer = realloc(buffer, bufsize);
		if(!buffer) {
			fprintf(stderr, "lawsh allocation error\n");
			exit(EXIT_FAILURE);
		}
	}
}
}

#define LAWSH_TOK_BUFSIZE 64
#define LAWSH_TOK_DELIM " \t\r\n\a"

char **lawsh_split_line(char *line)
{
	int bufsize = LAWSH_TOK_BUFSIZE, position =0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

if(!tokens) {
	fprintf(stderr, "lawsh: allocation error \n");
	exit(EXIT_FAILURE);
}

token = strtok(line, LAWSH_TOK_DELIM);
while(token != NULL) {
	tokens[position] = token;
	position++;

	if(position >= bufsize) {
		bufsize += LAWSH_TOK_BUFSIZE;
		token = realloc(tokens, bufsize * sizeof(char *));
		if(!tokens) {
			fprintf(stderr, "lawsh: allocation error \n");
			exit(EXIT_FAILURE);
}
}
token = strtok(NULL, LAWSH_TOK_DELIM);
}
tokens[position] = NULL;
return tokens;
}

void lawsh_loop(void)
{
	char *line;
	char **args;
	int status;

do{
	printf("> ");
	line = lawsh_read_line();
	args = lawsh_split_line(line);
	status = lawsh_execute(args);

	free(line);
	free(args);
}	while(status);
}



int main(int argc, char **argv)
{

lawsh_loop();

return EXIT_SUCCESS;
}