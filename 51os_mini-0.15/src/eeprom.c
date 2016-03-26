#include <eeprom.h>
#include <common.h>

/*****************************************
 * EEPROMд������
 * ��:iap_write(10,'A'); ���ַΪ10��EEPROMд������A
 *****************************************/
void iap_write(uint address, uchar wdata)
{
	clear_iap;
	if(!check_iap)						//�жϵ�ѹ�Ƿ���������
	{
		IAP_CONTR = 0x01;				//24M��Ϊ1
		IAP_CONTR |= 0x80;				//ʹ��
		IAP_CMD   = CMD_WRITE;          //ѡ������Ϊ��д���
		IAP_ADDRH = address >> 8;		//д��ַ��λ
		IAP_ADDRL = address;		    //д��ַ��λ
	
		IAP_DATA  = wdata;
		IAP_TRIG=0x5A;IAP_TRIG=0XA5;
		_nop_();_nop_();				//����������д������

	}
}

/*****************************************
 * EEPROM��������
 * ��:dat = iap_read(10); ����ַΪ10��EEPROM����dat
 *****************************************/
uchar iap_read(uint address)
{
	uchar rdata;

	clear_iap;
	if(!check_iap)		                //�жϵ�ѹ�Ƿ���������
	{
		IAP_CONTR = 0x01;				//24M��Ϊ1
		IAP_CONTR |= 0x80;				//ʹ��
		IAP_CMD   = CMD_READ;           //ѡ������Ϊ��д���
		IAP_ADDRH = address >> 8;		//д��ַ��λ
		IAP_ADDRL = address;		    //д��ַ��λ
		
		IAP_TRIG=0x5A;IAP_TRIG=0XA5;
		_nop_();_nop_();				//����������������������
		rdata     = IAP_DATA;

	}

	return rdata;
}

/*****************************************
 * EEPROM��������
 * ��:iap_erase(0); ����EEPROM����
 *****************************************/
void iap_erase(uint address)
{
	clear_iap;
	if(!check_iap)						//�жϵ�ѹ�Ƿ���������
	{
		IAP_CONTR = 0x01;				//24M��Ϊ1
		IAP_CONTR |= 0x80;				//ʹ��
		IAP_CMD   = CMD_ERASE;          //ѡ������Ϊ��д���
		IAP_ADDRH = address / 256;		//д��ַ��λ
		IAP_ADDRL = address % 256;		//д��ַ��λ
		
		IAP_TRIG=0x5A;IAP_TRIG=0XA5;
		_nop_();_nop_();				//����������������������

	}
}