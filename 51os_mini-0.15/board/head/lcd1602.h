#ifndef __lcd1602_H__
#define __lcd1602_H__

#include <stc15f2k60s2.h>
#include <intrins.h>

#define RS P20
#define RW P21
#define EN P12

/* д��ָ�� */
#define CLEAR_SCR 	0x01	//����
#define RESET_CUR 	0x02	//��긴λ

#define CURR_SCRN	0x14	//д�����ݵ�ʱ�������ƣ���Ļ���ƶ�
#define CURL_SCRN	0x10	//д�����ݵ�ʱ�������ƣ���Ļ���ƶ�
#define CURN_SCRR	0x1C	//д�����ݵ�ʱ���겻�ƶ�����Ļ����
#define CURN_SCRL	0x18	//д�����ݵ�ʱ���겻�ƶ�����Ļ����
/* 0 0 0 0 1 D C B
 * D	1 ��ʾ��		0 ��ʾ��
 * C	1 �й��		0 �޹��
 * B	1 �����˸		0 ��겻��˸
 */
#define DIS_CUR_FIC	0x0F	//��ʾ������꿪�������˸
/* 0 0 0 1 S/C R/L x x
 * S/C	1 ȫ���ַ�	0 ֻ���
 * R/L	1 ����		0 ����
 */

/* 0 0 1 DL N F x x
 * DL	0 ����4λ	1 ����8λ
 * N 	0 ��ʾһ�� 	1 ��ʾ����
 * F	0 5*7����	1 5*10����
 */
#define	NORMAL_DIS	0x38	//��λ���ݣ���ʾ���У�5*7����

/* 0 1 CGRAM_ADDRESS(6bits)
 * �趨CGRAM�ĵ�ַ
 * 543 �ַ���
 * 210 �к�
 */

/* 0 1 DDRAM_ADDRESS(6bits)
 * �趨DDRAM�ĵ�ַ
 */


/* ��ȡָ�� */
/* BF AC(7bits)
 * SF = 1ʱΪæ
 */


extern void lcd1602_init(void);
extern void lcd1602_clr(void);
extern void lcd1602_dis(unsigned char addr, unsigned char dat);
extern void lcd1602_dis_str(unsigned char addr, unsigned char *str);

#endif