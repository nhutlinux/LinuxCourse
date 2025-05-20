#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd; /* fd = File Descriptor */
    int offset;
    int writeNo;
    // char buff[] = "Hello world !\n";
    char buff[] = "I am Ubuntu !\n";

    /* Open hello.txt file with write only and append flags */
    fd = open("hello.txt", O_WRONLY | O_APPEND);
    if (fd == -1) {
        printf("Open hello.txt failed !\n");
    }

    /* lseek to the beginning of file */
    offset = lseek(fd, 0, SEEK_SET);
    printf("Offset = %d \n", offset);

    /* Write buff[] to hello.txt */
    writeNo = write(fd, buff, strlen(buff));
    printf("Write %d bytes to hello.txt \n", writeNo);

    /* Close file */
    close(fd);

    return 0;
}