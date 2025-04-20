```
1. pthread_create() function: use to create new thread.
2. pthread_join() function: use to wait for a specific thread end.
3. A thread terminates as following cases:
   - Normally end.
   - Calls pthread_exit() function.
   - Calls pthread_cancel() function.
   - Any thread calls exit() function or the main thread terminated.
* Note: pthread_detach() function: use to put a thread to detached state.
```