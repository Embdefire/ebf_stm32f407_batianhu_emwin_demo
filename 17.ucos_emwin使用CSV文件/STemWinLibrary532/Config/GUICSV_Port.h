#ifndef __GUICSV_PORT_H
#define	__GUICSV_PORT_H

#include "GUI.h"
#include "stm32f4xx.h"

//设置XBF字体存储的位置：
//FLASH非文件系统区域（推荐） 	USE_FLASH_FONT             	0 
//SD卡文件系统区域							USE_SDCARD_FONT							1
//FLASH文件系统区域							USE_FLASH_FILESYSTEM_FONT		2

#define CSV_SOURCE				1

//（速度最快）字库在FLASH的非文件系统区域，使用前需要往FLASH特定地址拷贝字体文件
//（速度中等）字库存储在SD卡文件系统区域，调试比较方便，直接使用读卡器从电脑拷贝字体文件即可
//（速度最慢）字库存储在FLASH文件系统区域，使用前需要往FLASH文件系统存储字体文件，仅作为演示，不推荐使用

#define USE_FLASH_CSV								0	
#define USE_SDCARD_CSV							1
#define USE_FLASH_FILESYSTEM_CSV		2


/*支持的字库类型*/
extern GUI_FONT     CSV_XINSONGTI_25;
extern GUI_FONT     CSV_XINSONGTI_19;


void Create_LANG_CSV(void) ;

#endif /* __GUICSV_PORT_H */
