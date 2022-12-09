#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <signal.h>

#define COMMAND_SIZE 1000

int cmds_in_run = 0;

int str_to_num(char* str) {
    int digit = 1;
    int num = 0;
    for(int i = (strlen(str) - 1); i != -1; i--) {
        num += (str[i] - '0')*digit;
        digit *= 10;
    }
    return num;
}

void my_handler(int sig) {
    if (sig == SIGCHLD) {
        cmds_in_run--;
    }
}

void run_execl(const char* command) {
    pid_t process_id = fork();

    int status;
    wait(&status);

    if(process_id < 0) printf("Failed to create child -> child procces! \n");

    else if(process_id > 0) {}

    else {
        int res_of_run = execl("/bin/bash", "/bin/bash", "-c", command, NULL);
        if(res_of_run == -1) {
            printf("Failed to run %s! \n", command);
            exit(-1);
        }
    }
}

void run_command (const char* command) {
    pid_t process_id = fork();

    if(process_id < 0) printf("Failed to create child procces! \n");

    else if(process_id > 0) {}

    else { 
        run_execl(command);
        printf("Commands in run = %d. \n", cmds_in_run);
        exit(0);
    }
}

int main (int argc, char* argv []) {
    if(argc == 0) {
        printf("No args in command line. \n");
        exit(-1);
    }
    int N = str_to_num(argv[1]);
    printf("N = %d. \n", N);

    signal(SIGCHLD, my_handler);
 
    char command [COMMAND_SIZE] = {'\0'};

    while(1) {
        if (read(STDIN_FILENO, command, COMMAND_SIZE) == 0) {
            exit(0);
        }
        command[strlen(command) - 1] = '\0';

        printf("Command: \"%s\". \n", command);

        if(cmds_in_run == N) {
            printf("Too many processes are being run! Try later. \n");
            continue;
        } 
        else {
            cmds_in_run++;
            printf("Commands in run = %d. \n", cmds_in_run);
            run_command(command);
        }
        memset(command, '\0', COMMAND_SIZE);
    }
}
























