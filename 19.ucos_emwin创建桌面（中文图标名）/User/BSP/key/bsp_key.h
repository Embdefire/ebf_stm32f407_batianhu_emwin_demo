#ifndef __KEY_H
#define	__KEY_H

#include "stm32f4xx.h"
#include "os.h"


#define EVEN_KEY1_DOWN 0x01
#define EVEN_KEY2_DOWN 0x02
extern OS_FLAG_GRP key_flag_grp;                   //�����¼���־��


//���Ŷ���
/*******************************************************/
#define KEY1_PIN                  GPIO_Pin_0                 
#define KEY1_GPIO_PORT            GPIOA                      
#define KEY1_GPIO_CLK             RCC_AHB1Periph_GPIOA

#define KEY2_PIN                  GPIO_Pin_13                 
#define KEY2_GPIO_PORT            GPIOC                      
#define KEY2_GPIO_CLK             RCC_AHB1Periph_GPIOC
/*******************************************************/

 /** �������±��ú�
	* ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	* ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#define KEY_ON	1
#define KEY_OFF	0

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);

void  AppTaskKeyScan  (void );

#endif /* __LED_H */

