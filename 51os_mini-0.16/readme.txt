�����ļ�ϵͳ��51΢�Ͳ���ϵͳ

1����������
��Ƭ����IAP15F2K61S2
IDE��keil4�����̱����Ż�����2��
���ع��ߣ�stc-isp-15xx-v6.85I��24MHz��115200�����ʣ�
���ڹ��ߣ�SecureCRT 7.2

2������ϵͳ����
�ںˣ���ռʽ�ںˡ�֧�����ȼ�256������ʱ���ṩ���ڽ����½��Զ������ȼ������񣩡�֧��������ʱ/�������������������32������������
�����е��½���ɾ�����ļ������ݲ�֧���ź��������ڴ�ӡδ������---һ���޿ӣ�
�ļ�ϵͳ��֧���ı��ļ����ļ��еĴ�����ɾ�����ı��ļ�������508���ֽڣ����ļ���ϸ���͵Ĳ鿴���ļ�ϵͳ��ʽ��

3���ļ�˵��
���̸�Ŀ¼->
board	������й������ļ�
head	����ͷ�ļ�
main	���������Ŀ¼
pro		�����ļ�
src		Դ�ļ�

board->
head	ͷ�ļ� -->
	18b20.h		�¶ȴ�����
	at24c02.h	eeprom�洢оƬ
	ds1302.h	ʱ��оƬ
	hc_sr04.h	������
	lcd1602.h	Һ��
	nt_led.h	������Լ�led��
src		Դ�ļ� -->
	18b20.c		�¶ȴ�����
	at24c02.c	eeprom�洢оƬ
	ds1302.c	ʱ��оƬ
	hc_sr04.c	������
	lcd1602.c	Һ��
	nt_led.c	������Լ�led��
	
head->
char.h		���ַ��������йص�ͷ�ļ��������ִ��Աȵȵ�
core.h		�ںˣ�������ȵ�
common.h	�꿪��
eeprom.h	��Ƭ���Դ�EEPROM������
fs.h		�ļ�ϵͳ
task.h		��������
uart.h		���ڲ���
vector.h	��ʱ������

src->
char.c		���ַ��������йص�ͷ�ļ��������ִ��Աȵȵ�
core.c		�ںˣ�������ȵ�
eeprom.c	��Ƭ���Դ�EEPROM������
fs.c		�ļ�ϵͳ
task.c		��������
uart.c		���ڲ���
vector.c	��ʱ������

4��ָ��ϵͳ
����Ҫ����help�ˣ�ֱ�����п�����ʾ���е�������Ϣ

1.ps:   ��ʾ�������е�����
2.kill: ɱ��ѡ�е��������е�����
        example:
                kill 4 ---kill the 4th task
3.new:  ����һ������
        example:
                new task2 ---create the task named 'task2'
4.ls:   ��ʾ��ǰ�ļ�������������ļ�������lѡ���ʱ�����ʾ�ļ����ͣ�����Ȩ�ޡ��ļ����ڴ��е�λ��
        example:
                ls l ---display in detail
5.mkdir:����һ���ļ���
        example:
                mkdir dir ---make a floder named 'dir'
6.touch:����һ���ı��ļ�
        example:
                touch txt ---make a file named 'txt'
7.cd:   �л��ļ���
        example:
                cd dir ---change to dir named 'dir'
8.mkfs: ��ʽ������ϵͳ
9.rm:   ɾ��ѡ�е��ļ� 
        example: 
                rm dir ---delete the file named 'dir'
10.vi:  �༭ѡ�е��ı��ļ�
        example:
                 vi txt ---edit the file named 'txt'
11.cat: �鿴�ı��ļ�����
        example:
                cat txt ---look up the file named 'txt'
12.exit:�˳�����ϵͳ

5�����Ի�����
1��������һ���Լ�������ϵͳ��
1.1����src/task.c�ļ�������Լ�����������Ϊvoid task_tty0(void)��ʽ
1.2����src/task.c��static void new_msghandler(void *msg)�����м������ƣ�
else if(!os_strcmp("�����������", cmd_split[1]))
{
	CREATE_TASK_RUNNING(���ȼ�, �����������, "�����������");
	printf("The task has been created : task name = ");
	printf(cmd_split[1]);
	printf("\r\n");
}�����
1.3����main/main.c��������ƣ�
task_create(���ȼ�, (unsigned int)(&�����������), 0, "�����������");�����
1.4����head/core.h��
#define IDATA_SP_BUFFER		//ջ���ݻ�������ѡ��
//�ɸ�ΪXDATA_SP_BUFFER������������������Ϊidata�����Ȳ�����
#define TASK_MAX 5 			//�����е�������������Ĵ�����������
#define SP_DEEP  30			//�����ջ�����
#define PUBLIC_SP_DEEP 30	//����Ĺ�����ջ�ռ���ȣ����궨��XDATA_SP_BUFFERʱ��Ч�����õ�ͬ��SP_DEEP��
����������������ļ�ϵͳ֮���ΪXDATA���������г����ˣ������޷����������ˣ�
  ���ǿ�����������ֻ���ں˵��Ǹ��汾�����ӣ�������head/common.h�иı�궨�����ע�ͺ궨�弴�ɣ�����
