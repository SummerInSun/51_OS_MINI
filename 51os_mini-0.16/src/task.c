#include "task.h"


/* ��������ֶ� */
code unsigned char cmd_buff[][100] = {
	"1.ps:\tdisplay the running tasks\r\n",
	"2.kill:\tkill the task you have selected\r\n\texample:\r\n\t\tkill 4 ---kill the 4th task\r\n",
	"3.new:\tcreate a task\r\n\texample:\r\n\t\tnew task2 ---create the task named 'task2'\r\n",
#ifdef FS_SELECT
	"4.ls:\tdisplay the files in the current floder\r\n\texample:\r\n\t\tls l ---display in detail\r\n",
	"5.mkdir:\tcreate a floder\r\n\texample:\r\n\t\tmkdir dir ---make a floder named 'dir'\r\n",
	"6.touch:\tcrate a text file\r\n\texample:\r\n\t\ttouch txt ---make a file named 'txt'\r\n",
	"7.cd:\tchange the current floder\r\n\texample:\r\n\t\tcd dir ---change to dir named 'dir'\r\n",
	"8.mkfs:\tformat all the file system\r\n",
	"9.rm:\tdelete file \r\n\texample: \r\n\t\trm dir ---delete the file named 'dir'\r\n",
	"10.vi:\tedit text file\r\n\texample:\r\n\t\t vi txt ---edit the file named 'txt'\r\n",
	"11.cat:\tlook text file\r\n\texample:\r\n\t\tcat txt ---look up the file named 'txt'\r\n",
#endif
	"12.exit:\texit all this system\r\n",
	"\0"
};

/* �����ִ�������־,������ɱ�־��0,���ڽ�����־��1 */
xdata unsigned char parse_commond_flag = 0;

/* �ȴ�������� */
static void wait_parse(void)
{
	parse_commond_flag = 1;

	while(parse_commond_flag)
	{
		task_delay(2);	//�������ȴ�һ������������ѯ�����Ƿ�������
		TF1 = 1;			//�������һ���ж�
	}

}

/* ���е���Ϣ����ģ�� */
//'ps'
static void ps_msghandler(void *msg)
{
	xdata unsigned char i = 0;
	xdata unsigned char mod = 0;

/* ��ӡ�������е������Ҫ */
	printf("There are \"");
	uart_send_byte(task_running + 48);
	printf("\" tasks are running.");
/* ��ӡCPU�ӿ������������е�ʱ�� */
	printf("\t Cpu has run \"");
	putchar(task_runtime / 10000000000+ 48);
	putchar(task_runtime % 1000000000 / 1000000000 + 48);
	putchar(task_runtime % 100000000 / 10000000 + 48);
	putchar(task_runtime % 10000000 / 1000000 + 48);
	putchar(task_runtime % 1000000 / 100000 + 48);
	putchar(task_runtime % 100000 / 10000 + 48);
	putchar(task_runtime % 10000 / 1000 + 48);
	putchar(task_runtime % 1000 / 100 + 48);
	putchar(task_runtime % 100 / 10 + 48);
	putchar(task_runtime % 10 + 48);
	printf("\" times\r\n");
	uart_send_str("pid\tpriority\tcpu_use\tname\r\n\r\n");
/* ������ѯ�������е����񲢴�ӡ��������ϸ����� */
	for(i = 0; i < TASK_MAX; i ++)
	{
		if(task_run_flag & (1 << i))
		{
			mod = (task_pcb_buf[i].run_time) * 100 / task_runtime;
			putchar(i + 48);
			putchar('\t');
			putchar(task_pcb_buf[i].priority / 100 + 48);
			putchar(task_pcb_buf[i].priority % 100 / 10 + 48);
			putchar(task_pcb_buf[i].priority % 10 + 48);
			printf("\t\t");
			putchar(mod / 10 + 48);
			putchar(mod % 10 + 48);
			putchar('\t');
			printf(task_pcb_buf[i].id);
			uart_send_str("\r\n");
		}
	}

//	uart_send_str("You do a 'test' commond\r\n");
}
//'kill	'pid''
static void kill_msghandler(void *msg)
{		
	if(cmd_split[1][0] != '2' && cmd_split[1][0] != '3' && cmd_split[1][0] != '0')
	{
		if(task_kill(cmd_split[1][0] - 48))
		{
			printf("The task don't exit\r\n");
		}
		else
		{
			uart_send_str("The task has been killed : pidnum = ");
			uart_send_byte(cmd_split[1][0]);
			uart_send_str("\r\n");
		}
	}
	else
	{
		uart_send_str("The task you select can't be killed\r\n");
	}
}
/* 'new 'pid'' �½�һ������*/
static void new_msghandler(void *msg)
{	
	xdata unsigned char i;
	
	for(i = 0; i < TASK_MAX; i ++)
	{
		if(task_run_flag & (1 << i))
		{
			if(!os_strcmp(cmd_split[1], task_pcb_buf[i].id))
			{
				printf("The task has been exist\r\n");
				return;
			}
		}
	}
	
	if(!os_strcmp("task1", cmd_split[1]))
	{
		CREATE_TASK_RUNNING(2, task1, "task1");
		printf("The task has been created : task name = ");
		printf(cmd_split[1]);
		printf("\r\n");
	}
	else if(!os_strcmp("idle", cmd_split[1]))
	{
		CREATE_TASK_RUNNING(255, idle, "idle");
		printf("The task has been created : task name = ");
		printf(cmd_split[1]);
		printf("\r\n");
	}
	else if(!os_strcmp("task2", cmd_split[1]))
	{
		CREATE_TASK_RUNNING(3, task2, "task2");
		uart_send_str("The task has been created : task name = ");
		uart_send_str(cmd_split[1]);
		uart_send_str("\r\n");
	}
	else
	{
		uart_send_str("The task you select can't be created\r\n");
	}
}
#ifdef FS_SELECT
/* 'ls' �г���ǰ�ļ����µ��ļ� */
static void ls_msghandler(void *msg)
{	
//	xdata unsigned char i;

	if(!os_strcmp("", cmd_split[1]))
	{
		list_file(fs_struct_ptr, 0);
	}
	else if(!os_strcmp("l", cmd_split[1]))
	{
		list_file(fs_struct_ptr, 1);
	}
	else
	{
		printf("Unknow target after 'ls'");
	}
	
	printf("\r\n");
}

