#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	char *args[] = {"./exec", NULL};
	printf("Start of Original Process.\n");
	execv(args[0], args);
	printf("End of Original Process.\n");
	return 0;
}
