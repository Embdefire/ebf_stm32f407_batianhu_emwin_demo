/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emwin + ucosIII ��ֲFatfs
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */


/*
*********************************************************************************************************
*                                             �����ļ�
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                               �궨��
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             ������ƿ�TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;
static  OS_TCB   AppTaskKeyScanTCB;
static  OS_TCB   AppTaskPictureTCB;


/*
*********************************************************************************************************
*                                              ջ�ռ�STACKS
*********************************************************************************************************
*/
static  CPU_STK  AppTaskKeyScanStk[APP_TASK_KEY_SCAN_STK_SIZE];
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  CPU_STK  AppTaskPictureStk[APP_TASK_PICTURE_STK_SIZE];


/*
*********************************************************************************************************
*                                             ��������
*********************************************************************************************************
*/
static  void  AppTaskStart  (void *p_arg);
static  void  BSPTaskCreate (void);
static  void  AppTaskCreate(void);


extern  void  Picture_MainTask(void);


/*
*********************************************************************************************************
*                                            
*********************************************************************************************************
*/


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int  main (void)
{
    OS_ERR  os_err;


    OSInit(&os_err);                                               		/* ��ʼ�� uC/OS-III.                                      */

		//������������
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                // ����TCB                               
                 (CPU_CHAR   *)"App Task Start", 								// ��������                             
                 (OS_TASK_PTR ) AppTaskStart,									  // ������ָ��                                
                 (void       *) 0,																	// ��ѡ��������
                 (OS_PRIO     ) APP_TASK_START_PRIO,							// ���ȼ�
                 (CPU_STK    *)&AppTaskStartStk[0],							// ����ջ����ַ
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,		// ջ��ˮӡ������
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,        //ջ��С
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),//��ѡ����
                 (OS_ERR     *)&os_err);															//�������

		//��ʼִ�����񣬴Ӵ˴���ʼ��ucosϵͳ����
    OSStart(&os_err);                                              
		
		
}

extern void GTP_TouchProcess(void);

/**
  * @brief  ��������������Ҫ���ucos��BSP��GUI�Լ���������ĳ�ʼ��
  * @param  p_arg: OSTaskCreate����ʱ���������ָ��
  * @retval ��
  */
static  void  AppTaskStart (void *p_arg)
{
    OS_ERR      os_err;

   (void)p_arg;
	
    CPU_Init();	//��ʼ��CPU
    BSP_Init();  //��ʼ��BSP  	
	
    Mem_Init(); 	//��ʼ���洢������                                     

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&os_err); //����������ʱCPUʹ����                            
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif


	APP_TRACE_DBG(("���ڴ���Ӧ������...\n\r"));

	//������������
	BSPTaskCreate();
	//����Ӧ������
	AppTaskCreate();  

 //������ѭ��
	while (DEF_TRUE) 
	{                                        
			LED2_TOGGLE ;
			APP_TRACE_DBG(("cpuʹ����:%.2f%%\n",((float)OSStatTaskCPUUsage/100)));
			
		  //��ʱ����������������ѭ���ڶ�Ӧ������1ms��ʱ
		  //�ر��Ǹ����ȼ�������������ʱ�����������ȼ�������ܻ��޻���ִ��
			OSTimeDly(1000u, 
									OS_OPT_TIME_DLY,
									&os_err);
	}	
		
}


/**
  * @brief  �������������簴������ʱɨ���
  * @param  ��
  * @retval ��
  */
static  void  BSPTaskCreate (void)
{
	OS_ERR  os_err;

		//��������ɨ������
	OSTaskCreate((OS_TCB     *)&AppTaskKeyScanTCB,             // ����TCB                               
							 (CPU_CHAR   *)"Key Scan", 									// ��������                             
							 (OS_TASK_PTR ) AppTaskKeyScan,									  // ������ָ��                                
							 (void       *) 0,																	// ��ѡ��������
							 (OS_PRIO     ) APP_TASK_KEY_SCAN_PRIO,					// ���ȼ�
							 (CPU_STK    *)&AppTaskKeyScanStk[0],							// ����ջ����ַ
							 (CPU_STK_SIZE) APP_TASK_KEY_SCAN_STK_SIZE / 10,				// ջ��ˮӡ������
							 (CPU_STK_SIZE) APP_TASK_KEY_SCAN_STK_SIZE,        		//ջ��С
							 (OS_MSG_QTY  ) 0u,
							 (OS_TICK     ) 0u,
							 (void       *) 0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),//��ѡ����
							 (OS_ERR     *)&os_err);															//�������


}

/**
  * @brief  ����Ӧ������ĺ��������ᱻ������������Դ���Ӧ��
  * @param  ��
  * @retval ��
  */
static  void  AppTaskCreate (void)
{
	OS_ERR  os_err;

		//����Ӧ������,������ʾͼƬ
	OSTaskCreate((OS_TCB     *)&AppTaskPictureTCB,             // ����TCB                               
							 (CPU_CHAR   *)"GUI Picture Test", 									// ��������                             
							 (OS_TASK_PTR ) Picture_MainTask,									  // ������ָ��                                
							 (void       *) 0,																	// ��ѡ��������
							 (OS_PRIO     ) APP_TASK_PICTURE_PRIO,					// ���ȼ�
							 (CPU_STK    *)&AppTaskPictureStk[0],							// ����ջ����ַ
							 (CPU_STK_SIZE) APP_TASK_PICTURE_STK_SIZE / 10,				// ջ��ˮӡ������
							 (CPU_STK_SIZE) APP_TASK_PICTURE_STK_SIZE,        		//ջ��С
							 (OS_MSG_QTY  ) 0u,
							 (OS_TICK     ) 0u,
							 (void       *) 0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),//��ѡ����
							 (OS_ERR     *)&os_err);															//�������


}


/*********************************************END OF FILE**********************/