/* 'mkdir 'dirname'' �½�һ���ļ��� */
static void mkdir_msghandler(void *msg)
{	
	xdata unsigned char i;
	xdata unsigned int pos_mod[20];

	i = check_buff(cmd_split[1]);
	if(i == 1)
	{
		printf("There are some invalid words in the dir name string\r\n");
		return;
	}
	else if(i == 2)
	{
		printf("Name too large\r\n");
		return;
	}

	if(alloc(FS_PAGE, pos_mod))
	{			
		if(create_dir_file(cmd_split[1], pos_mod[0], fs_struct_ptr, NORMAL_DIR_TYPE))
		{
			printf("Create error, the same name or no more nodes\r\n");
		}
	}
	else
	{
		printf("No more space to create a dir\r\n");
	}
}

/* 'touch 'filename'' �½�һ���ı��ļ� */
static void touch_msghandler(void *msg)
{	
	xdata unsigned char i;
	xdata unsigned int pos_mod[20];

	i = check_buff(cmd_split[1]);
	if(i == 1)
	{
		printf("There are some invalid words in the file name string\r\n");
		return;
	}
	else if(i == 2)
	{
		printf("Name too large\r\n");
		return;
	}

	if(alloc(FS_PAGE, pos_mod))
	{			
		if(create_dir_file(cmd_split[1], pos_mod[0], fs_struct_ptr, NORMAL_FILE_TYPE))
		{
			printf("Create error, the same name or no more nodes\r\n");
		}
	}
	else
	{
		printf("No more space to create a file\r\n");
	}
}

/* 'cd 'dirname'' ���л��ļ��� */
static void cd_msghandler(void *msg)
{
	change_dir(cmd_split[1]);
}

/* 'mkfs' ����ʽ���������ļ�ϵͳ */
static void mkfs_msghandler(void *msg)
{
	fs_format();
}
 
/* 'rm 'filename'' ��ɾ���ļ� */
static void rm_msghandler(void *msg)
{
	ENTER_CRITICAL;
	delete_file(cmd_split[1]);
	EXIT_CRITICAL;
}

