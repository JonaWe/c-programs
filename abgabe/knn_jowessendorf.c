#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <pthread.h>
#include <math.h>

#define COND_ERR(cond, err)                      \
    if (cond) { fprintf(stderr, err); exit(-1);} \

/* This is my dll implementation. */
typedef struct list_head_t {
    struct list_head_t *next, *prev;
} list_head_t;

void list_init(list_head_t *head) {
    head->next = head;
    head->prev = head;
}

void list_add(list_head_t *newElement, list_head_t *head) {
    list_head_t *tmp_node = head->next;
    head->next = newElement;
    newElement->next = tmp_node;
    newElement->prev = head;
    tmp_node->prev = newElement;
}

void list_add_tail(list_head_t *newElement, list_head_t *head) {
    list_head_t *tmp_node = head->prev;
    head->prev = newElement;
    newElement->next = head;
    newElement->prev = tmp_node;
    tmp_node->next = newElement;
}

list_head_t *
list_del(list_head_t *entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    entry->next = NULL;
    entry->prev = NULL;
    return entry;
}

void list_move(list_head_t *entry, list_head_t *head) {
    list_head_t *tmp = list_del(entry);
    list_add(tmp, head);
}

void list_move_tail(list_head_t *entry, list_head_t *head) {
    list_head_t *tmp = list_del(entry);
    list_add_tail(tmp, head);
}

int list_empty(list_head_t *head) {
    return head->next == head && head->prev == head;
}
/* End of my dll implementation. */

typedef struct {
    void *(*function)(void *);
    void *args;
    void *result;
} Task;

typedef struct task_queue_t {
    list_head_t head;
    Task *task;
} task_queue_t;

typedef struct {
    int size;
    pthread_t threads[];
} thread_pool_t;

pthread_mutex_t queue_mutex;
pthread_cond_t queue_cond;
task_queue_t *task_queue;

pthread_mutex_t finished_queue_mutex;
pthread_cond_t finished_queue_cond;
task_queue_t *finished_task_queue;

void *worker_function(void *args) {
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        while (list_empty(&task_queue->head))
            pthread_cond_wait(&queue_cond, &queue_mutex);

        task_queue_t *current = (task_queue_t *)list_del(task_queue->head.next);
        Task *task = current->task;
        pthread_mutex_unlock(&queue_mutex);

        void *result = task->function(task->args);
        task->result = result;

        pthread_mutex_lock(&finished_queue_mutex);
        list_add_tail(&current->head, &finished_task_queue->head);
        pthread_mutex_unlock(&finished_queue_mutex);

        pthread_cond_signal(&finished_queue_cond);
    }
}

void thread_pool_init(thread_pool_t *thread_pool, int thread_count) {
    // initialize the mutex locks and conditions
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&finished_queue_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);
    pthread_cond_init(&finished_queue_cond, NULL);

    // initialize the task queue
    task_queue = malloc(sizeof(task_queue_t));
    list_init(&task_queue->head);

    // initialize the finished task queue
    finished_task_queue = malloc(sizeof(task_queue_t));
    list_init(&finished_task_queue->head);

    // create the threads
    thread_pool->size = thread_count;
    for (int i = 0; i < thread_pool->size; i++)
        if (pthread_create(&(thread_pool->threads[i]), NULL, worker_function, NULL) != 0)
            fprintf(stderr, "Thread creation failed!\n");
}

void thread_pool_enqueue(thread_pool_t *thread_pool, void *(*function)(void *), void *args) {
    Task *task = malloc(sizeof(Task));
    task->args = args;
    task->function = function;

    task_queue_t *new_element = malloc(sizeof(task_queue_t));
    new_element->task = task;

    pthread_mutex_lock(&queue_mutex);
    list_add_tail(&new_element->head, &task_queue->head);
    pthread_mutex_unlock(&queue_mutex);

    pthread_cond_signal(&queue_cond);
}

