#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	printf("I am original process. My pid is %i.\n" , getpid());
	pid_t t = fork();
	if (t == -1) {
		printf("fork failed!\n");
		exit(1);	
	}
	if (t == 0) {
		printf("I am the child process. My pid is %i.\n" , getpid());
		exit(0);
	} else {
		printf("I am the parent process. My pid is %i.\n" , getpid());
	}
	return 0;
}
