#include "common.h"

/*****************************************
 *������ʱ����
 *��:delay_ms(1000);    ��ʱ1000���� 
 *****************************************/
void delay_ms(unsigned int ms)
{
	unsigned int onetime = 500;

	while(ms --)
	{
		while(onetime --);
		onetime = 500;
	}
}

/*****************************************
 *΢����ʱ����
 *��:delay_us(1000);    ��ʱ1000΢�� 
 *****************************************/
void delay_us(unsigned int us)
{
	while(us --)
	{
		_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();
		
	}
}