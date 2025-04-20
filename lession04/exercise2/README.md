```
* pthread_mutex_lock(): use to create a critical section to ensure that there's only one thread can access to the shared resources such as memory, etc.
* pthread_mutex_unlock(): use to release the locked critical section after the access to the shared resources finished.
* if not use mutex, multi threads can access to the shared resources at the same time. This will makes the program runs incorrectly.
```