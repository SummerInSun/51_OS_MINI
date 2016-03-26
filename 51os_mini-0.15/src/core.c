#include "core.h"

/* �����������ģ��ṹ�����飬�洢��������Ŀ��ƽṹ�� */
struct task_pcb xdata task_pcb_buf[TASK_MAX];

#ifdef XDATA_SP_BUFFER
/* �����ջ���飨α��ջ��ʵ�ʶ�ջֻ�ܷ���idata���棩 */
volatile unsigned char xdata task_sp[TASK_MAX][SP_DEEP];
/* �����ջ�ռ䣬����������ͬһ����ջ�ռ� */
volatile unsigned char idata task_sp_public[PUBLIC_SP_DEEP];
unsigned char idata task_sp_bottom;	//ջ�ռ�ĵײ�
#endif
#ifdef IDATA_SP_BUFFER
/* �����ջ���� */
volatile unsigned char idata task_sp[TASK_MAX][SP_DEEP];
#endif

/* ��ǰϵͳ�������е������� */
xdata unsigned char task_running = 0;
/* �������������id����ʼ��Ϊ0��Ĭ���ʼ����0������ */
xdata unsigned char current_id = 0;
/* �������б�־��ֵ��ӳ���ж��������������� */
xdata unsigned long task_run_flag = 0;
/* ͳ�ƽ����ٽ�ջ�Ĵ��� */
xdata unsigned int critical_counter = 0;
/* �ӿ�����������������ʱ�䣬��λ10ms */
xdata unsigned long task_runtime = 0;
/* ���ڽ������ݻ�����������Ϊ256���ֽ� */
xdata unsigned char reve_buff[256];
xdata unsigned char cmd_split[10][20];
/* ����ϵͳ���б�־��1Ϊ�������У�0Ϊδ��ʼ���С���ʼ��Ϊ0 */
bit task_start_flag = 0;

//void pop_ptr(void)
//{
//	#pragma asm
//		POP 7
//		POP 6
//		POP 5
//		POP 4
//		POP 3
//		POP 2
//		POP 1
//		POP 0
//		POP PSW
//		POP DPH
//		POP DPL
//		POP B
//		POP ACC
//	#pragma endasm
//}

/* ϵͳ��ʱ,��λms */
void OS_delay(unsigned int ms)
{
	ms = ms / 10;	
	task_delay(ms);

	while(task_pcb_buf[current_id].delay);
}

/* ϵͳ������ʼ�� */
void OS_init(void)
{
	unsigned int i = 0;

	current_id = 0;	 		//��ǰ���������Ϊ0
	task_run_flag = 0;		//��ǰ����������
	task_runtime = 0; 		//��ǰ����ʱ��Ϊ0
	task_start_flag = 0;	//ϵͳδ��ʼ����
	
	/* ��������ʱ��ȫ������ */
	for(i = 0; i < TASK_MAX; i ++)
	{
		task_pcb_buf[i].run_time = 0;
		task_pcb_buf[i].delay  = 0;
	}	
}

/* ϵͳ��ʼ������� */
void OS_start(void)
{
#ifdef XDATA_SP_BUFFER
	task_sp_bottom = task_sp_public; 	//������ʵ�����е�ջ�ռ���ʼ��ַ����ջ���ݴ����

	task_sp_public[0] = task_sp[current_id][0];	 //��ʼ���������л��жϷ�������ʱ�����һ�������еĺ����ĵ�ַ
	task_sp_public[1] = task_sp[current_id][1];	 //�߰�λ��Ͱ�λ�浽ջ�ռ���

	SP = task_sp_public;	//��ջ�ռ���ʼ��ַ��ֵ��SPָ��
	SP = SP + 1;   			//SPָ���һ��ָ������ַ�ĸ߰�λ����ʼ���У�51��ջ���������ǳ��ŵ�ַ���ӵķ��������ģ�
	TR1 = 1;										  //��ʼ��ʱ
	ET1 = 1;										  //ʹ�ܶ�ʱ��1
	EA  = 1;										  //��ȫ���ж�
#endif
#ifdef IDATA_SP_BUFFER
	task_pcb_buf[current_id].sp -= NUM_PTR_NOPC;

	SP = task_pcb_buf[current_id].sp;
	TR1 = 1;										  //��ʼ��ʱ
	ET1 = 1;										  //ʹ�ܶ�ʱ��1
	EA  = 1;										  //��ȫ���ж�
#endif
	
}

/* ɱ��һ������ */
unsigned char task_kill(unsigned char pid)
{
	ENTER_CRITICAL;		//�����ٽ�ջ��������ִ�еĹ����б��ж�
	if(task_run_flag & (1 << pid))
	{
		task_run_flag &= ~(1 << pid);	

	}
	else
	{
		goto error;
	}
	task_running --;	//��ǰϵͳ�������е���������1
	EXIT_CRITICAL;		//�˳��ٽ�ջ�������������
	return 0;	
error:		
	EXIT_CRITICAL;		//�˳��ٽ�ջ�������������
	return 1;
}

