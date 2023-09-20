///////////////////////////////////////////////////////////////////////////
//
//	File Name:	shellex.c
//	Project Name:	Project 3
//	Description:	The shell program executes the input commands with a terminal or by an input from a file.
//	Last Edit:	12/10/2022
//	Student Name:	Feyza Sakin
//
//////////////////////////////////////////////////////////////////////////

/* $begin shellmain */
#include "csapp.h"
#include <signal.h>
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 

int main(int argc, char *argv[]) 
{
	char cmdline[MAXLINE]; /* Command line */

	/*	The signal handler to prevent CTRL+C or SIGINT signal to take out of the shell	*/
	signal(SIGINT, SIG_IGN);

	while (1) {

		/* Read */
		/* Set the command prompt 'sh257> '	*/
		if(argc == 1){
			printf ("sh257> ");	
		}
		else{
			/*	Change the command prompt to third word in the input when '-p' option is used	*/
			if((strcmp(argv[1], "-p"))==0){
				printf ("%s> ", argv[2]);
			}
			else{
				return 0;
			}
		}

		Fgets(cmdline, MAXLINE, stdin); 
		if (feof(stdin)){
			exit(0);
		}

		/* Evaluate */

		eval(cmdline);

	} 
}
/* $end shellmain */

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
	char *argv[MAXARGS]; /* Argument list execve() */
	char buf[MAXLINE];   /* Holds modified command line */
	int bg;              /* Should the job run in bg or fg? */
	pid_t pid;           /* Process id */

	strcpy(buf, cmdline);
	bg = parseline(buf, argv); 
	if (argv[0] == NULL)  
		return;   /* Ignore empty lines */

	if (!builtin_command(argv)) { 

		if ((pid = Fork()) == 0) {   /* Child runs user job */
			if (execvp(argv[0], argv) < 0) {
				/* Print 'Execution failed (in fork)' when it's failed	*/
				printf("Execution failed (in fork)\n");
				printf("%s: Command not found.\n", argv[0]);
				/*	exit(1) since it's unsuccesful	*/
				exit(1);
			}

		}

		/* Parent waits for foreground job to terminate */
		if (!bg) {
			int status;
			if (waitpid(pid, &status, 0) < 0){
				unix_error("waitfg: waitpid error");	
				//at the exit we need to print out the status
			}
			else{
				printf ("Process exited with status code %d\n", WEXITSTATUS(status));
			}	
		}
		else{
			printf("%d %s", pid, cmdline);
		}
	}
	return;
}


/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
	/*	Exit when the input is 'exit'	*/	
	if (!strcmp(argv[0], "exit")){ /* exit command */
		raise(SIGTERM);
	}

	/*	Print the process ID when the input is 'pid' 	*/
	if (!strcmp(argv[0], "pid")){
		printf("%d\n", getpid());
		return 1;
	}

	/*	Print the parent process ID when the input is 'ppid'	*/
	if (!strcmp(argv[0], "ppid")){
		printf("%d\n", getppid());
		return 1;
	}

	/*	When the input is 'help', print the developer's name, the usage info, and refering user to get help.*/
	if (!strcmp(argv[0], "help")){
		printf ("**********************************************************************\n");
		/*	Print the developer's name	*/
		printf ("\nA Custom Shell for CMSC 257\n");
		printf ("	- Feyza Sakin\n");

		/*	Print the usage info	*/
		printf ("Usage:\n");
		/*	How to change the prompt	*/
		printf ("	- './sh257 -p <new_prompt>' To change the prompt\n");
		printf ("**********************************************************************\n\n");
		/*	Information about the bulletin commands	*/
		printf ("BULLETIN COMMANDS:\n");
		printf ("	- 'exit'	exit the shell\n");
		printf ("	- 'pid'		print the shell's process ID\n");
		printf ("	- 'ppid'	print the shell's parent process ID\n");
		printf ("	- 'help'	print the shell's developer name\n");
		printf ("	  		print the usage information (including changing shell prompt and the list of built-in commands)\n");
		printf ("			how to look for non-built-in commands\n\n");
		printf ("	- 'cd'		print the current working directory\n");
		printf ("	- 'cd <path>'	change the directory to the path\n");

		/*	Information about non-built-in commands	*/
		printf ("SYSTEM COMMANDS:\n");
		printf ("	- Try 'man' pages for non-built-in commands\n\n");
		return 1;
	}

	/*	Print the path if the input is 'cd'	*/
	if ((!strcmp(argv[0], "cd"))){
		if(argv[1] == NULL){
			char s[100];
			getcwd(s, sizeof(s));
			printf ("%s\n", s);
			return 1;
		}
		else{
			/*	Change the directory if the input is 'cd <path>'	*/
			chdir(argv[1]);
			return 1;
		}
	}

	return 0;

	/* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
	char *delim;         /* Points to first space delimiter */
	int argc;            /* Number of args */
	int bg;              /* Background job? */

	buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
	while (*buf && (*buf == ' ')) /* Ignore leading spaces */
		buf++;

	/* Build the argv list */
	argc = 0;
	while ((delim = strchr(buf, ' '))) {
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while (*buf && (*buf == ' ')) /* Ignore spaces */
			buf++;
	}
	argv[argc] = NULL;

	if (argc == 0)  /* Ignore blank line */
		return 1;

	/* Should the job run in the background? */
	if ((bg = (*argv[argc-1] == '&')) != 0)
		argv[--argc] = NULL;

	return bg;
}
/* $end parseline */