/* 'vi 'filename'' ���༭�ļ� */
static void vi_msghandler(void *msg)
{
	fs_write(cmd_split[1]);
}

/* 'cat 'filename'' ����ȡ�ļ� */
static void cat_msghandler(void *msg)
{
	fs_read(cmd_split[1]);
	printf("\r\n");	//����
}

#endif

/* 'help' ����ȡ�����ļ� */
static void help_msghandler(void *msg)
{
	unsigned char i = 0, j = 0;

	ENTER_CRITICAL;
	while(cmd_buff[i][0])
	{
		while(cmd_buff[i][j] != '\0')
		{
			putchar(cmd_buff[i][j]);
			j ++;
		}
		i ++;
		j = 0;
	}
	EXIT_CRITICAL;
}

/* 'exit' ���˳�����ϵͳ */
static void exit_msghandler(void *msg)
{
	EA = 0;	//�������жϼĴ������ر�����ϵͳ,ʹ��Ƭ���������ģʽ����
	TR1 = 0;
	ET1 = 0;
	while(1);
}
  
void task1(void)
{	
	while(1)
	{
		ENLED = 0;
		P2 = 0x06;
		P00 = ~P00;
		OS_delay(500);
	}		
}

/* ϵͳ�����������ȼ���ͣ��������ڲ��ɽ����κβ��� */
void idle(void)
{	
	while(1)
	{

	}
}

void task2(void)
{	
	while(1)
	{
		ENLED = 0;
		P2 = 0x06;
		P06 = ~P06;
		OS_delay(50);
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
				TF1 = 1;	//�������һ���ж�
			}
			else
				break;
		}

		if(reve_buf_len >= 254)
		{
			
			printf("\r\nToo much words\r\n");		//�û�������ַ���������
			printf("Please input your commond again and dont't more than 254\r\n#");		
//			clear_char_str(reve_buff, 256);		//����������飬��ʵ������������Ӱ�죬�˴�����ע�͵�
			reve_buf_len = 0;
		}	
		else
		{
			if(reve_buff[reve_buf_len] == '\b')
			{
				if(reve_buf_len > 0)
				{
					reve_buf_len --;
					printf("\b \b");
				}
			}
			else if(reve_buff[reve_buf_len] == '\r')	  	//���յ����з�
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
		
		split_str(reve_buff, cmd_split);
		
		if(!os_strcmp("kill", cmd_split[0]))
		{
			kill_msghandler(0);
		}
		else if(!os_strcmp("new", cmd_split[0]))
		{
			new_msghandler(0);
		}
		else if(!os_strcmp("ps", cmd_split[0]))
		{
			ps_msghandler(0);
		}
#ifdef FS_SELECT
		else if(!os_strcmp("ls", cmd_split[0]))
		{
			ls_msghandler(0);
		}
		else if(!os_strcmp("mkdir", cmd_split[0]))
		{
			mkdir_msghandler(0);
		}
		else if(!os_strcmp("touch", cmd_split[0]))
		{
			touch_msghandler(0);
		}
		else if(!os_strcmp("cd", cmd_split[0]))
		{
			cd_msghandler(0);
		}
		else if(!os_strcmp("mkfs", cmd_split[0]))
		{
			mkfs_msghandler(0);
		}
		else if(!os_strcmp("rm", cmd_split[0]))
		{
			rm_msghandler(0);
		}
		else if(!os_strcmp("vi", cmd_split[0]))
		{
			vi_msghandler(0);
		}
		else if(!os_strcmp("cat", cmd_split[0]))
		{
			cat_msghandler(0);
		}
#endif
		else if(!os_strcmp("help", cmd_split[0]))
		{
			help_msghandler(0);
		}
		else if(!os_strcmp("exit", cmd_split[0]))
		{
			exit_msghandler(0);
		}
		else if(os_strcmp("", cmd_split[0]))
		{
			uart_send_str(cmd_split[0]);
			uart_send_str(": undefined commond\r\n");
		}
				
		uart_send_str("#");	//������ʾ��
		parse_commond_flag = 0;	//����������
		SUSPEND_TASK(TASK_MSGHANDLER);	//�Թ���Ӧ���ȹ����ٵȴ�
		TF1 = 1;			//���̲���һ���жϣ�������ִ�е�����	
	}
}