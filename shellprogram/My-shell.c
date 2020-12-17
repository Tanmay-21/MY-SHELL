// -------Including Necessary Headers-------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>

// -------Function to Convert integer to string-------
char *itoa(int value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36)
    {
        *result = '\0';
        return result;
    }
    char *pointer = result, *pointer1 = result, temp_char;
    int temp_value;
    do
    {
        temp_value = value;
        value /= base;
        *pointer++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (temp_value - value * base)];
    } while (value);
    // Apply negative sign
    if (temp_value < 0)
        *pointer++ = '-';
    *pointer-- = '\0';
    while (pointer1 < pointer)
    {
        temp_char = *pointer;
        *pointer-- = *pointer1;
        *pointer1++ = temp_char;
    }
    return result;
}

// -------The max number of arguments in a command-------
#define SH_TOKEN_BUFSIZE 1000
// -------The characters acting as seperators in a command-------
#define SH_TOKEN_DELIMIN " \"\t\r\n"
#define SH_TOKEN_DELIMIN_SUB "{}./"

// -------Function to separate command into arguments-------
char **sh_split_line(char *line, bool sub)
{
    int bufsize = SH_TOKEN_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;
    if (!tokens)
    {
        // if tokens not empty initially
        fprintf(stderr, "My-Shell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    // take arguments seperated by specified deliminators
    if (sub)
    {
        token = strtok(line, SH_TOKEN_DELIMIN_SUB);
    }
    else
    {
        token = strtok(line, SH_TOKEN_DELIMIN);
    }
    while (token != NULL)
    {
        tokens[position] = token;
        if (sub)
        {
            token = strtok(NULL, SH_TOKEN_DELIMIN_SUB);
        }
        else
        {
            token = strtok(NULL, SH_TOKEN_DELIMIN);
        }
        position++;
    }
    // end the arguments array with a null to specify end
    tokens[position] = NULL;
    return tokens;
}

// -------Function to read command from terminal-------
char *sh_read_line()
{
    char *line = NULL;
    ssize_t bufsize = 0;
    if (getline(&line, &bufsize, stdin) == -1)
    {
        // failure in getting input
        if (feof(stdin))
        {
            // stream ends in null i.e. empty
            exit(EXIT_SUCCESS);
        }
        else
        {
            // error message with pretext
            perror("My-Shell: getline\n");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

// -------Function to initiate and run process-------
int sh_launch(char **args)
{
    pid_t pid;
    int status;
    pid = fork(); // fork the current process
    if (pid == 0)
    {
        // child process
        // execute the specified process
        if (execvp(args[0], args) == -1)
        {
            // if any return value
            // then execution failed
            printf("Invalid Command\n");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        // failure in forking
        perror("My-Shell");
    }
    else
    {
        // wait till the completion of child process
        do
        {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

// -------Functions to change color of output text-------
void change_red() { printf("\033[0;31m"); }
void change_yellow() { printf("\033[1;33m"); }
void change_magenta() { printf("\033[0;35m"); }
void change_cyan() { printf("\033[1;36m"); }
void change_blue() { printf("\033[0;34m"); }
void change_green() { printf("\033[0;32m"); }
// -------Function to reset color of output text to default-------
void reset() { printf("\033[0m"); }

// -------Function to print main command input line header-------
void sh_print_main()
{
    char path[100];
    ssize_t bufsize = 100; // size of path string
    change_red();
    printf("My-Shell:");
    change_cyan();
    // print current working directory path
    char *pwd = getcwd(path, bufsize);
    char **args = sh_split_line(pwd , true);
    // visible path shortening
    if(args[0] == NULL)
    {
        printf("/");
    }
    else if(args[1] == NULL)
    {
        printf("/%s", args[0]);
    }
    else if(args[2] == NULL)
    {
        printf("/%s/%s",args[0],args[1]);
    }
    else
    {
        char *first, *second;
        for (int i = 1; args[i] != NULL; i++)
        {
            first = args[i-1];
            second = args[i];
        }
        printf("~/%s/%s", first, second);
    }
    change_red();
    printf(">> ");
    reset();
}

// -------Function to print sub command input line header-------
void sh_print_sub()
{
    change_yellow();
    printf("@>> ");
    reset();
}

// -------Defining primary builtin commands-------
// -------Function for builtin command "cd" to change directory-------
int sh_cd(char **args)
{
    // args[0] == "cd"
    if (args[1] == NULL)
    {
        // no arguments follow "cd"
        fprintf(stderr, "Shell: expected argument to \"cd\"\n");
    }
    else
    {
        // change directory to arguments followed by "cd"
        if (chdir(args[1]) != 0)
        {
            // invalid arguments
            perror("Invalid Call");
        }
    }
    return 1;
}

// -------Function for builtin command "exit" for terminating the shell-------
int sh_exit(char **args)
{
    // acknowledgement message
    printf("Logged Out !!!\n");
    return 0; // return status 0 to end the main shell loop
}

// -------Function to search anything on google from shell-------
int sh_google(char **args)
{
    if (args[1] == NULL)
    {
        // if nothing to search open google search default
        char line[] = "xdg-open https://www.google.com"; // command to open url in default browser
        char **Args;
        Args = sh_split_line(line, false);
        return sh_launch(Args);
    }
    // create url from command arguments as search query
    char line[10000] = "xdg-open https://www.google.com/search?q=";
    strcat(line, args[1]);
    const char *add = "+"; // character to split arguments in search query
    for (int i = 2;; i++)
    {
        if (args[i] == NULL)
        {
            break;
        }
        strcat(line, add);
        strcat(line, args[i]);
    }
    char **Args;
    Args = sh_split_line(line, false);
    return sh_launch(Args);
}

// -------Function to search anything on stackoverflow from shell-------
int sh_stackoverflow(char **args)
{
    if (args[1] == NULL)
    {
        // if nothing to search open stackoverflow default
        char line[] = "xdg-open https://www.stackoverflow.com"; // command to open url in default browser
        char **Args;
        Args = sh_split_line(line, false);
        return sh_launch(Args);
    }
    // create url from command arguments as search query
    char line[10000] = "xdg-open https://www.stackoverflow.com/search?q=";
    strcat(line, args[1]);
    const char *add = "+"; // character to split arguments in search query
    for (int i = 2;; i++)
    {
        if (args[i] == NULL)
        {
            break;
        }
        strcat(line, add);
        strcat(line, args[i]);
    }
    char **Args;
    Args = sh_split_line(line, false);
    return sh_launch(Args);
}

// -------Number of primary builtin commands-------
#define BUILTINPRIM 4
// -------Specifiying the keywords-------
char *builtin_primary_str[] = {
    "cd",
    "exit",
    "google",
    "stackoverflow"};

// -------Array of pointers to primary built in functions-------
int (*builtin_primary_func[])(char **) = {
    &sh_cd,
    &sh_exit,
    &sh_google,
    &sh_stackoverflow
};

// -------Function to check for the call to any primary builtin command-------
// -------Primary builtin commands can only call or use system commands-------
int sh_check_primary(char **args)
{
    for (int i = 0; i < BUILTINPRIM; i++)
    {
        // check for specified keywords
        if (strcmp(args[0], builtin_primary_str[i]) == 0)
        {
            return (*builtin_primary_func[i])(args);
        }
    }
    // no primary builtin command
    return -1;
}

// -------Function to run commands including primary builtin-------
int sh_sub_execute(char **args)
{
    if (args[0] == NULL)
    {
        // empty command
        return 1;
    }
    // check primary commands
    int prim_status = sh_check_primary(args);
    if (prim_status != -1)
    {
        // primary command executed
        return prim_status;
    }
    return sh_launch(args);
}

// -------max size of arguments-------
#define RANGEBUFSIZE 100
// -------Builtin function to run for loops in shell-------
int sh_for(char **args)
{
    // first line of command is "for variable in range"
    char *var; // store the variable
    if (args[1] == NULL)
    {
        // no arguments provided after "for"
        fprintf(stderr, "Shell: expected argument to \"for\"\n");
        return 1;
    }
    // args[1] is the variable
    var = args[1];
    char Var[3] = "$";
    strcat(Var, var); // add "$" at the beginning of the variable to use in commands inside the for loop
    if (args[2] == NULL)
    {
        // no arguments provided after "variable"
        fprintf(stderr, "Shell: expected argument to \"for\"\n");
        return 1;
    }
    // args[2] must be syntax "in"
    char *syntax = "in";
    if (strcmp(args[2], syntax) != 0)
    {
        fprintf(stderr, "Shell: wrong syntax\n");
        return 1;
    }
    // now the remaining part must specify the range for loop to run
    if (args[3] == NULL)
    {
        // no arguments provided after "in"
        fprintf(stderr, "Shell: no range specified\n");
        return 1;
    }
    // 3 ways to specify range
    // -> a b c d e .. so on (individual elements of range)
    // -> {a..b..c} range starts from a, ends at b, increments by c
    // -> {a..b} same as above except increment is 1 by default
    int bufsize = RANGEBUFSIZE;
    int position;
    bool type1 = false; // type specifying parameter
    // parameters for type 2 for loop
    int a, b, c;
    // parameters for type 1 for loop
    int *range;
    int RangeSize; 
    if (args[3][0] == '{')
    {
        type1 = false;
        if (args[4] != NULL)
        {
            fprintf(stderr, "Shell: too many arguments\n");
            return 1;
        }
        char **SubArgs;
        SubArgs = sh_split_line(args[3], true);
        if (SubArgs[0] == NULL) 
        {
            fprintf(stderr, "Shell: no range specified\n");
            return 1;
        }
        a = atoi(SubArgs[0]); // start value of range
        if (SubArgs[1] == NULL)
        {
            fprintf(stderr, "Shell: range termination value not specified\n");
            return 1;
        }
        b = atoi(SubArgs[1]); // end value of range
        c = 1; // increment is set to 1 by default
        if (SubArgs[2] != NULL)
        {
            c = atoi(SubArgs[2]);
            if(SubArgs[3] != NULL)
            {
                fprintf(stderr, "Shell: too many arguments\n");
                return 1;
            }
        }
    }
    else
    {
        type1 = true;
        // store values to iterate over in the range array
        range = malloc(bufsize * sizeof(int));
        position = 0;
        for (int i = 3;; i++)
        {
            if (args[i] == NULL)
            {
                break;
            }
            // as values a stored as strings convert them to int
            range[position] = atoi(args[i]);
            position++;
        }
        // store size of range array
        RangeSize = position;
    }
    char *line;
    char **Args;
    sh_print_sub();
    // do
    syntax = "do";
    line = sh_read_line();
    Args = sh_split_line(line, false);
    if (strcmp(Args[0], syntax) != 0)
    {
        fprintf(stderr, "Shell: wrong syntax1\n");
        return 1;
    }
    // command to loop
    sh_print_sub();
    line = sh_read_line();
    char **Store_Args = sh_split_line(line, false);
    sh_print_sub();
    // done
    syntax = "done";
    line = sh_read_line();
    Args = sh_split_line(line, false);
    if (strcmp(Args[0], syntax) != 0)
    {
        fprintf(stderr, "Shell: wrong syntax\n");
        return 1;
    }
    // find positions to replace $i with value of i in loop command
    int *positions = malloc(bufsize * sizeof(int));
    position = 0;
    for (int i = 0;; i++)
    {
        if (Store_Args[i] == NULL)
        {
            break;
        }
        if (strcmp(Store_Args[i], Var) == 0)
        {
            positions[position] = i;
            position++;
        }
    }
    int ReplaceSize = position;
    // execute loop
    if (type1 == true)
    {
        for (int i = 0; i < RangeSize; i++)
        {
            for (int j = 0; j < ReplaceSize; j++)
            {
                char snum[100];
                itoa(range[i], snum, 10);
                Store_Args[positions[j]] = snum;
            }
            int ret = sh_sub_execute(Store_Args);
            if (ret == 0)
            {
                return ret;
            }
        }
    }
    else
    {
        for (int i = a; i <= b; i += c)
        {
            for (int j = 0; j < ReplaceSize; j++)
            {
                char snum[100];
                itoa(i, snum, 10);
                Store_Args[positions[j]] = snum;
            }
            int ret = sh_sub_execute(Store_Args);
            if (ret == 0)
            {
                return ret;
            }
        }
    }
    return 1;
}

// -------Number of secondary builtin commands-------
#define BUILTINSECD 1
// -------Specifiying the keywords for those commands-------
char *builtin_secondary_str[] = {
    "for"};

// -------Array of pointers to the secondary built in functions-------
int (*builtin_secondary_func[])(char **) = {
    &sh_for};

// -------Function to check for the call to any secondary builtin command-------
// -------Secondary builtin commands can call system commands as well as primary builtin commands-------
int sh_check_secondary(char **args)
{
    for (int i = 0; i < BUILTINSECD; i++)
    {
        // check for specified keywords
        if (strcmp(args[0], builtin_secondary_str[i]) == 0)
        {
            return (*builtin_secondary_func[i])(args);
        }
    }
    // no secondary builtin command
    return -1;
}

// -------Function to run input commands including primary and secondary builtin-------
int sh_execute(char **args)
{
    if (args[0] == NULL)
    {
        // empty command
        return 1;
    }
    // check primary commands
    int prim = sh_check_primary(args);
    if (prim != -1)
    {
        // primary command executed
        return prim;
    }
    // check secondary commands
    int sec = sh_check_secondary(args);
    if (sec != -1)
    {
        // secondary command executed
        return sec;
    }
    return sh_launch(args);
}

int main(int argc, char **argv)
{
    char *line;  // store complete input command
    char **args; // store arguments of input command
    int status;  // store status after command execution
    printf("Initializing Personal Shell...\n");
    do
    {
        sh_print_main();
        line = sh_read_line();      // input command
        args = sh_split_line(line, false); // split into arguments
        status = sh_execute(args);  // execute command
        free(line);
        free(args);
    } while (status);
    return 0;
}