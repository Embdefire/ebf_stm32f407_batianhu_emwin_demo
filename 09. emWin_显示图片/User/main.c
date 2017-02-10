/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emWin_lcd_V532_FatFs��ֲ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "GUI.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./lcd/bsp_ili9806g_lcd.h"
#include "./systick/bsp_SysTick.h"
#include "./touch/gt5xx.h"
#include "./sram/bsp_sram.h"	
#include "./key/bsp_key.h" 

#include "ff.h"

#include "GUIFont_Port.h"   

FATFS fs;													/* Work area (file system object) for logical drives */


extern void drawbmp(void);
extern void drawJPEG(void);
extern void drawpng(void);
extern void drawgif(void);

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{
	static uint8_t show_pic_flag = 2;
	
	GUI_RECT info_text ={0,100,480,200};

	 //��ʼ���ⲿSRAM  
  FSMC_SRAM_Init();

  LED_GPIO_Config();
	Key_GPIO_Config();
	
	LED_GREEN;	
	
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
	
	/* ��ʼ��GUI */
	GUI_Init();	

	//��ʼ������
	Debug_USART_Config();	

	//������ʼ����ʹ�ô������������Ϣ��Ϊȷ���������ȳ�ʼ�������ٳ�ʼ������
	GTP_Init_Panel();
	 
	/* ��ʼ����ʱ�� */
	SysTick_Init();	

	/* ���ڵ�����Ϣ */
	printf("emWin FatFs demo\r\n");
	
	//����XBF���壬ʹ��UTF8����
	Creat_XBF_Font();
	GUI_UC_SetEncodeUTF8();
	GUI_SetDefaultFont(&FONT_XINSONGTI_25);
	
	GUI_SetBkColor(GUI_WHITE);
  GUI_Clear(); 
  GUI_SetFont(GUI_FONT_32B_ASCII);
  GUI_SetColor(GUI_BLUE);
	
	info_text.x0 = 10;
	info_text.x1 = 480-10;
	info_text.y0 = 100;
	info_text.y1 = 200;

	GUI_DispStringInRectWrap("Press 'KEY1' to show different format picture",
															&info_text,
															GUI_TA_CENTER,
															GUI_WRAPMODE_WORD) ;
	
	//�����ļ�ϵͳ�������ȹ����ļ�ϵͳ����ʾ�ļ�ϵͳ�е�ͼƬ	
	f_mount(&fs,"1:",1);	
	
	while(1)
	{	
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			show_pic_flag++;
			
			switch(show_pic_flag%4)
			{
				case 0:drawpng();			break;
				case 1:drawbmp();			break;
				case 2:drawJPEG();   	break; 
				case 3:drawgif();   	break; 				

			}
			LED2_TOGGLE;
		}   
    
	  GUI_Delay(10);
	}
}





/*********************************************END OF FILE**********************/

