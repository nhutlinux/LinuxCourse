#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    printf("Number of input arguments: %d\n", argc);

    if (!strcmp(argv[1], "1")) {
        printf("Run command <ls -lah> after 2 seconds\n");
        sleep(2);
        // execl("/bin/ls", "ls", "-l", "-a", "-h", NULL);
        // execlp("/bin/ls", "ls", "-l", "-a", "-h", NULL);
        char* const params[] = { "ls", "-l", "-a", "-h", NULL };
        execvp("/bin/ls", params);
    } else if (!strcmp(argv[1], "2")) {
        printf("Run command <date -u> after 2 seconds\n");
        sleep(2);
        // execl("/bin/date", "date", "-u", NULL);
        // execlp("/bin/date", "date", "-u", NULL);
        char* const params[] = { "date", "-u", NULL };
        execvp("/bin/date", params);
    } else {
        printf("Unknown input arguments: %s\n", argv[1]);
        printf("Please input 1 for <ls -lah> command or 2 for <date -u> command.\n");
    }
    
    return 0;
}