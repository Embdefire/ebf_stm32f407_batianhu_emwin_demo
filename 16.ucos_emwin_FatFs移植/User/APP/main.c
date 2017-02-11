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
static  OS_TCB   AppTaskFatfsTestTCB;


/*
*********************************************************************************************************
*                                              ջ�ռ�STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  CPU_STK  AppTaskFatfsTestStk[APP_TASK_FATFS_TEST_STK_SIZE];


/*
*********************************************************************************************************
*                                             ��������
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);
static  void  AppTaskCreate(void);
extern  void  Fatfs_MainTask(void);


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
  * @brief  ����Ӧ������ĺ��������ᱻ������������Դ���Ӧ��
  * @param  ��
  * @retval ��
  */
static  void  AppTaskCreate (void)
{
	OS_ERR  os_err;

		//����Ӧ������,����Fatfs
	OSTaskCreate((OS_TCB     *)&AppTaskFatfsTestTCB,             // ����TCB                               
							 (CPU_CHAR   *)"GUI FatFs Test", 									// ��������                             
							 (OS_TASK_PTR ) Fatfs_MainTask,									  // ������ָ��                                
							 (void       *) 0,																	// ��ѡ��������
							 (OS_PRIO     ) APP_TASK_FATFS_TEST_PRIO,					// ���ȼ�
							 (CPU_STK    *)&AppTaskFatfsTestStk[0],							// ����ջ����ַ
							 (CPU_STK_SIZE) APP_TASK_FATFS_TEST_STK_SIZE / 10,				// ջ��ˮӡ������
							 (CPU_STK_SIZE) APP_TASK_FATFS_TEST_STK_SIZE,        		//ջ��С
							 (OS_MSG_QTY  ) 0u,
							 (OS_TICK     ) 0u,
							 (void       *) 0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),//��ѡ����
							 (OS_ERR     *)&os_err);															//�������


}

/*********************************************END OF FILE**********************/

