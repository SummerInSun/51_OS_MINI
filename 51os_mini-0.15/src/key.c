#include "key.h"

//�����ṹ��
KEY_STRUCT key_struct;

/*****************************************
 *��������ʼ��
 *��:key_init() //��ʼ��������״̬Ϊδ���� 
 *���У�������������
 *****************************************/
void key_init(void)
{
	key_struct.key1_down = 0;
	key_struct.key2_down = 0;
	key_struct.key3_down = 0;
	key_struct.key4_down = 0;
	key_struct.key5_down = 0;
	key_struct.key6_down = 0;

	key_struct.key_down_flag_turn = 0;
	key_struct.key_up_flag_turn = 0;
}

/*****************************************
 *�������
 *��:key_check() //���һ�ΰ�����״̬ 
 *���У�������������
 *****************************************/
void key_check(void)
{
	P3 &= ~(1 << 7);	
	P3 |= (1 << 6);	//�Ѱ����ĵ�һ����Ϊ1���ڶ�����Ϊ0
	_nop_();_nop_();_nop_();_nop_();
	key_struct.key_highcode = (P0 >> 5) ^ 0x07;	//����һ�вɼ����ļ������ṹ���Դ������½ṹ�� 
	P3 &= ~(1 << 6);
	P3 |= (1 << 7);	//�Ѱ����ĵ�һ����Ϊ0���ڶ�����Ϊ1
	_nop_();_nop_();_nop_();_nop_();
	key_struct.key_lowcode  = (P0 >> 5) ^ 0x07; 	//���ڶ��вɼ����ļ������ṹ���Դ������½ṹ��
	//���ߵ����������ϳ�һ��������
	key_struct.key_long_code = (((1 << 3) | (key_struct.key_highcode)) << 8) | ((1 << 4) | (key_struct.key_lowcode));
}									    

/*****************************************
 *����״̬���
 *��:key_stat_out() //���һ�ΰ�����״̬ 
 *���У�������������
 *****************************************/
void key_stat_out(void)
{
	if((key_struct.key_highcode & 0x01) && (!key_struct.key4_down))
	{
		uart_send_str("key 4 down!\r\n");
		key_struct.key4_down = 1;
		key_struct.key_down_flag_turn = ~key_struct.key_down_flag_turn; 
	}
	if((key_struct.key_highcode & 0x02) && (!key_struct.key5_down))
	{
		uart_send_str("key 5 down!\r\n");
		key_struct.key5_down = 1;
		key_struct.key_down_flag_turn = ~key_struct.key_down_flag_turn;
	}
	if((key_struct.key_highcode & 0x04) && (!key_struct.key6_down))
	{
		uart_send_str("key 6 down!\r\n");
		key_struct.key6_down = 1;
		key_struct.key_down_flag_turn = ~key_struct.key_down_flag_turn;
	}

	if((1 == key_struct.key4_down) && (!(key_struct.key_highcode & 0x01)))
	{
		uart_send_str("key 4 up!\r\n");
		key_struct.key4_down = 0;
		key_struct.key_up_flag_turn = ~key_struct.key_up_flag_turn;
	}
	if((1 == key_struct.key5_down) && (!(key_struct.key_highcode & 0x02)))
	{
		uart_send_str("key 5 up!\r\n");
		key_struct.key5_down = 0;
		key_struct.key_up_flag_turn = ~key_struct.key_up_flag_turn;
	}
	if((1 == key_struct.key6_down) && (!(key_struct.key_highcode & 0x04)))
	{
		uart_send_str("key 6 up!\r\n");
		key_struct.key6_down = 0;
		key_struct.key_up_flag_turn = ~key_struct.key_up_flag_turn;
	}


	if((key_struct.key_lowcode & 0x01) && (!key_struct.key1_down))
	{
		uart_send_str("key 1 down!\r\n");
		key_struct.key1_down = 1;
		key_struct.key_down_flag_turn = ~key_struct.key_down_flag_turn;
	}
	if((key_struct.key_lowcode & 0x02) && (!key_struct.key2_down))
	{
		uart_send_str("key 2 down!\r\n");
		key_struct.key2_down = 1;
		key_struct.key_down_flag_turn = ~key_struct.key_down_flag_turn;
	}
	if((key_struct.key_lowcode & 0x04) && (!key_struct.key3_down))
	{
		uart_send_str("key 3 down!\r\n");
		key_struct.key3_down = 1;
		key_struct.key_down_flag_turn = ~key_struct.key_down_flag_turn;
	}

	if((1 == key_struct.key1_down) && (!(key_struct.key_lowcode & 0x01)))
	{
		uart_send_str("key 1 up!\r\n");
		key_struct.key1_down = 0;
		key_struct.key_up_flag_turn = ~key_struct.key_up_flag_turn;
	}
	if((1 == key_struct.key2_down) && (!(key_struct.key_lowcode & 0x02)))
	{
		uart_send_str("key 2 up!\r\n");
		key_struct.key2_down = 0;
		key_struct.key_up_flag_turn = ~key_struct.key_up_flag_turn;
	}
	if((1 == key_struct.key3_down) && (!(key_struct.key_lowcode & 0x04)))
	{
		uart_send_str("key 3 up!\r\n");
		key_struct.key3_down = 0;
		key_struct.key_up_flag_turn = ~key_struct.key_up_flag_turn;
	}
}

/*****************
 *������ʹ��ʾ��
 *�Ƽ�ɨ��ʱ����Ϊ10ms
 *****************/
/*
void main()
{
	uart_bart11(57600); 		//���ô��ڲ�����Ϊ57600
	key_init();
	timer0_init11(10000);		//��ʼ������0����ʱ10ms���ڼ�ⰴ��
}

void key_scan() interrupt 1
{
	key_check();				//�Ƚ���һ��ɨ��
	key_stat_out();				//״̬���
	timer0_update11(10000);		//װ����һ����Ҫ��ʱ�ļ����
}
*/
















