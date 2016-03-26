#ifndef	__ds1302_H__
#define __ds1302_H__

#include <stc15f2k60s2.h>
#include <intrins.h>

/* �궨������ŵ����� */
#define IO    P23
#define RESET P13
#define SCLK  P17 

/* ������ʱ����ĵ�ַ */
#define YEAR  0x8C
#define DAY   0x8A
#define MONTH 0x88
#define DATE  0x86
#define HOUR  0x84
#define MIN   0x82
#define SEC   0x80

#define WRITE_MASK 0x00
#define READ_MASK  0x01

extern void get_time(unsigned char *time);
//extern void set_time(unsigned char *time);
extern void ds1302_init(void);

#endif