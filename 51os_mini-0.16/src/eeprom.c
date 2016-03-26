#include <eeprom.h>
#include <common.h>

/*****************************************
 * �ر�IAP
 * ��:iap_idle();
 *****************************************/
static void iap_idle(void)
{
	IAP_CONTR = 0;		//�ر�IAP����
	IAP_CMD   = 0;		//�������Ĵ���
	IAP_TRIG  = 0;		//��������Ĵ���
//	IAP_ADDRH = 0x00;	//����ַ���õ���IAP����
//	IAP_ADDRL = 0x00;
}

/*****************************************
 * EEPROMд������
 * ��:iap_write(10,'A'); ���ַΪ10��EEPROMд������A
 *****************************************/
void iap_write(uint address, uchar wdata)
{
	IAP_CONTR = 0x01;				//24M��Ϊ1
	IAP_CONTR |= 0x80;				//ʹ��
	IAP_CMD   = CMD_WRITE;          //ѡ������Ϊ��д���
	IAP_ADDRH = address >> 8;		//д��ַ��λ
	IAP_ADDRL = address;		    //д��ַ��λ

	IAP_DATA  = wdata;
	IAP_TRIG=0x5A;IAP_TRIG=0xA5;
	_nop_();_nop_();				//����������д������
	iap_idle();		//�ر�IAP
}

/*****************************************
 * EEPROMд�볤����
 * ��:iap_write_str(10, "1234567", 7); ���ַΪ10��EEPROMд���ַ���"1234567",д�볤��Ϊ7
 *****************************************/
void iap_write_str(uint address, uchar wdata, uint len)
{
	IAP_CONTR = 0x01;				//24M��Ϊ1
	IAP_CONTR |= 0x80;				//ʹ��
	IAP_CMD   = CMD_WRITE;          //ѡ������Ϊ��д���
	IAP_ADDRH = address >> 8;		//д��ַ��λ
	IAP_ADDRL = address;		    //д��ַ��λ

	IAP_DATA  = wdata;
	IAP_TRIG=0x5A;IAP_TRIG=0xA5;
	_nop_();_nop_();				//����������д������
	iap_idle();		//�ر�IAP
}

/*****************************************
 * EEPROM��������
 * ��:dat = iap_read(10); ����ַΪ10��EEPROM����dat
 *****************************************/
uchar iap_read(uint address)
{
	uchar rdata;

	IAP_CONTR = 0x01;				//24M��Ϊ1
	IAP_CONTR |= 0x80;				//ʹ��
	IAP_CMD   = CMD_READ;           //ѡ������Ϊ��д���
	IAP_ADDRH = address >> 8;		//д��ַ��λ
	IAP_ADDRL = address;		    //д��ַ��λ
	
	IAP_TRIG=0x5A;IAP_TRIG=0xA5;
	_nop_();_nop_();				//����������������������
	rdata     = IAP_DATA;
	iap_idle();		//�ر�IAP

	return rdata;
}

/*****************************************
 * EEPROM��������
 * ��:iap_erase(0); ����EEPROM����
 *****************************************/
void iap_erase(uint address)
{
	IAP_CONTR = 0x01;				//24M��Ϊ1
	IAP_CONTR |= 0x80;				//ʹ��
	IAP_CMD   = CMD_ERASE;          //ѡ������Ϊ��������
	IAP_ADDRH = address / 256;		//д��ַ��λ
	IAP_ADDRL = address % 256;		//д��ַ��λ
	
	IAP_TRIG=0x5A;IAP_TRIG=0xA5;
	_nop_();_nop_();_nop_();_nop_();				//����������������������

	iap_idle();		//�ر�IAP
}