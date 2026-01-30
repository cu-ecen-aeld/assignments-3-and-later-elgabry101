#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    thread_func_args->thread_complete_success=false;
    usleep(thread_func_args->wait_obtain*1000);
    if(pthread_mutex_lock(thread_func_args->mtx)!=0)
        return thread_func_args;
    usleep(thread_func_args->wait_release*1000);
    if(pthread_mutex_unlock(thread_func_args->mtx)!=0)
        return thread_func_args;
    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    thread_func_args->thread_complete_success=true;
    return thread_func_args;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,
                                  int wait_to_obtain_ms, int wait_to_release_ms)
{
    struct thread_data *data = malloc(sizeof(struct thread_data));
    if (!data)
        return false;

    data->wait_obtain = wait_to_obtain_ms;
    data->wait_release = wait_to_release_ms;
    data->mtx = mutex;
    data->thread_complete_success = false;

    if (pthread_create(thread, NULL, threadfunc, data) != 0) {
        free(data);
        return false;
    }

    /* Do NOT join or free here */
    return true;
}

