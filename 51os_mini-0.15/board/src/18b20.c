#include "18b20.h"

static void b20_delay(unsigned int period)
{
	xdata unsigned int i;

	for(i = 0; i < period; i ++)
	{
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	}
}

static void b20_reset(void)
{
	DQ = 0;
	b20_delay(500);
	DQ = 1;
	while(DQ);
	while(!DQ);
	b20_delay(1);
}

static void b20_writebyte(unsigned char dat)
{
	xdata unsigned char i = 0;

	for(i = 0; i < 8; i ++)
	{
		DQ = 0;
		b20_delay(1);
		DQ = dat & 0x01;
		b20_delay(70);
		DQ = 1;
		dat = dat >> 1;
		b20_delay(1);
	}
	b20_delay(50);
}

static unsigned char b20_readbyte(void)
{
	xdata unsigned char i = 0,dat = 0;

	for(i = 0; i < 8; i ++)
	{
		DQ = 0;
		b20_delay(2);
		DQ = 1;
		b20_delay(2);
		dat = dat >> 1;
		if(DQ)
		{
			dat = dat | 0x80;
		}
		DQ = 1;
		b20_delay(50);		
	}

	return dat;
}

unsigned char get_temp(void)
{
	xdata unsigned char low,high;
	xdata unsigned char temp;
	
	EA = 0;
	b20_reset();
	b20_writebyte(0xCC);
	b20_writebyte(0x44); //�����¶�ת��
//	b20_delay(500);
	
	b20_reset();
	b20_writebyte(0xCC);
	b20_writebyte(0xBE); //��ȡ�Ĵ���
	low = b20_readbyte(); //���ֽ�
	high = b20_readbyte(); //���ֽ�
	
	temp = high<<4;
	temp |= (low>>4);
	EA = 1;
	return temp;
}