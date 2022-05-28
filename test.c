#include <asm-generic/errno-base.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DBG(fmt, ...) printf(fmt, ##__VA_ARGS__);

#define DEFAULT_THREADCOUNT 100
#define DEFAULT_DELAY_MULTIPLE 1

struct thread_args {
    pthread_t t_id;
    int thread_num;
    int num_threads;
    pthread_barrier_t *barrier;
    pthread_t previous_thread;
};


static void * thread_func(void* args) {
    int err = 0;
    struct thread_args* targs = args;
    pthread_barrier_wait(targs->barrier);
    printf("Thread %02d started.\n", targs->thread_num); 
    sleep(DEFAULT_DELAY_MULTIPLE * (targs->num_threads - targs->thread_num));
    if (0 != targs->previous_thread) {
        DBG("Waiting for thread #(%03d) %lud to join tid #%lud\n", targs->thread_num, targs->t_id, targs->previous_thread);
        err = pthread_join(targs->previous_thread, NULL);
        if (0 != err) {
            DBG("Thread #(%03d) %lud failed to join tid #%lud. Errno %d\n", 
                    targs->thread_num, targs->t_id, targs->previous_thread, err);
            return NULL;
        }
    }
    printf("Thread #(%03d) %lud ended.\n", targs->thread_num, targs->t_id); 
    return NULL;
}

int main(int argc, char** argv){
    int i = 0, j = 0;
    int retcode = -1;
    int num_threads = DEFAULT_THREADCOUNT;
    int num_iters = 20; 
    struct thread_args *targs = NULL;
    
    pthread_t prev_tid = 0;
    pthread_attr_t attr = {0};
    pthread_barrierattr_t b_attr = {0};
    pthread_barrier_t barrier = {0};

    if (argc < 2) {
        printf("Using default num_threads %d.\n", num_threads);
    }
    else {
        num_threads = atoi(argv[1]);
    }
    if (-1 == pthread_attr_init(&attr)) {
       DBG("Failed to init pthread attr. Error %s\n", strerror(errno)); 
       goto FAIL;
    }
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (-1 == pthread_barrierattr_init(&b_attr)) {
       DBG("Failed to init pthread barrier attr. Error %s\n", strerror(errno)); 
       goto FAIL;
    }
    if (-1 == pthread_barrier_init(&barrier, &b_attr, num_threads)) {
       DBG("Failed to init pthread barrier attr. Error %s\n", strerror(errno)); 
       goto FAIL;
    }
    targs = calloc(num_threads, sizeof(*targs)); 
    if (NULL == targs) {
        DBG("Failed to alloc thread args array. Error: %s\n", strerror(errno));
        goto FAIL;
    }
    for (j = 0; j < num_iters; j++) {
        for (i = 0; i < num_threads; i++) {
            targs[i].thread_num = i;
            targs[i].num_threads = num_threads;
            targs[i].previous_thread = prev_tid;
            targs[i].barrier = &barrier;
            if (-1 == pthread_create(&targs[i].t_id, &attr, &thread_func, &targs[i])) {
                DBG("Failed to create thread #%d. Error: %s\n", i, strerror(errno));
                goto FAIL;
            }
            prev_tid = targs[i].t_id;
        }
        /* Wait for all threads to end */
        pthread_join(targs[num_threads-1].t_id, NULL);
        DBG("All threads have joined.\n");
    }
    if (-1 == pthread_barrier_destroy(&barrier)) {
       DBG("Failed to init pthread barrier attr. Error %s\n", strerror(errno)); 
       goto FAIL;
    }
    if (-1 == pthread_barrierattr_destroy(&b_attr)) {
       DBG("Failed to destroy pthread barrier attr. Error %s\n", strerror(errno)); 
       goto FAIL;
    }
    if (-1 == pthread_attr_destroy(&attr)) {
        DBG("Failed to destroy the pthread attr. Error: %s\n", strerror(errno));
        goto FAIL;
    }

    free(targs);
    retcode = 0;
FAIL:
    return retcode;
}