Task *thread_pool_wait(thread_pool_t *thread_pool) {
    pthread_mutex_lock(&finished_queue_mutex);
    while (list_empty(&finished_task_queue->head))
        pthread_cond_wait(&finished_queue_cond, &finished_queue_mutex);

    task_queue_t *current = (task_queue_t *)list_del(finished_task_queue->head.next);
    pthread_mutex_unlock(&finished_queue_mutex);
    return current->task;
}

void thread_pool_shutdown(thread_pool_t *thread_pool) {
    // cancel all threads
    for (int i = 0; i < thread_pool->size; i++)
        if (pthread_cancel(thread_pool->threads[i]) != 0)
            fprintf(stderr, "Thread join failed!\n");

    // destroy the mutex locks and conditions
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&finished_queue_mutex);
    pthread_cond_destroy(&queue_cond);
    pthread_cond_destroy(&finished_queue_cond);
}

typedef struct vector_t {
    int class;
    int size;
    double coordinates[];
} vector_t;

typedef struct interval_t {
    int lower;
    int upper;
} interval_t;

typedef struct neighbor_t {
    int index;
    double distance;
} neighbor_t;

typedef struct func_args_t {
    vector_t **data;
    int index;
    int *array;
    int phase;
} func_args_t;

long n;
long n_max;
int k_max;
int b;
int dimension;
int classes;

void read_input(char *filename, int n, vector_t **data) {
    FILE *file;
    file = fopen(filename, "r");

    COND_ERR(file == NULL, "ERROR: File not found!\n")
    COND_ERR(fscanf(file, "%ld %d %d", &n_max, &dimension, &classes) != 3, "ERROR: Invalid input file!\n")
    COND_ERR(n > n_max, "ERROR: n is too big for the input!\n")

    for (int i = 0; i < n; i++) {
        data[i] = malloc(sizeof(vector_t) + dimension * sizeof(double));
        data[i]->size = dimension;
        for (int j = 0; j < dimension; j++)
            COND_ERR(fscanf(file, "%lf", &data[i]->coordinates[j]) != 1, "ERROR: Invalid input file!\n")

        COND_ERR(fscanf(file, "%d", &data[i]->class) != 1, "ERROR: Invalid input file!\n")
    }
}

void calc_blocks(interval_t *blocks) {
    int rem = n % b;
    int block_size = (n - rem) / b;
    int shift = 0;
    for (int i = 0; i < b; i++) {
        blocks[i].lower = i * block_size + shift;
        if (rem > 0) {
            rem--;
            shift++;
        }
        blocks[i].upper = (i + 1) * block_size + shift - 1;
    }
}

double calc_distance(vector_t *v1, vector_t *v2) {
    double distance = 0;
    for (int i = 0; i < v1->size; i++)
        distance += pow(v1->coordinates[i] - v2->coordinates[i], 2);
    return distance;
}

int *find_k_nearest_neighbors(func_args_t *args) {
    neighbor_t *nearest_neighbors = malloc(sizeof(neighbor_t) * k_max);
    for (int i = 0; i < k_max; i++)
        nearest_neighbors[i].index = -1;

    for (int i = 0; i < n; i++) {
        if (args->array[0] <= i && i <= args->array[1])
            continue;
        double d = calc_distance(args->data[args->index], args->data[i]);
        for (int j = 0; j < k_max; j++)
            if (nearest_neighbors[j].index == -1 || d < nearest_neighbors[j].distance) {
                for (int l = k_max - 1; l > j; l--)
                    nearest_neighbors[l] = nearest_neighbors[l - 1];
                nearest_neighbors[j].index = i;
                nearest_neighbors[j].distance = d;
                break;
            }
    }
    int *indexes = malloc(sizeof(int) * k_max);
    for (int i = 0; i < k_max; i++)
        indexes[i] = nearest_neighbors[i].index;
    return indexes;
}

int *get_classification(func_args_t *args) {
    int *classifications = malloc(sizeof(int) * k_max);
    for (int k = 0; k < k_max; k++) {
        int *class_cout = malloc(sizeof(int) * classes);
        for (int i = 0; i < classes; i++)
            class_cout[i] = 0;
        for (int i = 0; i <= k; i++)
            class_cout[args->data[args->array[i]]->class]++;

        int max_index = 0;
        for (int i = 0; i < classes; i++)
            if (class_cout[i] >= class_cout[max_index])
                max_index = i;
        classifications[k] = max_index;
    }
    return classifications;
}

