#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct command_line_arguments
{
    int i;
    char const* s;
    bool b;
} cli_args;

int child_labour()
{
    printf("I'm %d, child of %d\n", getpid(), getppid());

    printf("[%d] Doing some work...\n", getpid());
    sleep(5);
    printf("[%d] Job's done!\n", getpid());

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    
    //CL-args verarbeiten
    cli_args args = {0, NULL, false};
    int optgot = -1;
    char* optstr = "i:s:b";

    do
    {
        optgot = getopt(argc, argv, optstr);
        switch (optgot)
        {
            case 'i':
                args.i = atoi(optarg);
                break;
            case 's':
                args.s = optarg;
                break;
            case 'b':
                args.b = true;
                break;
            case '?':
                printf("falsches Format\n");
                exit(EXIT_FAILURE);
                break;
            default:
                break;
        }
    } while (optgot != -1);

    printf("i: %d, s: %s, b: %d\n", args.i, args.s, args.b);

   //PIDs

    printf("My pid is:%d\n", getpid());

    printf("[%d] sending a child to the minds...\n", getpid());
    pid_t forked = fork();
    if (forked == 0)
    {
        return child_labour();
    }

    //printf("my PID is:%d\n", getpid());
    printf("%d Enjoying some brandy...", getpid());
    printf("[%d]: Where the fudge is my coal????\n", getpid());

    int wstatus = 0;
    pid_t const waited = wait(&wstatus);
    printf("[%d] wait returned %d, status is %d\n", getpid(), waited, wstatus);

    return 0;
}
