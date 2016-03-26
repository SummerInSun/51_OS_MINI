#ifndef	__structs_H__
#define __structs_H__

struct list_head
{
	struct list_head *prev;
	struct list_head *next;
};

/* �г�����ǰlist_head���ڵĽṹ�壬���ظýṹ�� */
#define mlist_entry(list_head, type, member_name) \
			(type *)((unsigned int)list_head - (unsigned int)(&(((type*)(0))->member_name)))
/* �г������е�˫�������е�list_head�ṹ�壬��head��ʼ����Ѱ�� */
#define mlist_each_entry(pos, head) \
			for(pos = (head)->next; pos != head; pos = pos->next)

extern void mlist_add_tail(struct list_head *new, struct list_head *head);

#endif