```
write(fd1, "Hello,", 6); ---> file.txt = "Hello,"
write(fd2, "world", 6); ---> file.txt = "world " ("Hello," is overwritten, after world is an undefined charactor)
lseek(fd2, 0, SEEK_SET); ---> Reset cursor of fd2 to the beginning of file. file.txt's content is not changed.
write(fd1, "HELLO,", 6); ---> file.txt = "world HELLO,", the cursor of fd1 is 7 from the last write command of fd1, 
                              therefore "HELLO," is written from 7th bytes.
write(fd3, "Gidday", 6); ---> file.txt = "GiddayHELLO,".
```