/* �����л������ڶ�ʱ���������������л� ����ʱ���ã�*/
void task_switch(void)
{
	EA= 0;	//�����л���ʱ�������ж�
	/* ����Ĵ���ֵ���Ĵ���ֵ��ջ */
//	push_ptr();
	/* ��ת��֮ǰ�����ջָ�뱣������������ */
	task_pcb_buf[current_id].sp = SP;
//	if(tasks_list

	/* �Ĵ���ֵ��ջ���ָ��Ĵ���ֵ */
//	pop_ptr();
	EA = 1;	//�����л���ɣ����¿����ж�
}

/* �������� 
 * priority �������ȼ�
 * task_address �����ַ
 * task_msg ������Ϣָ��
 * name ���������
 */
unsigned char task_create(unsigned char priority, unsigned int task_address, unsigned char xdata *task_msg, unsigned char *name)
{
	static xdata unsigned char i = 0;
	static xdata unsigned char stack_deep = 0;
	static unsigned char stack_p;

	ENTER_CRITICAL;	//��̬���������ʱ�������жϣ���ʱӦ�ý����ٽ�ջ

	for(i = 0; i < TASK_MAX; i ++)
	{
		if(task_run_flag & (1 << i))
			;
		else
		{
#ifdef XDATA_SP_BUFFER
			/* ��ʼ����ն�ջ�� */
			for(stack_deep = 0; stack_deep < SP_DEEP; stack_deep ++)
				task_sp[i][stack_deep] = 0;
			
			task_pcb_buf[i].sp = 14;	//��ʼ��ѹ��ջ�����ݵĳ���Ϊ14
			task_sp[i][0] = task_address % 256;	  //�������ַ��8λ��ջ
			task_sp[i][1] = task_address >> 8;	  //�������ַ��8λ��ջ
			
			task_pcb_buf[i].priority = priority;	//������������ȼ�
			task_pcb_buf[i].msg		 = task_msg; 	//�����������Ϣָ��
			task_pcb_buf[i].id       = name;
			
			task_run_flag |= (1 << i);		//�Ѵ�λ����Ϊ������
#endif
#ifdef IDATA_SP_BUFFER
			stack_p = (unsigned char)task_sp[i];	//�Ѷ�ջָ��ĵ�ַ��ֵ��ջ��ַ�ݴ���
			/* ��ʼ����ն�ջ�� */
			for(stack_deep = 0; stack_deep < SP_DEEP; stack_deep ++)
				((unsigned char idata *)stack_p)[stack_deep] = 0;
			
			task_pcb_buf[i].sp = stack_p;	//�Ѷ�ջָ�븳ֵ������ṹ��
			((unsigned char idata *)stack_p)[0] = task_address % 256;	  //�������ַ��8λ��ջ
			task_pcb_buf[i].sp ++;
			((unsigned char idata *)stack_p)[1] = task_address >> 8;	  //�������ַ��8λ��ջ
			
			task_pcb_buf[i].sp += NUM_PTR_NOPC;		//�������ջָ��ֵ��Ϊ�����л�ʱ��״̬
			task_pcb_buf[i].priority = priority;	//������������ȼ�
			task_pcb_buf[i].msg		 = task_msg; 	//�����������Ϣָ��
			task_pcb_buf[i].id       = name;
			
			task_run_flag |= (1 << i);		//�Ѵ�λ����Ϊ������
#endif
			
			task_running ++;	//��ǰϵͳ�������е���������1
			EXIT_CRITICAL;	//���񴴽���ϣ��˳��ٽ�ջ

			return 0;	//���񴴽��ɹ�������
		}
	}

	EXIT_CRITICAL;		//���񴴽�ʧ�ܣ�ҲҪ�˳��ٽ�ջ
	return 1;			//��Դ���㣬�޷������������񣬷���1
}


