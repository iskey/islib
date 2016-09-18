#ifndef __LIST_HEAD__
#define __LIST_HEAD__


#define LIST_HEAD_INIT(name) {&(name), &(name)}
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
/**
 *
 */
static inline void __list_add(struct list_head *new,
							  struct list_head *prev,
							  struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	WRITE_ONCE(prev->next, next);
}

struct  list_head{
	struct list_head *next, prev;
};



#endif
