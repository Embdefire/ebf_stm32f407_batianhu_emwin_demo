#ifndef __GUI_PICTURE_PORT_H
#define	__GUI_PICTURE_PORT_H

#include "stm32f4xx.h"
#include "GUI.h"

//ѡ��ʹ��emwin���ڴ�ռ䣨��̬���䣩���Ǿ�̬�ռ䣨�����ȫ�ֱ�����
#define USE_MEM 			EMWIN_MEM

#define EMWIN_MEM 	 0	//emwin���ڴ�ռ�
#define STATIC_MEM	 1	//��̬�Ŀռ�

void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ;
void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ; 
void _ShowGIFEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime);  
void _ShowGIF(uint8_t * imgBuffer, uint32_t BufferSize,uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime)  ;
void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY); 

#endif /* __GUI_PICTURE_PORT_H */