/* �жϷ��������ö�ʱ��1����������� */
void timer1_task_switch(void) interrupt 3
{
	unsigned char xdata temp = 0;
	char sp_count = 0;
	
	EA = 0;	//�����л���ʱ��Ҫ�ر��ж�

/* ����ĳ����ã���Ϊ����������֮��Ľ���ǲ���Ҫ������α�ע�͵���� */
//	if(!task_start_flag)	//��һ�ν����ж�
//	{
//		SP = SP - NUM_PTR_NOPC;	//��ʱָ��ָ����ô��ж�ʱ��ѹ���PC�͵�ַ
//		for(temp = 1; temp < NUM_PTR_NOPC + 1; temp ++)
//		{
//			((unsigned char idata *)SP)[temp] = ((unsigned char idata *)SP)[temp + 2];	//������PC֮��ļĴ���ֵ����ƽ����λ
//		}
//		SP = SP + NUM_PTR_NOPC;
//		task_start_flag = 1; //����ϵͳ���б�־����ʱΪ�Ѿ�����
//	}

	/* ����Ĵ���ֵ���Ĵ���ֵ��ջ */
//	push_ptr();	 /* �����б��뻷���¼Ĵ����Ѿ���ϵͳ�Զ������� */
	
	TF1 = 0;	//����жϱ�־
#ifdef XDATA_SP_BUFFER
	/* ������ʵ�����е�ջ�ռ����ֵ�浽xdata�� */
	for(sp_count = 0; SP >= task_sp_bottom; SP--)
	{
		task_sp[current_id][sp_count] = ((unsigned char idata*)task_sp_bottom)[sp_count];
		sp_count ++;
	}
	sp_count --;	//forѭ���˳���һ
	/* ��ת��֮ǰ�����ջָ�뱣����������ṹ������ */
	task_pcb_buf[current_id].sp = sp_count;
#endif
#ifdef IDATA_SP_BUFFER
	/* ��ת��֮ǰ�����ջָ�뱣����������ṹ������ */
	task_pcb_buf[current_id].sp = SP;
#endif
#if 0
	/* ��һ�棬�������������ã����ұ�������֮��������еĻ�ֱ������ T2_switch_exit ��ִ��*/
	for(temp = current_id + 1; temp < TASK_MAX; temp ++)
	{
		if(task_run_flag & (1 << temp))
		{
			current_id = temp;
			goto T2_switch_exit;			
		}
	}

	/* û���ҵ�������������֮������񣬷���ǰ����в��� */
	for(temp = 0; temp < current_id + 1; temp ++)
	{
		if(task_run_flag & (1 << temp))
		{
			current_id = temp;
			goto T2_switch_exit;	
		}
		else
			goto T2_switch_exit;
	}
#endif
	//��������ȼ���id��ֵ���������е�id��
	current_id = 0;
	//����id���л�
	for(temp = 1; temp < TASK_MAX; temp ++)
	{
		if(task_run_flag & (1 << temp))
		{
			if(task_pcb_buf[temp].priority < task_pcb_buf[current_id].priority)
			{
				if(task_pcb_buf[temp].delay != 0)
				{
					task_pcb_buf[temp].delay --;			
				}
				else if(task_pcb_buf[temp].suspend == 1)
					continue;
				else
				{
					current_id = temp;		
					task_pcb_buf[current_id].run_time ++;	//��������л������Ҹ������ִ��ʱ���һ������ͳ��CPUʹ����
				}		
			}
		}
	}

	/* �������0����ʱ�Ļ���������0����ʱ��һ��һ���ò����� */
	if(task_pcb_buf[0].delay != 0)
	{
		task_pcb_buf[0].delay --;			
	}
#ifdef XDATA_SP_BUFFER
	SP ++; 	//�ϸ�forѭ��������ʱ��SP��ֵ�����һ��
	sp_count = task_pcb_buf[current_id].sp;		//���ϴμ�¼��sp_count��ֵ�ٴθ�ֵ��sp_count
	/* ��xdata����洢��ջ�������¸�ֵ��ʵ�����е�ջ�ռ� */
	for(; sp_count >= 0; SP++)
	{
		((unsigned char idata*)task_sp_bottom)[sp_count] = task_sp[current_id][sp_count];
		sp_count --;
	}
	SP --;	//forѭ���˳���ʱ����Ҫ��ȥһ����
	task_pcb_buf[current_id].new_circle = 1;	//������һ�ε���
#endif
#ifdef IDATA_SP_BUFFER
	SP = task_pcb_buf[current_id].sp;	//��Ҫ���е�����SP��ַ����SP�Ĵ���
	task_pcb_buf[current_id].new_circle = 1;	//������һ�ε���
#endif
	task_runtime ++;	//ϵͳ����ʱ���1

	EA = 1;	//�����л���ɣ����¿����ж�
}

/* ϵͳ������ʱ */
void task_delay(unsigned char delay)
{
	task_pcb_buf[current_id].delay = delay;
}

/* ϵͳ�����ڿ�ʼ�ж� */
unsigned char get_task_newcircle_flag(void)
{
	if(task_pcb_buf[current_id].new_circle == 1)	//�������ڵĿ�ʼ
	{
		task_pcb_buf[current_id].new_circle = 0;	//�����ʼ��־������1
		return 1;
	}
	return 0;
}

/* ��ӡ����Դ��ʹ�� */

/* ��ջ�Ĳ��� */
void sp_test(void)
{
	SP = task_sp;

}
//	#pragma asm
//		PUSH DPL
//		PUSH DPH
//	#pragma endasm

//	void push_ptr(void)
//{
//	#pragma asm
//		PUSH ACC
//		PUSH B
//		PUSH DPL
//		PUSH DPH
//		PUSH PSW
//		PUSH 0
//		PUSH 1
//		PUSH 2
//		PUSH 3
//		PUSH 4
//		PUSH 5
//		PUSH 6
//		PUSH 7
//	#pragma endasm
//}
//

