#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd1, fd2, fd3;

    /* Open files */
    fd1 = open("file.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd1 == -1) {
        printf("Open file.txt failed ! \n");
        return 0;
    }
    // fd2 = open();
    fd2 = open("file.txt", O_RDWR);
    if (fd2 == -1) {
        printf("Open file.txt failed ! \n");
        return 0;
    }
    fd3 = open("file.txt", O_RDWR);
    if (fd3 == -1) {
        printf("Open file.txt failed ! \n");
        return 0;
    }

    write(fd1, "Hello,", 6);
    write(fd2, "world", 6);
    lseek(fd2, 0, SEEK_SET);
    write(fd1, "HELLO,", 6);
    write(fd3, "Gidday", 6);

    /* Close files */
    close(fd1);
    close(fd2);
    close(fd3);

    return 0;
}