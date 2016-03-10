#include "task.h"

/* �����ִ�������־,������ɱ�־��0,���ڽ�����־��1 */
xdata unsigned char parse_commond_flag = 0;

/* �ȴ�������� */
void wait_parse(void)
{
	parse_commond_flag = 1;

	while(parse_commond_flag)
	{
		task_delay(2);	//�������ȴ�һ������������ѯ�����Ƿ�������
	}

}

void task1(void)
{
	xdata unsigned char i = 1;

	P0 = 0x01;
	OS_delay(1000);
	while(1)
	{
		if(P0 == 0x80)
		{
			P0 = 0x01;
		}
		else
			P0 = P0 << 1;
		OS_delay(50);		
	}
		
}

void task0(void)
{
	while(1)
	{

	}
}

/* ���ڽ����ַ������л��ԣ������ַ�������һ����������֮����������Ϣ����Ϣ������������ִ�� */
void task_tty0(void)
{
	static xdata unsigned int reve_buf_len = 0;	//��ʼ�����ڽ��յ������ݴ�����Ϊ0

	printf("#");   	//������ӡ����ʾ��

	while(1)
	{
		while(1)
		{	
			reve_buff[reve_buf_len] = uart_reve_byte();	//���յ�һ���ַ�����ȡ�����ջ���������
			if(reve_buff[reve_buf_len] == 0) 			//���û�н��յ��ִ���������ѯ
			{
				task_delay(2);							//��ʱ�����������ڣ��Ա������������ִ��
			}
			else
				break;
		}

		if(reve_buf_len >= 254)
		{
			
			printf("\r\nToo much words\r\n");		//�û�������ַ���������
			printf("Please input your commond again and dont't input too much\r\n#");		
//			clear_char_str(reve_buff, 256);		//����������飬��ʵ������������Ӱ�죬�˴�����ע�͵�
			reve_buf_len = 0;
		}	
		else
		{
			if(reve_buff[reve_buf_len] == '\r')	  	//���յ����з�
			{
				reve_buff[reve_buf_len] = '\0';		//����ǰ�Ļ����ַ��滻Ϊ�ַ��������������Ƚ�
				DESUSPEND_TASK(TASK_MSGHANDLER);	//��Ϣ�����������
				wait_parse();						//�ȴ�����������
//				clear_char_str(reve_buff, 256);		//����������飬��ʵ������������Ӱ�죬�˴�����ע�͵�
				reve_buf_len = 0;					//�ִ���������0�Ա����½�������
			}
			else
			{
				uart_send_byte(reve_buff[reve_buf_len]);	//���ոս��յ���һ���ֽڲ��ҷ��͸����������ʾ
				reve_buf_len ++;							//�ִ�����ֵ��1
			}
			
		}
	}	
}

/* ��Ϣ���������Դ��ڽ��յ������������Ӧ�Ĵ��� */
void task_msghandler(void)
{
	while(1)
	{
		uart_send_str("\r\n");
		if(!os_strcmp("test", reve_buff))
		{
			uart_send_str("You do a 'test' commond");
		}
		else if(!os_strcmp("kill", reve_buff))
		{
			uart_send_str("You do a 'kill' commond, please input the pid number");
		}
		else
		{
			uart_send_str(reve_buff);
			uart_send_str(": undefined commond");
		}
		
		uart_send_str("\r\n#");	//������ʾ��
		parse_commond_flag = 0;	//����������
		SUSPEND_TASK(TASK_MSGHANDLER);	//�Թ���Ӧ���ȹ����ٵȴ�
		OS_delay(10);			//�ȴ�һ���������ڣ�������ִ�е�����	
	}
}