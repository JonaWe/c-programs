#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct {
    void* head;
    void* tail;
} queue;

typedef struct {
    // amout of threads
    // queue of tasks
    // array of threads
} thread_pool_t;

typedef struct {
    void *(*function) (void *);
    void *arg;
    // result
    // function pointer
    // maybe reference to thread

} Task;

void thread_pool_init(thread_pool_t* thread_pool, int thread_count) {
    printf("Number of worker threads %d", thread_count);
    /* thread_pool->size = thread_count; */

    /* for (int i = 0; i < thread_pool->size; i++) { */
    /*     pthread_create(); */
    /* } */
}

void thread_pool_enqueue(thread_pool_t* thread_pool, void *(*function) (void *), void* args) {

}

Task* thread_pool_wait(thread_pool_t* thread_pool) {
    return NULL;
}

void thread_pool_shutdown(thread_pool_t* thread_pool) {

}


int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    int k_man = atoi(argv[2]);
    int b = atoi(argv[3]);
    int n_threads = atoi(argv[4]);

    n_threads = 20;

    thread_pool_init(NULL, n_threads);

    exit(0);
}
