/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emwin������ʾ����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������ 
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



/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{
	int i=0;

  LED_GPIO_Config();
	LED_GREEN;
	
  /* ��ʼ����ʱ�� */
	SysTick_Init();
	
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
	
	/* ��ʼ��GUI */
	GUI_Init();

	Debug_USART_Config();		
	
	printf("\r\n **********  emwin������ʾ����*********** \r\n"); 
	printf("\r\n ������֧������ \r\n"); 

	 GUI_SetColor(GUI_WHITE);   
	/* ��ʾ���� */
	GUI_DispStringAt("wildfire F407 emWin test!",10,10);
	GUI_Delay(100);
	
	while (1)
	{
			GUI_DispDecAt(i++,50,50,4);
			if(i>9999)
				i=0; 
	}
	
}





/*********************************************END OF FILE**********************/

