/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emwin�������Գ���
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

#include "GUIDEMO.h"


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{
	 //��ʼ���ⲿSRAM  
  FSMC_SRAM_Init();

  LED_GPIO_Config();
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

	printf("\r\n ********** emwin�������Գ���*********** \r\n"); 
	
	//emwin�ٷ�demo���޸�guidemo.h�ļ��еĺ��������ʾӦ��
	GUIDEMO_Main();

}





/*********************************************END OF FILE**********************/

