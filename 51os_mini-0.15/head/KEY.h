#ifndef __key_h__
#define __key_h__

#include <stc10f08xe.h>
#include <intrins.h>
#include <uart.h>

typedef struct
{
	uchar	key_highcode;
	uchar	key_lowcode;
	uint	key_long_code;

	uchar	key1_down;
	uchar	key2_down;
	uchar	key3_down;
	uchar	key4_down;
	uchar	key5_down;
	uchar	key6_down;

	uchar   key_down_flag_turn;		//����һ���������µı�־��ÿ�ΰ��µ�ʱ�򶼻ᷭת���Դ����������һ�������İ��²�������ض���
	uchar   key_up_flag_turn;		//����һ�������ɿ��ı�־��ÿ���ɿ���ʱ�򶼻ᷭת���Դ����������һ���������ɿ���������ض���

}KEY_STRUCT;

extern KEY_STRUCT key_struct;

extern void key_check(void);
extern void key_stat_out(void);
extern void key_init(void);

#endif