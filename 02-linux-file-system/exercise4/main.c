#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <stdarg.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    struct stat sb;

    if (argc != 2) {
        printf("Error: Sorry input parameters are incorrect!\n");
        printf("Usage: ./file-info /path/to/file\n");
        return 0;
    }

    if (lstat(argv[1], &sb)) {
        printf("Error: Failed to get status of file %s !\n", argv[1]);
    }

    printf("File Type: ");
    switch (sb.st_mode & S_IFMT) {
        case S_IFBLK:   printf("block device\n");       break;
        case S_IFCHR:   printf("character device\n");   break;
        case S_IFDIR:   printf("directory\n");          break;
        case S_IFIFO:   printf("FIFO/pipe\n");          break;
        case S_IFLNK:   printf("symlink\n");            break;
        case S_IFREG:   printf("regular file\n");       break;
        case S_IFSOCK:  printf("socket\n");             break;
        default:        printf("unknown?\n");
    }
    
    printf("\nI-node number: %ju, sizeof(uintmax_t): %jd\n\n", (uintmax_t)sb.st_ino, sizeof(uintmax_t));
    printf("File size: %jd bytes\n\n", (intmax_t)sb.st_size);

    printf("Last status change:     %s\n", ctime(&sb.st_ctim.tv_sec));
    printf("Last file access:       %s\n", ctime(&sb.st_atim.tv_sec));
    printf("Last file modification: %s\n", ctime(&sb.st_mtim.tv_sec));

    return 0;
}

