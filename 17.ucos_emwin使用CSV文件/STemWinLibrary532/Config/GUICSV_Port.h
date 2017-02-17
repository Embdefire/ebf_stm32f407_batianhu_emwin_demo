#ifndef __GUICSV_PORT_H
#define	__GUICSV_PORT_H

#include "GUI.h"
#include "stm32f4xx.h"

//����XBF����洢��λ�ã�
//FLASH���ļ�ϵͳ�����Ƽ��� 	USE_FLASH_FONT             	0 
//SD���ļ�ϵͳ����							USE_SDCARD_FONT							1
//FLASH�ļ�ϵͳ����							USE_FLASH_FILESYSTEM_FONT		2

#define CSV_SOURCE				1

//���ٶ���죩�ֿ���FLASH�ķ��ļ�ϵͳ����ʹ��ǰ��Ҫ��FLASH�ض���ַ���������ļ�
//���ٶ��еȣ��ֿ�洢��SD���ļ�ϵͳ���򣬵��ԱȽϷ��㣬ֱ��ʹ�ö������ӵ��Կ��������ļ�����
//���ٶ��������ֿ�洢��FLASH�ļ�ϵͳ����ʹ��ǰ��Ҫ��FLASH�ļ�ϵͳ�洢�����ļ�������Ϊ��ʾ�����Ƽ�ʹ��

#define USE_FLASH_CSV								0	
#define USE_SDCARD_CSV							1
#define USE_FLASH_FILESYSTEM_CSV		2


/*֧�ֵ��ֿ�����*/
extern GUI_FONT     CSV_XINSONGTI_25;
extern GUI_FONT     CSV_XINSONGTI_19;


void Create_LANG_CSV(void) ;

#endif /* __GUICSV_PORT_H */
