/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ����Ӧ��bsp��ɨ��ģʽ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./key/bsp_key.h" 
#include "os.h"

OS_FLAG_GRP 	key_flag_grp;                   //�����¼���־��


/// ����ȷ����ʱ
void Key_Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 

/**
  * @brief  ���ð����õ���I/O��
  * @param  ��
  * @retval ��
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*��������GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK,ENABLE);
	
  /*ѡ�񰴼�������*/
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN; 
  
  /*��������Ϊ����ģʽ*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  
  /*�������Ų�����Ҳ������*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
  /*ʹ������Ľṹ���ʼ������*/
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);   
  
  /*ѡ�񰴼�������*/
	GPIO_InitStructure.GPIO_Pin = KEY2_PIN; 
  
  /*ʹ������Ľṹ���ʼ������*/
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);  
}

/**
  * @brief   ����Ƿ��а�������     
  *	@param 	GPIOx:����Ķ˿�, x�����ǣ�A...K�� 
	*	@param 	GPIO_PIN:����Ķ˿�λ�� ������GPIO_PIN_x��x������0...15��
  * @retval  ������״̬
  *		@arg KEY_ON:��������
  *		@arg KEY_OFF:����û����
  */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
		OS_ERR  os_err;

	/*����Ƿ��а������� */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*�ȴ������ͷ� */
		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON)
		{
					OSTimeDly(2u, 
									OS_OPT_TIME_DLY,
									&os_err);

		}			
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}



/**
  * @brief  ����������
  * @param  ��
  * @retval ��
  */
void  BSPTaskKeyScan  (void )
{
	OS_ERR  os_err;
	
	  /* �����¼���־�� flag_grp */
  OSFlagCreate ((OS_FLAG_GRP  *)&key_flag_grp,        //ָ���¼���־���ָ��
                (CPU_CHAR     *)"KEY Flag",  //�¼���־�������
                (OS_FLAGS      )0,                //�¼���־��ĳ�ʼֵ
                (OS_ERR       *)&os_err);					  //���ش�������


	while(1)
	{
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
			{
						OSFlagPost ((OS_FLAG_GRP  *)&key_flag_grp,                             //����־���BIT0��0
													(OS_FLAGS      )EVEN_KEY1_DOWN,
													(OS_OPT        )OS_OPT_POST_FLAG_SET,
													(OS_ERR       *)&os_err);
			}
			else
			{
			
			}
			
			if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
			{
						OSFlagPost ((OS_FLAG_GRP  *)&key_flag_grp,                             //����־���BIT0��0
													(OS_FLAGS      )EVEN_KEY2_DOWN,
													(OS_OPT        )OS_OPT_POST_FLAG_SET,
													(OS_ERR       *)&os_err);
			}
			else
			{
			
			}
			
			
			//��ʱ����������������ѭ���ڶ�Ӧ������1ms��ʱ
		  //�ر��Ǹ����ȼ�������������ʱ�����������ȼ�������ܻ��޻���ִ��
			OSTimeDly(15u, 
									OS_OPT_TIME_DLY,
									&os_err);

			
	}
}
/*********************************************END OF FILE**********************/
