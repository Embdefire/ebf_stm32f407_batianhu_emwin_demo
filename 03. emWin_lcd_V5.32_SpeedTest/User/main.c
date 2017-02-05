/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emwin�ٶȲ��Գ���
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

extern void SpeedTest(void);

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{
  LED_GPIO_Config();
	LED_GREEN;
	
  /* ��ʼ����ʱ�� */
	SysTick_Init();
	
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
	
	/* ��ʼ��GUI */
	GUI_Init();

	Debug_USART_Config();		
	
	printf("\r\n ********** emwin�ٶȲ��Գ���*********** \r\n"); 
	
 GUI_SetColor(GUI_WHITE);   
	/* ��ʾ���� */
	GUI_DispStringAt(" emWin Speed test Demo",10,10);
	GUI_Delay(2000);
   

	//��ֲ������ʱ��ע��Ҫ�������ļ��е�ջ�ռ����ô�һ��

	while(1)
	{
		//ִ���ٶȲ���Demo
		SpeedTest();
		GUI_Delay(3000);
	}
}





/*********************************************END OF FILE**********************/

