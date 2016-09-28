/*
 * A Simple Shell
 * Author: Geoff Whitehead
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <errno.h>

/*
* TRUE : used to set a neverending loop in the main
* size: used to specify the number of chars operations such as allocating memory. increasing this will allow the user to enter longer strings of commands and parameters at prompt.
* args: used to specify the number of array indexes to create. increasing this will allow more parameters when entering commands at promt.
*/
#define TRUE 1
#define size 256
#define args 32

extern int errno;
/*
* prompt()
* Description: display the shell promt. Contains the user, the logged in user, the TERM (OS) and the current working directory.
* Param: null
* Returns: null
*/
void prompt() {
        char *cwd;
        char buffer[PATH_MAX + 1];
        cwd = getcwd (buffer, PATH_MAX + 1);
        printf("%s@%s-%s:%s$ ", getenv("USER"),getenv("LOGNAME"),getenv("TERM"),cwd);
}

/*
* exec(char **args)
* Description: forkes a child processes. Performs error checking to ensure successful creation. initiates execution in child process whilst parent waits for completion. Uses execvp to search PATH environment variable automatically rather than specifying (as with execve).
* Param: char **argv: a pointer to a pointer of strings containing the command at index[0] and the parameters in the rest of the array (NULL terminated). 
* Returns: null
*/
void exec(char **argv) {
        pid_t pid;
        int status;
        int err_id;
        
        if ( ( pid = fork() ) <0 ) {
	        err_id = errno;
		fprintf(stderr,"\terror id %d : %s\n",errno, strerror( err_id ));
	        exit(1);
	}
        else if (pid == 0) {
	        if (execvp(*argv, argv) < 0) {
		        err_id = errno;
		        fprintf(stderr,"\terror id %d : %s\n",errno, strerror( err_id ));
		        exit(1);
		}
	}
        else {
	        while (wait(&status) != pid) ;
	}
}


/*
* split(char *line, char **newargv)
* Description: Tokenises the string based on the delimeters " " and "\n". polulates an array with the tokens.
* Param: char *line: a pointer to a string, char **argv: a pointer to an array of pointers to strings. Used to store the tokenised string.
* Returns: null
*/

void split(char *line, char**newargv){
        char *token;
        const char delim[] = " \n\t";
        
        token = strtok(line, delim);
        newargv[0] = token;
        
        int i = 1;  
       
        while( token != NULL ) {
                token = strtok(NULL, delim);
                if (token != NULL) {
                        newargv[i] = token;
                        i++;
                }
        }
        newargv[i] = NULL;
}

/*
* help()
* Description: print block containing useful information for using the shell.
* Param: null.
* Returns: null
*/

void help () {
        puts("\n\n\tbelow you will find a short list of some common commands. To access more detailed help information type -help or use the man command. Commands will be run in current working directory when not specifying a path\n");
        puts("\t\t pwd - used to find your current working directory");
        puts("\t\t cd [path] - used to change current working directory");
        puts("\t\t\t cd - sets cwd to home directory");
        puts("\t\t\t cd [full path] - sets current working directory to path name");
        puts("\t\t\t cd .. - sets cwd to previous directory");
        puts("\t\t mkdir [directory name] - create a folder");
        puts("\t\t rmdir [directory name] - remove a folder");
        puts("\t\t mv [path][name] [path][name] - move a folder, this can also be used to rename a folder by specifying a different name in the 2nd set of parameters.");
        puts("\t\t exit - exits the shell");
        puts("\t\t which [command name] - shows the directory of the command");
        puts("\t\t env - lists all the environment variables");
        puts("\t\t ls - list the contents of the directory");
        puts("\n\tits also possible to add parameters onto the end of certain commands. for instance, some of the commands you can append onto the end of ls are:");
        puts("\t\t -l : use a long listing format");
        puts("\t\t -g : use a long listing format without the owner field");
        puts("\t\t -m : comma seperated list");
        puts("\t\t -S : sort by file size");
        puts("\n\tthese are only a few of the available commands, try types [man ls] to see more...");
        puts("\n\n");
}

/*
* main (int argc, char *argv[])
* Description: main function for shell application. displays prompt then uses getline to fetch a line of string into malloced space. performs basic error checking. uses string comparison to check for function to change directory, 
*              then for exiting, then for help, finally to execute a command. malloced space is freed before closing.
* Param: argc: intger for the number of arguments. *argv[] pointer to a array.
* Returns: null
*/

int main ( int argc, char *argv[] ){

        char *newargv[args];
        char *line = (char*) malloc((sizeof(char*))* args);
        size_t len = 0;
        ssize_t read;
        int err_id;
        
        while (TRUE) {
                prompt();
	        read = getline (&line, &len, stdin);
	        if (read != 1) {
		        split(line, newargv);
		        //checks for whitespace
		        if (newargv[0] != NULL){
	                        if (strcmp(newargv[0], "cd") == 0) {
	                                //special condition: checks if user entered cd with no other parameters. if so changes cwd to HOME.
		                        if (newargv[1] == NULL) {
		                                chdir (getenv("HOME"));
		                        }
		                        else if (chdir(newargv[1]) == -1) {
		                                err_id = errno;
		                                fprintf(stderr,"\terror id %d : %s\n",errno, strerror( err_id ));
		                        }
                                }
                                else if (strcmp(newargv[0], "exit") == 0) {
                                        break;
                                }
                                else if (strcmp(newargv[0], "help") == 0) {
                                        help();
                                }
	                        else {
	                                exec(newargv);
	                        }
                        }}       	
                }
                free(line);
                printf("***myShell will now terminate***\n\n");
                exit(EXIT_SUCCESS);
}
