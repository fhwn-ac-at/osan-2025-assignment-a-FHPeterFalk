#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

typedef struct command_line_arguments
{
    int i;
    char const* s;
    bool b;
} cli_args;

int main(int argc, char* argv[]) {
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
    

    //char const* s = NULL;

    printf("i: %d, s: %s, b: %d\n", args.i, args.s, args.b);

    return 0;
}
