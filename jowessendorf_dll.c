#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

struct list_head {
  struct list_head *next, *prev;
};

/* initialize "shortcut links" for empty list */
void
list_init(struct list_head *head) {
  head->next = head;
  head->prev = head;
}

/* insert new entry after the specified head */
void
list_add(struct list_head *newElement, struct list_head *head) {
  struct list_head *tmp_node = head->next;
  head->next = newElement;

  newElement->next = tmp_node;
  newElement->prev = head;

  tmp_node->prev = newElement;
}

/* insert new entry before the specified head */
void
list_add_tail(struct list_head *newElement, struct list_head *head) {
  struct list_head *tmp_node = head->prev;
  head->prev = newElement;

  newElement->next = head;
  newElement->prev = tmp_node;

  tmp_node->next = newElement;
}

/* deletes entry from list, reinitializes it (next = prev = 0),
and returns pointer to entry */
struct list_head*
list_del(struct list_head *entry) {
  entry->prev->next = entry->next;
  entry->next->prev = entry->prev;
  entry->next = NULL;
  entry->prev = NULL;
  return entry;
}

/* delete entry from one list and insert after the specified head */
void
list_move(struct list_head *entry, struct list_head *head) {
  struct list_head *tmp = list_del(entry);
  list_add(tmp, head);

}

/* delete entry from one list and insert before the specified head */
void
list_move_tail(struct list_head *entry, struct list_head *head) {
  struct list_head *tmp = list_del(entry);
  list_add_tail(tmp, head);
}

/* tests whether a list is empty */
int
list_empty(struct list_head *head) {
  return head->next == head && head->prev == head;
}

// ---------------------------
// ------ TEST PROGRAMM ------
// ---------------------------

typedef struct _proc_info {
  struct list_head head;
  int pid;
  int counter;
  int priority;
} proc_info;

void print_proc_list(proc_info *head) {
  proc_info *tmp = head;
  char *prefix = "┌── ";
  char *lastPrefix = (proc_info*) head->head.next == head ? "───" : "└── ";
  do {
    prefix = (proc_info*) tmp->head.next == head ? lastPrefix : prefix;
    printf("%spid:%d\tcounter:%d\tpriority:%d\n", prefix,  tmp->pid, tmp->counter, tmp->priority);
    tmp = (proc_info*) tmp->head.next;
    prefix = "├── ";
  } while (tmp != head);
  printf("\n");
}

int
main () {
  proc_info
    *anker = malloc(sizeof(proc_info)),
    *elem1 = malloc(sizeof(proc_info)),
    *elem2 = malloc(sizeof(proc_info)),
    *elem3 = malloc(sizeof(proc_info)),
    *elem4 = malloc(sizeof(proc_info));

  proc_info
    *anker2 = malloc(sizeof(proc_info)),
    *elem5 = malloc(sizeof(proc_info)),
    *elem6 = malloc(sizeof(proc_info));

  // initialize list element content
  anker->pid = 0;
  anker->counter = 1;
  anker->priority = 2;

  elem1->pid = 1;
  elem1->counter = 100;
  elem1->priority = 0;

  elem2->pid = 2;
  elem2->counter = 0;
  elem2->priority = 10;

  elem3->pid = 3;
  elem3->counter = 10000;
  elem3->priority = 100;

  elem4->pid = 4;
  elem4->counter = 0;
  elem4->priority = 100;

  elem5->pid = 5;
  elem5->counter = 3;
  elem5->priority = 3;

  elem6->pid = 6;
  elem6->counter = 6;
  elem6->priority = 42;

  anker2->pid = 7;
  anker2->counter = 2;
  anker2->priority = 1;

  // initialize the list
  list_init(&anker->head);
  printf("list_empty:%d\n\n", list_empty(&anker->head));
  print_proc_list(anker);
  list_add(&elem1->head, &anker->head);
  printf("list_empty:%d\n\n", list_empty(&anker->head));
  print_proc_list(anker);
  list_add_tail(&elem2->head, &anker->head);
  print_proc_list(anker);
  list_add_tail(&elem3->head, &anker->head);
  print_proc_list(anker);
  list_add(&elem4->head, &anker->head);
  print_proc_list(anker);
  list_del(&elem4->head);
  print_proc_list(anker);
  list_add(list_del(&elem3->head), &anker->head);
  print_proc_list(anker);


  list_init(&anker2->head);
  list_add(&elem5->head, &anker2->head);
  list_add(&elem6->head, &anker2->head);
  print_proc_list(anker2);
  list_move(&elem5->head, &anker->head);
  print_proc_list(anker);
  print_proc_list(anker2);
  list_move_tail(&elem6->head, &anker->head);
  print_proc_list(anker);
  print_proc_list(anker2);
}
