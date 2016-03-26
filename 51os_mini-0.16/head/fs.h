/*
 * 51_MINI_OS file system head
 */

#ifndef __fs_H__
#define __fs_H__

#include <stc15f2k60s2.h>
#include "eeprom.h"
#include "uart.h"
#include "char.h"
#include "core.h"
#include "common.h"

#ifdef FS_SELECT

#define FS_BEGIN 0x5000	//�ļ�ϵͳ��ʼ��ַ
#define FS_END   0xEFFF	//�ļ�ϵͳ������ַ
#define FS_PAGE  512	//�ļ�ϵͳ��С��Ĵ�С

#define NORMAL_DIR_TYPE  0x3A	//��׼�ļ��У��ɶ���д��ɾ�����ļ��У��ѱ�ռ��
#define NORMAL_FILE_TYPE 0x38	//��׼�ı��ļ����ɶ���д��ɾ�����ļ����ѱ�ռ�� 
#define ROOT_DIR         0x32	//��Ŀ¼���ɶ���д����ɾ�����ļ��У��ѱ�ռ��

#define FS_NEED_KEEP    92	//����������֮����Ҫ�ظ��⼸������
#define FS_TOTAL_SIZE   92	//�ṹ����ܵĳ�����92�ֽ�
#define FS_FLAG_SIZE	2	//fs�ṹ������flag�ĳ��������ֽ�
#define FS_NAME_SIZE	20	//fs�ṹ������name�ĳ�����20���ֽ�
#define FS_SIZE_SIZE	2	//fs�ṹ������size�ĳ��������ֽ�
#define FS_PREV_SIZE	2	//fs�ṹ������prev�ĳ��������ֽ�
#define FS_NEXT_SIZE	2	//fs�ṹ������next�ĳ��������ֽ�
#define FS_NODE_SIZE	2*32	//fs�ṹ������node�ĳ�����32*2�ֽ�

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
/* �������ļ��ṹ����һ��ʾ����ʵ�ʲ�����ʱ����ʹ��IAPֱ��д�� */

/* ��ֵ�洢ָ���ַ����ָ��ָ��ǰ�ļ��� */
extern xdata unsigned int fs_struct_ptr;

/* ��ֵ�洢ָ���ַ����ָ��ָ���һ���յ��ڴ�飬�ӿ�ϵͳ�ڴ������� */
extern xdata unsigned int first_empty_page;

extern void fs_format(void);
extern void fs_write(unsigned char *name);
extern void fs_read(unsigned char *name);
extern unsigned int alloc(unsigned int len, unsigned int *pos);
extern unsigned char create_dir_file(unsigned char *name, unsigned int addr, unsigned int prev_addr, unsigned char flag);
extern void list_file(unsigned int cur_f_ptr, unsigned char l_flag);
extern unsigned char change_dir(unsigned char *name);
extern unsigned char delete_file(unsigned char *name);

#endif

#endif