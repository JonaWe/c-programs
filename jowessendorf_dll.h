struct list_head {
  struct list_head *next, *prev;
};

/* initialize "shortcut links" for empty list */
extern void
list_init(struct list_head *head);

/* insert new entry after the specified head */
extern void
list_add(struct list_head *newElement, struct list_head *head);

/* insert new entry before the specified head */
extern void
list_add_tail(struct list_head *newElement, struct list_head *head);

/* deletes entry from list, reinitializes it (next = prev = 0),
and returns pointer to entry */
extern struct list_head*
list_del(struct list_head *entry);

/* delete entry from one list and insert after the specified head */
extern void
list_move(struct list_head *entry, struct list_head *head);

/* delete entry from one list and insert before the specified head */
extern void
list_move_tail(struct list_head *entry, struct list_head *head);

/* tests whether a list is empty */
extern int
list_empty(struct list_head *head);
