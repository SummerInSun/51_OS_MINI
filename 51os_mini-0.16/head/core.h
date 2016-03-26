#ifndef __core_H__
#define __core_H__

#include <stc15f2k60s2.h>
#include "common.h"

#define TASK_MAX 5 			//�����е����������
#define SP_DEEP  30			//�����ջ�����
#define PUBLIC_SP_DEEP 30	//����Ĺ�����ջ�ռ����

#define NUM_PTR_NOPC 13	//����PC�ļĴ�������ռ䳤��

#define ENTER_CRITICAL do{EA = 0; critical_counter ++;}while(0)	//�����ٽ�״̬����̬�л����񣬶�̬���������ʱ����Ҫ
#define EXIT_CRITICAL  do{EA = 1; critical_counter --;}while(0)	//�˳��ٽ�״̬��
#define SUSPEND_TASK(taskid)   do{task_pcb_buf[taskid].suspend = 1;}while(0)	//��������
#define DESUSPEND_TASK(taskid) do{task_pcb_buf[taskid].suspend = 0;}while(0)	//��������

/* ������̹��� */
struct task_pcb
{
	unsigned char *id;		//�����id��Ҳ�������������
	unsigned char new_circle;	//���������ڵĿ�ʼ��Ҳ�����ϴ��˳�����֮����һ�ν�������ı�־
	unsigned char priority;	//��������ȼ�
	unsigned char suspend;	//��������־
	unsigned char delay;	//������ʱ��־
	unsigned char sp;		//����Ķ�ջָ��
	unsigned long run_time;	//�������е�ʱ��
	unsigned char *msg;//�������Ϣ
};

/* ��������������д��һ������Ȼ�������ѯ */
struct task_list
{
	struct task_pcd *task_pcb;	//�������
	struct task_list *next;	//�б���һ��
};
//struct task_list *tasks_list;	//��������������������������г�ʼ��

/* �ⲿ�������� */
/* ���ڽ������ݻ�����������Ϊ256���ֽ� */
extern xdata unsigned char reve_buff[256];
extern xdata unsigned char cmd_split[10][20];
/* ͳ�ƽ����ٽ�ջ�Ĵ��� */
extern xdata unsigned int critical_counter;
/* �������������id����ʼ��Ϊ0��Ĭ���ʼ����0������ */
extern xdata unsigned char current_id;
/* �����������ģ��ṹ�����飬�洢��������Ŀ��ƽṹ�� */
extern xdata struct task_pcb task_pcb_buf[TASK_MAX];
/* �������б�־��ֵ��ӳ���ж��������������� */
extern xdata unsigned long task_run_flag;
/* ��ǰϵͳ�������е������� */
extern xdata unsigned char task_running;
/* �ӿ�����������������ʱ�䣬��λ10ms */
extern xdata unsigned long task_runtime;

/* �ⲿ�������� */
extern void sp_test(void);
extern unsigned char task_kill(unsigned char pid);
extern unsigned char task_create(unsigned char priority, unsigned int task_address, unsigned char xdata *task_msg, unsigned char *name);
extern void task_switch(void);
extern void OS_start(void);
extern void OS_init(void);
extern void OS_delay(unsigned int ms);
extern void task_delay(unsigned char delay);
extern unsigned char get_task_newcircle_flag(void);

#endif