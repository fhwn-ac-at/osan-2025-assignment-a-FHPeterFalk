#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>

typedef struct command_line_arguments
{
    int i;
    char const* s;
    bool b;
} cli_args;

struct work_message {
    int work;
};

cli_args parse_arguments(int argc, char* argv[])
{
    int optgot = -1;
    char* optstr = "i:s:b";
    cli_args args = {0, NULL, false};    

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

    return args;
}

int child_labour()
{
    mqd_t command_queue = mq_open("/mq_212105", O_WRONLY);
    printf("[%d] mq_open returned %d\n", getpid(), command_queue);

    printf("I'm %d, child of %d\n", getpid(), getppid());

    printf("[%d] Doing some work...\n", getpid());
    sleep(5);
    printf("[%d] Job's done!\n", getpid());

    return EXIT_SUCCESS;
}

int child_labour_messages()
{
    mqd_t command_queue = mq_open("/mq_212105", O_WRONLY);
    printf("[%d] mq_open returned %d\n", getpid(), command_queue);   
    
    printf("I'm %d, child of %d\n", getpid(), getppid());

    printf("[%d] waiting for instructions...\n", getpid());
    struct work_message instructions;
    mq_receive(command_queue, (void*)&instructions, sizeof(struct work_message), NULL);

    printf("[%d] Doing some work...\n", getpid());
    sleep(instructions.work);
    printf("[%d] Job's done!\n", getpid());

    mq_close(command_queue);
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
/*    
    //CL-args verarbeiten
    cli_args args = parse_arguments(argc, argv);

    printf("i: %d, s: %s, b: %d\n", args.i, args.s, args.b);
*/
   //PIDs
/*
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
*/
    //Message queue

    struct mq_attr queue_options = {
        .mq_maxmsg = 1,
        .mq_msgsize = sizeof(struct work_message),
    };

    mqd_t command_queue =
        mq_open("/mq_212105", O_WRONLY | O_CREAT | S_IRWXU, &queue_options);
    printf("[%d] mq_open returned %d\n", getpid(), command_queue);

    for (int i = 0; i<10; i++){
        pid_t forked = fork();
        if (forked == 0) {
            return child_labour_messages();
        }
    }

    mq_send(command_queue, );    

    for (int i = 0; i < 10; i++){
        int wstatus = 0;
        pid_t const waited = wait(&wstatus);
        printf("[%d] wait returned %d, status is %d\n", getpid(), waited, wstatus);
    }
    mq_close(command_queue);
    return 0;
}
