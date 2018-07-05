/**
  ******************************************************************************
  * @file    GUIBaseTest.c
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
  
#include <includes.h>


/**
  * @brief  GUIBase_Test����������emwin������ʾ���ԣ�
  * @param  ��  
  * @retval ��
  */
void GUIBase_Test ( void )
{	
	int i=0;
	char str_temp[50];
	OS_ERR      os_err;


	//�����Ļ
	GUI_Clear();

	printf("\r\n **********  emwin������ʾ����*********** \r\n"); 
	printf("\r\n ������֧������ \r\n"); 

	GUI_SetColor(GUI_WHITE);   
	/* ��ʾ���� */
	GUI_DispStringAt("wildfire F407 emWin test!",10,10);
	GUI_Delay(100);
	
	while (1)
	{
			//��ʾ����
			GUI_DispDecAt(i++,50,50,4);
			if(i>9999)
				i=0; 
			
			//��ʾCPUʹ����
			sprintf(str_temp,"cpu usage:%.2f%%",((float)OSStatTaskCPUUsage/100));
			GUI_DispStringAtCEOL(str_temp,10,100);

		
			//��ʱ����������������ѭ���ڶ�Ӧ������1ms��ʱ
		  //�ر��Ǹ����ȼ�������������ʱ�����������ȼ�������ܻ��޻���ִ��
			
			//ʹ��GUI_Delay�ǵ�Ч��
			
			//�����޸���ʱֵ���鿴CPUʹ���ʵĲ���
			OSTimeDly(1000u, 
									OS_OPT_TIME_DLY,
									&os_err);


	}
	
}





/*********************************************END OF FILE**********************/

