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

