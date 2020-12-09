#include <stdio.h>
#include <curses.h>

int main() { 
    char ch;
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    ch = getch();
    printf("%c\n",ch);
    char ch1;
    ch1 = getch();
    printf("%c\n", ch1);
    endwin();
}