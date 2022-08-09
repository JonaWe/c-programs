#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond;

typedef struct {
    void *(*function) (void *);
    void *args;
    /* result */
    /* function pointer */
    /* maybe reference to thread */

} Task;

typedef struct task_queue_t {
    Task *task;
    struct task_queue_t *next;
    struct task_queue_t *prev;
} task_queue_t;


int queue_empty(task_queue_t *task_queue) {
    return task_queue == NULL || task_queue->prev == NULL && task_queue->next == NULL;
}

task_queue_t* queue_pop(task_queue_t *task_queue) {
    if (task_queue->next != NULL) {
        task_queue->next->prev = NULL;
        return task_queue->next;
    } else {
        task_queue->next = NULL;
        task_queue->prev = NULL;
        task_queue->task = NULL;
        return task_queue;
    }
}

void enqueue(task_queue_t *task_queue, Task *task) {
    task_queue_t *current = task_queue;
    while (task_queue->next != NULL) {
        current = current->next;
    }

    if (current->task != NULL) {
        task_queue_t *new_item = malloc(sizeof(task_queue_t));
        new_item->prev = current;
        new_item->next = NULL;
        new_item->task = task;
        current->next = new_item;
    } else {
        current->task = task;
    }
}

void queue_init(task_queue_t *task_queue) {
    Task *task = malloc(sizeof(Task));
    task_queue->next = NULL;
    task_queue->prev = NULL;

    enqueue(task_queue, task);
}

typedef struct {
    task_queue_t *queue;
    int size;
    pthread_t threads[];
} thread_pool_t;

void* worker_function(void* args) {
    thread_pool_t *thread_pool = (thread_pool_t*) args;

    while (1) {
        pthread_mutex_lock(&queue_mutex);
        while (queue_empty(thread_pool->queue)) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }

        /* Task *task = thread_pool->queue->task; */
        /* thread_pool->queue = queue_pop(thread_pool->queue); */
        pthread_mutex_unlock(&queue_mutex);

        /* task->function(task->args); */

    }
}

void *test_function(void* args) {
    printf("HEYYYYYYYYYYYY\n");
    return NULL;
}

void thread_pool_init(thread_pool_t* thread_pool, int thread_count) {
    printf("Number of worker threads %d\n", thread_count);

    // initialize the mutex lock(s) and condition(s)
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);

    // create the threads
    thread_pool->size = thread_count;
    for (int i = 0; i < thread_pool->size; i++) {
       if (pthread_create(&(thread_pool->threads[i]), NULL, worker_function, (void*) thread_pool) != 0) {
            fprintf(stderr, "Thread creation failed!\n");
        }
    }

    // initialize the task queue
    task_queue_t *queue = malloc(sizeof(task_queue_t));
    pthread_mutex_lock(&queue_mutex);
    queue_init(queue);
    pthread_mutex_unlock(&queue_mutex);
}

void thread_pool_enqueue(thread_pool_t* thread_pool, void *(*function) (void *), void* args) {
    Task *task = malloc(sizeof(Task));
    task->args = args;
    task->function = function;
    pthread_mutex_lock(&queue_mutex);
    enqueue(thread_pool->queue, task);
    pthread_mutex_unlock(&queue_mutex);

    pthread_cond_signal(&queue_cond);

}

Task* thread_pool_wait(thread_pool_t* thread_pool) {
    return NULL;
}

void thread_pool_shutdown(thread_pool_t* thread_pool) {

    for (int i = 0; i < thread_pool->size; i++) {
        if (pthread_join(thread_pool->threads[i], NULL) != 0) {
            fprintf(stderr, "Thread join failed!\n");
        }
    }

    // destroy the mutex lock(s) and conditions(s)
    pthread_mutex_destroy(&queue_mutex);
    pthread_cond_destroy(&queue_cond);
}


int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "Incorrect usage: knn_jowessendorf <filename> <n> <k_max> <b> <n_threads>\n");
    }
    char *filename = argv[1];
    long n = atol(argv[2]);
    int k_max = atoi(argv[3]);
    int b = atoi(argv[4]);
    int n_threads = atoi(argv[5]);

    thread_pool_t* thread_pool = malloc(sizeof(thread_pool_t) + n_threads * sizeof(pthread_t));

    thread_pool_init(thread_pool, n_threads);
    /* thread_pool_enqueue(thread_pool, test_function, NULL); */

    exit(0);
}