int *is_classification_correct(func_args_t *args) {
    int *correct = malloc(sizeof(int) * k_max);
    for (int k = 0; k < k_max; k++)
        correct[k] = args->array[k] == args->data[args->index]->class;

    return correct;
}

void print_ks_and_best_k(int *accuracy) {
    for (int k = 0; k < k_max; k++)
        printf("%d %lf\n", k + 1, (double)accuracy[k] / (double)n);
    int best_k = 0;
    for (int i = 0; i < k_max; i++)
        if (accuracy[i] >= accuracy[best_k])
            best_k = i;
    printf("%d\n", best_k + 1);
}

int main(int argc, char **argv) {
    COND_ERR(argc != 6, "ERROR: Incorrect usage: knn_jowessendorf <filename> <n> <k_max> <b> <n_threads>\n")

    char *filename = argv[1];
    n = atol(argv[2]);
    k_max = atoi(argv[3]);
    b = atoi(argv[4]);
    int n_threads = atoi(argv[5]);

    vector_t *data[n];

    read_input(filename, n, data);

    if (n_threads == -1)
        exit(0);

    interval_t *block_indices = malloc(sizeof(interval_t) * b);
    calc_blocks(block_indices);

    int *class_correct = malloc(sizeof(int) * k_max);

    for (int k = 0; k < k_max; k++)
        class_correct[k] = 0;

    if (n_threads == 0) {
        for (int i = 0; i < b; i++)
            for (int j = block_indices[i].lower; j <= block_indices[i].upper; j++) {
                func_args_t *arg = malloc(sizeof(func_args_t));
                int *current_bounds = malloc(sizeof(int) * 2);
                current_bounds[0] = block_indices[i].lower;
                current_bounds[1] = block_indices[i].upper;
                arg->index = j;
                arg->data = data;
                arg->array = current_bounds;
                int *neighbors = find_k_nearest_neighbors(arg);

                arg->array = neighbors;
                int *classifications = get_classification(arg);

                arg->array = classifications;
                int *correct = is_classification_correct(arg);

                for (int k = 0; k < k_max; k++)
                    class_correct[k] += correct[k];

                free(current_bounds);
                free(neighbors);
                free(classifications);
                free(arg);
                free(correct);
            }

        print_ks_and_best_k(class_correct);
    } else {
        thread_pool_t *thread_pool = malloc(sizeof(thread_pool_t) + n_threads * sizeof(pthread_t));

        thread_pool_init(thread_pool, n_threads);

        for (int i = 0; i < b; i++)
            for (int j = block_indices[i].lower; j <= block_indices[i].upper; j++) {
                int *current_bounds = malloc(sizeof(int) * 2);
                current_bounds[0] = block_indices[i].lower;
                current_bounds[1] = block_indices[i].upper;
                func_args_t *arg = malloc(sizeof(func_args_t));
                arg->index = j;
                arg->data = data;
                arg->array = current_bounds;
                arg->phase = 1;
                thread_pool_enqueue(thread_pool, (void *)find_k_nearest_neighbors, arg);
            }
        for (int i = 0; i < 3 * n; i++) {
            Task *finished = thread_pool_wait(thread_pool);
            free(((func_args_t *)finished->args)->array);
            ((func_args_t *)finished->args)->array = finished->result;
            int phase = ++((func_args_t *)finished->args)->phase;
            if (phase == 2)
                thread_pool_enqueue(thread_pool, (void *)get_classification, finished->args);
            else if (phase == 3)
                thread_pool_enqueue(thread_pool, (void *)is_classification_correct, finished->args);
            else {
                for (int k = 0; k < k_max; k++)
                    class_correct[k] += ((int *)finished->result)[k];
                free(finished->args);
            }
            free(finished);
        }

        print_ks_and_best_k(class_correct);

        thread_pool_shutdown(thread_pool);
    }

    for (int i = 0; i < n; i++)
        free(data[i]);

    free(class_correct);

    exit(0);
}
