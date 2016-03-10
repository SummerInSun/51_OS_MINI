#ifndef __core_H__
#define __core_H__

#include <stc15f2k60s2.h>

#define TASK_MAX 5 		//�����е����������
#define SP_DEEP  27		//�����ջ�����

#define NUM_PTR_NOPC 13	//����PC�ļĴ�������ռ䳤��

/* ������̹��� */
struct task_pcb
{
	unsigned char *id;		//�����id��Ҳ�������������
	unsigned char priority;	//��������ȼ�
	unsigned char suspend;	//��������־
	unsigned char delay;	//������ʱ��־
	unsigned char sp;		//����Ķ�ջָ��
	unsigned char xdata *msg;//�������Ϣ
};

/* ��������������д��һ������Ȼ�������ѯ */
struct task_list
{
	struct task_pcd *task_pcb;	//�������
	struct task_list *next;	//�б���һ��
};
//struct task_list *tasks_list;	//��������������������������г�ʼ��


extern void sp_test(void);
extern void task_create(unsigned char priority, unsigned int task_address, unsigned char task_msg);
extern void task_switch(void);
extern void OS_start(void);
extern void OS_init(void);

#endif