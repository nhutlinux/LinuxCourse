#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char buff[4096];
    char* filename;
    char* lenPos;
    char* modePos;
    char* cPos;
    int fd;
    int length;
    int mode;
    int i;
    int writeCount;
    int readCount;

    /* Validate input arguments */
    if (argc < 5) {
        printf("Error: Sorry, input parametes are incorrect !\n");
        return 0;
    }

    /* Validate filename */
    filename = argv[1];
    if (filename == NULL) {
        printf("Error: Sorry, filename is incorrect !\n");
        return 0;
    } else if (strlen(filename) < 1) {
        printf("Error: Sorry, filename is incorrect !\n");
        return 0;
    }

    /* Validate number of bytes */
    lenPos = argv[2];
    if (lenPos == NULL) {
        printf("Error: Sorry, number of bytes is incorrect !\n");
        return 0;
    } else if (strlen(lenPos) < 1) {
        printf("Error: Sorry, number of bytes is incorrect !\n");
        return 0;
    }
    length = 0;
    for (i = 0; i< strlen(lenPos); i++) {
        if (*(lenPos + i) < '0') {
            length = 0;
            break;
        } else if (*(lenPos + i) <= '9') {
            length *= 10;
            length += *(lenPos + i) - '0';
        } else {
            length = 0;
            break;
        }
    }
    if (length < 1) {
        printf("Error: Sorry, number of bytes is incorrect !\n");
        return 0;
    }

    /* Validate mode */
    modePos = argv[3];
    if (*modePos == 'w') {
        mode = 1;
    } else if (*modePos == 'r') {
        mode = 0;
    } else if (*modePos == 'W') {
        mode = 1;
    } else if (*modePos == 'R') {
        mode = 0;
    } else {
        printf("Error: Sorry, access mode is incorrect !\n(Only accept [w] (write mode) or [r] (read mode)) !\n");
        return 0;
    }
    /* Validate number of bytes limitation */
    if (mode == 0) {
        if (length > sizeof(buff)) {
            printf("Error: Sorry, read length is limit over (max %ld bytes)!\n", sizeof(buff));
            return 0;
        }
    }

    /* Validate input string (write mode only) */
    cPos = argv[4];
    if (mode == 1) {
        if (strlen(cPos) < length) {
            printf("Error: Sorry, input content is incorrect !\n");
            return 0;
        }
    }

    /* Open files */
    if (mode) {             /* Write mode */
        // fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    } else {                /* Read mode */
        fd = open(filename, O_RDONLY);
    }
    if (fd == -1) {
        printf("Open file: %s failed ! \n", filename);
        return 0;
    }
    // printf("Open file: %s successed ! \n", filename);

    /* Write or Read data from file */
    if (mode) {
        writeCount = write(fd, cPos, length);
        printf("Write %d bytes to %s file \n", writeCount, filename);
    } else {
        readCount = read(fd, buff, length);
        printf("Read %d bytes from %s file \n", readCount, filename);
        if (readCount > 0) {
            printf("Read content:\n");
            printf("%s\n", buff);
        }
    }

    /* Close file */
    close(fd);

    return 0;
}

