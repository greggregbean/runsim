#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h>

#define COMMAND_SIZE 1000

int str_to_num(char* str) {
    int digit = 1;
    int num = 0;
    for(int i = (strlen(str) - 1); i != -1; i--) {
        num += (str[i] - '0')*digit;
        digit *= 10;
    }
    return num;
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

void run_command (const char* command, int* cmds_in_run) {
    pid_t process_id = fork();

    if(process_id < 0) printf("Failed to create child procces! \n");

    else if(process_id > 0) {}

    else {
        cmds_in_run[0]++;
        printf("Cmds_in_run = %d. \n", cmds_in_run[0]); 

        run_execl(command);

        cmds_in_run[0]--;
        printf("Cmds_in_run = %d. \n", cmds_in_run[0]);

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
 
    key_t key = 0; 
    if ((key = ftok("main.c", 0)) < 0) {
        printf("Can't generate a key. \n");
        exit(-1);
    }
    int shmid;
    if((shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
        if (errno != EEXIST) {
            printf("Can't create shared memory. \n");
        }
        else {
            printf("Shmem is already existing. \n");
        }
        exit(-1);
    }
    int* cmds_in_run = shmat(shmid, NULL, 0);
    cmds_in_run[0] = 0;
    printf("Cmds_in_run = %d. \n", cmds_in_run[0]);

    char command [COMMAND_SIZE] = {'\0'};

    while(1) {
        gets(command);
        fflush(stdin);

        if(strcmp(command, "^D") == 0) break;

        else if(cmds_in_run[0] == N) {
            printf("Too many processes are being run! Try later. \n");
            continue;
        } 
        else run_command(command, cmds_in_run);
    }

    if((shmctl(shmid, IPC_RMID, NULL)) == -1)
    {
        printf("Failed to delete shmem. \n");
        exit(-1);
    }
}
























