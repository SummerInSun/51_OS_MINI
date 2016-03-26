#include "hc_sr04.h"

static void sr04_delay(unsigned int period)
{
	xdata unsigned int i;

	for(i = 0; i < period; i ++)
	{
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
		_nop_(); _nop_(); _nop_(); _nop_(); _nop_(); _nop_();
	}
}

static void sr04_timer_init(void)
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0;		//���ö�ʱ��ֵ
	TH0 = 0;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
}

unsigned int get_distance(void)
{
	xdata unsigned char i;

	sr04_timer_init();
	for(i = 0; i < 8; i ++)
	{
		HC_TX = 1;
		sr04_delay(2);
		HC_TX = 0;
		sr04_delay(2);
	}
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	while((HC_RX == 1) && (TF0 == 0));
	TR0 = 0;		//�رռ�ʱ
	if(TF0 == 1)
	{
		TF0 = 0;
		return 255;
	}
	return((TH0 * 256 + TL0) * 17 / 1000);
}