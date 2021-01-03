#include <stdio.h>
#include <stdlib.h>

// ~Functions to change color of output text~
void change_red () { printf("\033[0;31m"); }
void change_yellow () { printf("\033[1;33m"); }
void change_magenta () { printf("\033[0;35m"); }
void change_cyan () { printf("\033[0;36m"); }
void change_blue () { printf("\033[0;34m"); }
void change_green () { printf("\033[1;32m"); }
// ~Function to reset color of output text to default~
void reset () { printf("\033[0m"); }
