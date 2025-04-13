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

#define TASK_QUEUE "/mq_212105_t"
#define REPORT_QUEUE "/mq_212105_r"

typedef struct command_line_arguments {
    int w;
    int t;
    int s;        
} cli_args;

typedef struct worker {
    int id;
    int work_done;
    int task_amount;    
} worker;

typedef struct work_message {
    int task_id;
    int workload;
} work_message;

cli_args parse_arguments(int argc, char* argv[]) {
    int optgot = -1;
    char* optstr = "w:t:s:";
    cli_args args = {0, 0, 0};    

    do
    {
        optgot = getopt(argc, argv, optstr);
        switch (optgot)
        {
            case 'w':
                args.w = atoi(optarg);
                break;
            case 's':
                args.s = atoi(optarg);
                break;
            case 't':
                args.t = atoi(optarg);
                break;
            case '?':
                printf("falsches Format\n");
                exit(EXIT_FAILURE);
                break;
            default:
                break;
        }
    } while (optgot != -1);

    if (args.s <= 0 || args.w <= 0 || args.t <= 0) {
        printf("falsche Werte für Eingabeparameter\n");
        exit(EXIT_FAILURE);
    }

    return args;
}

int do_work(worker* worker) {
    //open message queues
    mqd_t command_queue = mq_open(TASK_QUEUE, O_RDONLY);
    if (command_queue == -1) {
        printf("message queue could not be opened\n");
        exit(EXIT_FAILURE);
    }
    mqd_t report_queue = mq_open(REPORT_QUEUE, O_WRONLY);
     if (report_queue == -1) {
        printf("message queue could not be opened\n");
        exit(EXIT_FAILURE);
    }

    printf("Worker %d | started worker with PID %d\n", worker->id, getpid());
    //worker starts out with 0 tasks done and 0 seconds worked
    worker->task_amount = 0;
    worker->work_done = 0;

    while (1) {
        work_message msg;
        mq_receive(command_queue, (char*)&msg, sizeof(msg), NULL);

        if (msg.workload == 0) { //if the worker receives the shutdown message
            //compile a report
            struct worker report = {
                .id = worker->id,
                .work_done = worker->work_done,
                .task_amount = worker->task_amount,
            };
            //send it back to the Ventilator
            if (mq_send(report_queue, (const char*)&report, sizeof(report), 1) == -1) {
                printf("couldn't send message\n");
                exit(EXIT_FAILURE);
            }
            //exit loop
            break;
        }
        
        //do work and update information on the amount of tasks done and how long it took
        printf("Worker %d | received Task %d with effort %d\n", worker->id, msg.task_id, msg.workload);
        sleep(msg.workload);
        worker->task_amount++;
        worker->work_done += msg.workload;

    }
    
    //cleanup and exit
    mq_close(report_queue);
    mq_close(command_queue);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {

    cli_args args = parse_arguments(argc, argv);
    //opening message queue for tasks
    struct mq_attr queue_options = {
        .mq_maxmsg = args.s,
        .mq_msgsize = sizeof(work_message),
    };
    mqd_t command_queue = mq_open(TASK_QUEUE, O_CREAT | O_WRONLY, 0644, &queue_options);
    if (command_queue == -1) {
        printf("message queue could not be opened\n");
        exit(EXIT_FAILURE);
    }
    //opening message queue for reports
    struct mq_attr queue_options_r = {
        .mq_maxmsg = args.s,
        .mq_msgsize = sizeof(worker),
    };
    mqd_t report_queue = mq_open(REPORT_QUEUE, O_CREAT | O_RDONLY, 0644, &queue_options_r);
     if (report_queue == -1) {
        printf("message queue could not be opened\n");
        exit(EXIT_FAILURE);
    }

    printf("Ventilator | Starting %d Worker for %d tasks and a queue size of %d\n", args.w, args.t, args.s);

    //creating workers
    for (int i = 1; i <= args.w; i++) {
        int id = i;  //set index before forking

        pid_t pid = fork();
        if (pid == 0) { //if it's a child process -> do work
            worker worker_info;
            worker_info.id = id;
            return do_work(&worker_info);
        }
    }    

    //sending out tasks
    printf("Ventilator | Distributing tasks...\n");
    for (int i = 1; i <= args.t; i++) {
        //make the messages
        int effort = rand() % 10 + 1; //randomize task effort
        work_message task = {
            .task_id = i,
            .workload = effort,
        };

        if (mq_send(command_queue, (const char*)&task, sizeof(task), 1) == -1) {
            printf("couldn't send message\n");
            exit(EXIT_FAILURE);
        }
        printf("Ventilator | queing task %d with effort %d\n", i, effort);
    }

    //waiting for reports and for child-processes to exit
    printf("Ventilator | waiting for workers to terminate\n");
    for (int i = 0; i < args.w; i++) {
        //send shutdown message
        work_message shutdown = {
            .task_id = 0,
            .workload = 0,
        };
        if (mq_send(command_queue, (const char*)&shutdown, sizeof(shutdown), 1) == -1) {
            printf("couldn't send message\n");
            exit(EXIT_FAILURE);
        }               

        //receive worker report
        worker msg_report;
        mq_receive(report_queue, (char*)&msg_report, sizeof(msg_report), NULL);
        printf("Ventilator | Worker %d processed %d tasks in %d seconds\n", msg_report.id, msg_report.task_amount, msg_report.work_done);

        //wait for worker to exit        
        int wstatus;
        pid_t finished_pid = wait(&wstatus);
        printf("Ventilator | Worker %d with PID %d exited with status %d\n", msg_report.id, finished_pid, WEXITSTATUS(wstatus));
    }
    
    //cleanup and exit
    mq_close(command_queue);
    mq_close(report_queue);
    exit(EXIT_SUCCESS);
}
