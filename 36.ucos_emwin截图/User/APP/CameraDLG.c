/**
  ******************************************************************************
  * @file    CameraDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ����ͷ��Ӧ�ô���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */


// USER START (Optionally insert additional includes)
#include "includes.h"
#include "DIALOG.h"
#include "./camera/bsp_ov2640.h"
#include "./camera/bsp_ov5640.h"
#include "./camera/ov5640_AF.h"
// USER END

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/


// USER START (Optionally insert additional defines)
/*--------------------  (ʹ��U2C.exeС��������)  ------------------------------------*/
#define UTF8_INIT_INFO				"\xe6\xad\xa3\xe5\x9c\xa8\xe5\x88\x9d\xe5\xa7\x8b\xe5\x8c\x96\xe6\x91\x84\xe5\x83\x8f\xe5\xa4\xb4\xef\xbc\x8c\xe8\xaf\xb7\xe7\xa8\x8d\xe5\x80\x99..."//���ڳ�ʼ������ͷ�����Ժ�...
#define UTF8_RETURN_INFO			"\xe7\xa8\x8b\xe5\xba\x8f\xe8\xbf\x90\xe8\xa1\x8c\xe5\x90\x8e\xe5\x8f\xaf\xe7\x82\xb9\xe5\x87\xbb \xe7\x94\xb5\xe5\xae\xb9\xe6\x8c\x89\xe9\x94\xae \xe8\xbf\x94\xe5\x9b\x9e"//�������к�ɵ�� ���ݰ��� ����
#define UTF8_CAMERERROR      "\xe8\xaf\xb7\xe6\x8f\x92\xe5\x85\xa5\xe6\x91\x84\xe5\x83\x8f\xe5\xa4\xb4"//���������ͷ
/*-----------------------------------------------------------------------------------*/
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
CAMERA_SENSOR camera_sensor = OV2640;  
// USER END


/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
void cbCameraWin(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId) 
	{
	  case WM_DELETE:
			APP_TRACE_DBG(("CameraDLG delete\n"));
		  //�ָ�ɨ�跽��
		  ILI9806G_GramScan(6);
			UserApp_Running = 0;
      tpad_flag=0;
			break;
		case WM_PAINT:
			GUI_Clear();	/* ��������ʾȫ�� */
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&FONT_XINSONGTI_25);

			//��ʾ��Ϣ
			GUI_DispStringHCenterAt(UTF8_INIT_INFO,480/2,854/2-40);   
			GUI_DispStringHCenterAt(UTF8_RETURN_INFO,480/2,854/2);      
		
		
			break;
	}
}

/**
  * @brief  Һ����������ʹ������ͷ���ݲɼ�
  * @param  ��
  * @retval ��
  */
void ImagDisp(uint8_t sx,uint8_t sy,uint16_t width,uint16_t height)
{
	  LCD_SetColors(RED,BLACK);

		//ɨ��ģʽ������
    ILI9806G_GramScan(5);
	
    ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */

	/*DMA������ݴ��䵽Һ���������������ݰ��������� */
    ILI9806G_OpenWindow(sx,sy,width,height);	
	
}

extern uint16_t img_width ,img_height;
/*********************************************************************
*
*       CreateCamera
*/

void FUN_ICON7Clicked(void)
{
  OS_ERR  err;
  WM_HWIN hWin;
  OV2640_IDTypeDef OV2640_Camera_ID;
	
	//OV2640Ҫ��Ϊ4�ı���
	img_width = 852;
	img_height = 480;
  
	GUI_Clear();	/* ��������ʾȫ�� */
  APP_TRACE_DBG(("CameraDLG create\n"));
  
	//�������ڣ��ô��ڲ����մ���
  hWin = WM_CreateWindowAsChild(0,0,480,854,WM_HBKWIN,WM_CF_SHOW,cbCameraWin,0);
  
	GUI_Delay(10);
	
	//��ʼ��I2C
	OV2640_HW_Init();  

  /* Read the OV9655/OV2640 Manufacturer identifier */
  OV2640_ReadID(&OV2640_Camera_ID);
  if(OV2640_Camera_ID.PIDH  == 0x26)
  {
		
		APP_TRACE_DBG(("\r\n��⵽����ͷ %x %x\r\n",OV2640_Camera_ID.Manufacturer_ID1 ,OV2640_Camera_ID.Manufacturer_ID2));
		/* Initializes the DCMI interface  */

    camera_sensor = OV2640;
    bsp_result &=~BSP_CAMERA;
  }
  else
  {
    OV5640_IDTypeDef OV5640_Camera_ID;

		APP_TRACE_DBG(("\r\n��ʼ��OV5640\r\n"));

    OV5640_HW_Init();   

    /* ��ȡ����ͷоƬID��ȷ������ͷ�������� */
    OV5640_ReadID(&OV5640_Camera_ID);

    if(OV5640_Camera_ID.PIDH  == 0x56)
    {
				APP_TRACE_DBG(("\r\n��⵽����ͷ %x %x\r\n",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL));
				
				img_width = 854;
				img_height = 480;

        camera_sensor = OV5640;
        bsp_result &=~BSP_CAMERA;    
    }    
    else
    {
			APP_TRACE_DBG(("\r\nû�м�⵽OV5640����ͷ�������¼�����ӡ�\r\n"));
      bsp_result |= BSP_CAMERA;			
  
			GUI_Delay(100);    
			ErrorDialog(hWin,"Camera Error","camera drvice cannot work!");
			while(1)
			{
				if(tpad_flag)WM_DeleteWindow(hWin);
				 if(!UserApp_Running)return;
				 GUI_Delay(10);
			}

    }  
  
  }

  if(camera_sensor == OV2640)
  {
		//��ֹ����
		OSSchedLock(&err);

		/*ʹ�üĴ������ʼ������ͷ*/
		OV2640_Init();
		OV2640_UXGAConfig(); 
		
		APP_TRACE_DBG(("\r\n��ʼ�����\r\n"));

	
		/*DMAֱ�Ӵ�������ͷ���ݵ�LCD��Ļ��ʾ*/
		ImagDisp(0,0,img_width,img_height);
    //ʹ�ܲɼ�
    OV2640_Capture_Control(ENABLE);

    while(UserApp_Running)
    {  
      if(TPAD_Scan(0))//GetPinStateOfKey2()==1)
//			if(tpad_flag)//GetPinStateOfKey2()==1)
      {		
				OSSchedUnlock(&err);
				//�ر�����ͷ
				OV2640_Stop();
				ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */

        BEEP_ON;              
        WM_DeleteWindow(hWin);      
      }
    }
  }
  else if(camera_sensor == OV5640)
  {
			//��ֹ����
      OSSchedLock(&err);
			
			OV5640_Init();
			
			OV5640_RGB565_Default_Config();
			
			OV5640_USER_Config();

			OV5640_FOCUS_AD5820_Init();
			
			OV5640_FOCUS_AD5820_Constant_Focus();

      ImagDisp(0,0,img_width,img_height);

      //ʹ��DCMI�ɼ�����
			OV5640_Capture_Control(ENABLE);

      while(UserApp_Running)
      {    
//        GUI_Delay(10);    
				
        if(TPAD_Scan(0))//GetPinStateOfKey2()==1)
//       if(tpad_flag)//GetPinStateOfKey2()==1)
       {
					//�ر�����ͷ
					OV5640_Stop();
          OSSchedUnlock(&err);
          BEEP_ON;              
          WM_DeleteWindow(hWin);      
        }
      }
  
  }
}

// USER START (Optionally insert additional public code)
// USER END
/*************************** End of file ****************************/
