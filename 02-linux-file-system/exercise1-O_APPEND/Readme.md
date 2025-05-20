```
When open file with O_APPEND flag, the new data is always written to the end of file.

O_APPEND
    The file is opened in append mode.  Before each write(2),
    the file offset is positioned at the end of the file, as if
    with lseek(2).  The modification of the file offset and the
    write operation are performed as a single atomic step.

    O_APPEND may lead to corrupted files on NFS filesystems if
    more than one process appends data to a file at once.  This
    is because NFS does not support appending to a file, so the
    client kernel has to simulate it, which can't be done
    without a race condition.

```
