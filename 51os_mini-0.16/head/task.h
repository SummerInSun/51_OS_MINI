#ifndef	__task_H__
#define __task_H__

#define TASK_MSGHANDLER 2	//�궨��task_msghandler�����Ϊ2�����ڽ������Ϣ��������

#include <stc15f2k60s2.h>
#include "uart.h"
#include "common.h"
#include "nt_led.h"
#include "vector.h"
#include "eeprom.h"
#include "core.h"
#include "char.h"
#include "structs.h"
#include "fs.h"
//#include "at24c02.h"
//#include "lcd1602.h"
//#include "ds1302.h"
//#include "18b20.h"
//#include "hc_sr04.h"

#define MSG_MAX	3	//����������������������Ķ��ٶ��ı�

#define CREATE_TASK_RUNNING(priority, task, name) \
			task_create(priority, (unsigned int)(&task), 0, name);

extern void idle(void);
extern void task1(void);
extern void task2(void);
extern void task_msghandler(void);
extern void task_tty0(void);
extern void msghandler_list_init(void);

#endif