ע�⣺���ڲ��ṩ���ȼ��Զ�ѡ���������ֶ�ѡ�����Ѵ������ȼ���һ�������ȼ�������ԽС�������ȼ�Խ��
	  �����ڴ���������������ؼ���{OS_delay(100); TF1 = 1;}������ʽ����䣬��������ִ�п��е�ʱ�����ϵͳ��ʱ�Թ��������ȼ��������ֻ�
	  ���������ʱ�������Ƕ�׵��ò���������Ҫ����6�㣬�����п��ܻ�������г��������Ҫ����������������ΪXDATA_SP_BUFFER������
	  ��PUBLIC_SP_DEEP�����Ĵ�һ��
2��������һ���Լ������ϵͳ��ȥ
2.1����code unsigned char cmd_buff[][100]�м����Լ����������
2.2����src/task.c��������ƣ�
/* 'cd 'dirname'' ���л��ļ��� */
static void cd_msghandler(void *msg)
{
	change_dir(cmd_split[1]);
}�Ĵ���
2.3����src/task.c��void task_msghandler(void)���������
else if(!os_strcmp("cd", cmd_split[0]))
{
	cd_msghandler(0);
}�����

ע�⣺��Ҫʹ�������������100�ֽ�

6����ν���Դ�����Ķ�
ȫ���ļ�������Ҫ�ļ����ļ���task.c�Լ�core.c����fs.c
�������ǵ�ͷ�ļ�
core.c������Ҫ��ע
unsigned char task_kill(unsigned char pid);	//�����ɾ��
unsigned char task_create(unsigned char priority, unsigned int task_address, unsigned char xdata *task_msg, unsigned char *name);	//����Ĵ���
void task_switch(void);	//������л�

task.c����ֻ��Ҫ��ע
extern void task_msghandler(void);	//�Կ���̨�������Ϣ���д���
extern void task_tty0(void);		//�������̨�����û�����
�����Ѿ�������һ�����µĲ���ϵͳ�ںˣ�������ļ�ϵͳ���Ǹ��ӵĿɿ��ɲ���

fs.c��
/* �ļ�ϵͳ�����ͣ�����ֻ��һ���򵥵��ļ����ͣ���һ��˫������Ľṹ
 * һ���ڴ��Ĵ�СΪ128byte
 * �ļ�ϵͳ��0x5000��ʼ��0xEFFF������һ����40K��С
 **************************��Ҫ***********************************
 *�˽ṹ����ʹ��IAP������ʱ��Ҫ��ѭint�ĵ��ֽ���ǰ���ֽ��ں��Լ��
 *****************************************************************
 */
struct fs_type							//111:�ɶ���дɾ��				  // 01:�ļ���
{			//0 0 0 0 0 0 0 0 0 0       0 0 0                          00(00:�ļ�)          0(1:δ��ռ��)
	unsigned int flag;		//        ���ļ�Ȩ�ޣ�                 �ļ���type��         �˿��Ƿ�ռ��,Ҫ���ֽ�
	unsigned char name[20];	//�ļ�������Ϊ20
	unsigned int size;		//�ļ���С
	/* ����˫��ṹ */
	struct fs_type *prev;
	struct fs_type *next;
	unsigned int inode[32];	//inode�ĸ���Ϊ32��Ҳ����һ���ļ������������32���ļ�
	//һ���ļ��Ĵ�С�����32*128�ֽ�
};
unsigned int alloc(unsigned int len, unsigned int *pos);	//���ڴ浱�з���ռ�
void fs_format(void);	//��ʽ������ϵͳ				
����mkfs�����йأ�
unsigned char create_dir_file(unsigned char *name, unsigned int addr, unsigned int prev_addr, unsigned char flag);	//�����ļ�
����mkdir��touch�����йأ�
void list_file(unsigned int cur_f_ptr, unsigned char l_flag);	//�г���ǰ�ļ����������е��ļ�
����ls�����йأ�
unsigned char change_dir(unsigned char *name);	//�л��ļ���
����cd�����йأ�
void fs_write(unsigned char *name);	//�༭�ı��ļ�
����vi�����йأ�
void fs_read(unsigned char *name);	//���ı��ļ�
����cat�����йأ�
unsigned char delete_file(unsigned char *name);	//ɾ���ļ�
����rm�����йأ�

ÿһ����������Ҫ�Ķ����£�˳�������Ѿ��źã���˳�򿴻�Ƚ��������һ��