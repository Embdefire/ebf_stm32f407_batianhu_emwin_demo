/**
  ******************************************************************************
  * @file    desktop.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����ۺ����̵��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "includes.h"
#include "desktop.h"



static void _cbMidWin(WM_MESSAGE * pMsg) ;
static void CreateMidWin(void);
static void CreateTopWin(void);

RTC_TIME rtc_time;

WIN_PARA WinPara;			//�û�ʹ�õĴ��ڶ�������

/* ��������ICONVIEW0ͼ��Ĵ��� */
static const BITMAP_ITEM _aBitmapItem[] = 
{
#if (ICON_NAME_FORMAT == ICON_NAME_EN)
	//��Ӣ��ͼ���ַ�
  {&bmIdea,              "RGBLED"},
  {&bmMusic,    	        "Music"},
  {&bmSiri,            "Recorder"},
	{&bmWeather,     	    "Weather"}, 
  
  {&bmCompass,          "Compass"},  
	{&bmClock,              "Clock"},
	{&bmSafari, 		       "Network"}, 
  {&bmCamera,    	       "Camera"}, 
  
	{&bmPhone,  	 	        "Phone"},    
	{&bmMessages,  	 	   "Messages"},
  {&bmPhotos,   	      "Picture"},		
	{&bmUSB,  	              "USB"},   	
	{&bmCalculator,    "Calculator"}, 
	
#elif (ICON_NAME_FORMAT == ICON_NAME_CH_GBK)
	//����GBK�����ַ�
	{&bmIdea,              "RGBLED"},
  {&bmMusic,    	        "����"},
  {&bmSiri,            "¼��"},
	{&bmWeather,     	    "����"}, 
  
  {&bmCompass,          "����"},  
	{&bmClock,              "ʱ��"},
	{&bmSafari, 		       "����"}, 
  {&bmCamera,    	       "����ͷ"}, 
  
	{&bmPhone,  	 	        "�绰"},    
	{&bmMessages,  	 	   "����"},
  {&bmPhotos,   	      "ͼƬ"},		
	{&bmUSB,  	              "USB"},   	
	{&bmCalculator,    "������"}, 
	
#elif (ICON_NAME_FORMAT == ICON_NAME_CH_UTF8)
	//����UTF8���룬����ʹ�ù���Ŀ¼�µġ�U2C.exe������ת��
	{&bmIdea,              "RGBLED"},
  {&bmMusic,    	        "\xe9\x9f\xb3\xe4\xb9\x90"},
  {&bmSiri,            "\xe5\xbd\x95\xe9\x9f\xb3"},
	{&bmWeather,     	    "\xe5\xa4\xa9\xe6\xb0\x94"}, 
  
  {&bmCompass,          "\xe7\xbd\x97\xe7\x9b\x98"},  
	{&bmClock,              "\xe6\x97\xb6\xe9\x92\x9f"},
	{&bmSafari, 		       "\xe7\xbd\x91\xe7\xbb\x9c"}, 
  {&bmCamera,    	       "\xe6\x91\x84\xe5\x83\x8f\xe5\xa4\xb4"}, 
  
	{&bmPhone,  	 	        "\xe7\x94\xb5\xe8\xaf\x9d"},    
	{&bmMessages,  	 	   "\xe7\x9f\xad\xe4\xbf\xa1"},
  {&bmPhotos,   	      "\xe5\x9b\xbe\xe7\x89\x87"},		
	{&bmUSB,  	              "USB"},   	
	{&bmCalculator,    "\xe8\xae\xa1\xe7\xae\x97\xe5\x99\xa8"},
	
#endif
};


uint8_t UserApp_Running=0;

uint8_t ICON_Clicked[13]   = {0};   /* ICONVIEW�ؼ����µı�־��0��ʾδ���£�1��ʾ���� */


/*���ͼ���ִ�еĺ������˴����������*/
extern void FUN_ICON0Clicked(void);

//void FUN_ICON0Clicked(void)  {printf("FUN_ICON0Clicked\n");}
extern void FUN_ICON1Clicked(void);
extern void FUN_ICON2Clicked(void);
extern void FUN_ICON3Clicked(void);

extern void FUN_ICON4Clicked(void);
extern void FUN_ICON5Clicked(void);
extern void FUN_ICON6Clicked(void);
extern void FUN_ICON7Clicked(void);

extern void FUN_ICON8Clicked(void);
extern void FUN_ICON9Clicked(void);
extern void FUN_ICON10Clicked(void);
extern void FUN_ICON11Clicked(void);

extern void FUN_ICON12Clicked(void);


/**
  * @brief  CreateTopWin�����������Ĵ���
  * @param  none
  * @retval none
  */
static void CreateTopWin(void)
{	
	WM_HWIN hText;
	/* ������ "wildfire"�ı� */
	hText = TEXT_CreateEx(0, 2, 200 , 30, WM_HBKWIN, WM_CF_SHOW|WM_CF_MEMDEV, GUI_TA_LEFT|TEXT_CF_VCENTER, GUI_ID_TEXT0, "");
	TEXT_SetFont(hText,GUI_FONT_24B_ASCII);
	TEXT_SetText(hText,"BH-STM32F407");
	TEXT_SetTextColor(hText,STATUS_TEXT_COLOR);
	
	/* ״̬����ʱ����ʾ�ı� */
	hText = TEXT_CreateEx(WinPara.xSizeLCD/2-50,2,100,30,
													WM_HBKWIN,WM_CF_SHOW|WM_CF_MEMDEV,
													GUI_TA_HCENTER|TEXT_CF_VCENTER,
													GUI_ID_TEXT1,
													"11:56:00");
	
	//TEXT_SetBkColor(hText,GUI_INVALID_COLOR);
	TEXT_SetTextColor(hText,STATUS_TEXT_COLOR);
	TEXT_SetFont(hText,GUI_FONT_24B_ASCII);
  	/* ״̬����ʱ����ʾ�ı� */
	hText = TEXT_CreateEx(WinPara.xSizeLCD-200,2,200,30,
													WM_HBKWIN,WM_CF_SHOW|WM_CF_MEMDEV,
													GUI_TA_RIGHT|TEXT_CF_VCENTER,
													GUI_ID_STATUS_CPU_USAGE,
													"CPU: 10%");
													
	//TEXT_SetBkColor(hText,GUI_INVALID_COLOR);
	TEXT_SetTextColor(hText,STATUS_TEXT_COLOR);
	TEXT_SetFont(hText,GUI_FONT_24B_ASCII);
}



/**
  * @brief  CreateMidWin�������м�Ĵ���
  * @param  none
  * @retval none
  */
static void CreateMidWin(void)
{
	uint8_t i=0;
	WM_HWIN MIDWIN;
	
#if (ICON_NAME_FORMAT == ICON_NAME_CH_GBK)
	//�洢ת����utf8�����ģ�ע�ⳤ�ȣ�������ÿ����4�ֽ���
  char utf8_pText[20];	
#endif
	
	WinPara.hWinMid= WM_CreateWindowAsChild(
											WinPara.xPosWin,											
											WinPara.yPosWin,
											WinPara.xSizeWin,
											WinPara.ySizeWin,	
											WM_HBKWIN, 
											WM_CF_SHOW|WM_CF_MEMDEV,
											_cbMidWin, 
											sizeof(WIN_PARA *)
											);
/*-------------------------------------------------------------------------------------*/
					/*��ָ��λ�ô���ָ���ߴ��ICONVIEW1 С����*/
					MIDWIN=ICONVIEW_CreateEx(
											 ICONVIEW_XPos, 					          /* С���ߵ��������أ��ڸ������У�*/
											 ICONVIEW_YPos, 								     /* С���ߵ��������أ��ڸ������У�*/
											 WinPara.xSizeWin-ICONVIEW_XPos,	  /* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
											 WinPara.ySizeWin - ICONVIEW_YPos , 			      /* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
											 WinPara.hWinMid, 				          /* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
											 WM_CF_SHOW|WM_CF_MEMDEV|WM_CF_HASTRANS,         /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */ 
											 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* Ĭ����0�����������ʾ������������ֱ������ */
											 GUI_ID_ICONVIEW0, 			            /* С���ߵĴ���ID */
											 ICONVIEW_Width, 				            /* ͼ���ˮƽ�ߴ� */
											 ICONVIEW_Height+20);				        /* ͼ��Ĵ�ֱ�ߴ� */
											 
		/* ��ICONVIEW С���������ͼ�� */
		for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++) 
		{	
			#if (ICON_NAME_FORMAT == ICON_NAME_CH_GBK)
				//GBK�ַ�תUTF8
				com_gbk2utf8(_aBitmapItem[i].pText,utf8_pText);
				ICONVIEW_AddBitmapItem(MIDWIN, _aBitmapItem[i].pBitmap,utf8_pText);			
			#else
				//ֱ��ʹ���ַ�
				ICONVIEW_AddBitmapItem(MIDWIN, _aBitmapItem[i].pBitmap,_aBitmapItem[i].pText);
			#endif
			
			ICONVIEW_SetTextColor(MIDWIN,i,GUI_WHITE);
		}	
		
		/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
		ICONVIEW_SetBkColor(MIDWIN, ICONVIEW_CI_SEL, ICON_COLOR | 0x80000000);
		
		/* �������� */
#if (ICON_NAME_FORMAT == ICON_NAME_CH_GBK || ICON_NAME_FORMAT == ICON_NAME_CH_UTF8)
		/* �������� */
		ICONVIEW_SetFont(MIDWIN, &FONT_XINSONGTI_19);
#elif (ICON_NAME_FORMAT == ICON_NAME_EN)
		/* ��Ӣ�����壨ֱ��ʹ����������Ҳ�ǿ��Եģ� */
		ICONVIEW_SetFont(MIDWIN, GUI_FONT_20B_ASCII);
#endif
		
		/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
		ICONVIEW_SetSel(MIDWIN,-1);
		/* ����ͼ����x ��y �����ϵļ�ࡣ*/
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_X, ICONVIEW_XSpace);
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_Y, ICONVIEW_YSpace);
		/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
		ICONVIEW_SetIconAlign(MIDWIN, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
		
/*-------------------------------------------------------------------------------------*/
}

/**
  * @brief  �ص��������м䴰�ڵĻص����������������ػ����Ϣ
	* @param  pMsg:����WM_MESSAGE���ݵ�����
  * @retval none
  */
static void _cbMidWin(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	
	//ָʾ��ǰͼ���Ƿ񱻰���
	static uint8_t selecting = 0;
	
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
			NCode = pMsg->Data.v;                 /* Notification code */
			switch (Id) 
			{
				case GUI_ID_ICONVIEW0:
					switch (NCode) 
					{
						case WM_NOTIFICATION_MOVED_OUT:
							selecting =0;
							WM_InvalidateWindow(pMsg->hWin);
							break;
						
						/* ICON�ؼ������Ϣ */
						case WM_NOTIFICATION_CLICKED:
							selecting =1;
							break;
						
						/* ICON�ؼ��ͷ���Ϣ */
						case WM_NOTIFICATION_RELEASED: 
              UserApp_Running=1;
							
							selecting =0;
							WM_InvalidateWindow(pMsg->hWin);

							
							/* ����Ӧѡ�� */
							switch(ICONVIEW_GetSel(pMsg->hWinSrc))
							{
								/* RGBLED *******************************************************************/
								case 0:	
									//��¼ͼ�걻���µı�־
									ICON_Clicked[0] = 1;

									FUN_ICON0Clicked();			
									break;	
								
								/* Music ***********************************************************************/
								case 1:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[1] = 1;

									FUN_ICON1Clicked();
									break;
								
								/* Recorder *********************************************************************/
								case 2:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[2] = 1;
								
									FUN_ICON2Clicked();	
									break;
								
								/* Weather *******************************************************************/
								case 3:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[3] = 1;
								
									FUN_ICON3Clicked();
									break;
								
								/* Compass  ******************************************************************/
								case 4:	
									//��¼ͼ�걻���µı�־
									ICON_Clicked[4] = 1;
								
									FUN_ICON4Clicked();            
									break;	
								
								/* Clock ***********************************************************************/
								case 5:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[5] = 1;
								
									FUN_ICON5Clicked();
									break;
								
								/* Safari *********************************************************************/
								case 6:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[6] = 1;
								
 									FUN_ICON6Clicked();	
									break;
								
								/* Camera ********************************************************************/
								case 7:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[7] = 1;
								
									FUN_ICON7Clicked();
                  GUI_Delay(50);
									break;
								
								/* Phone **********************************************************************/
								case 8:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[8] = 1;
								
									FUN_ICON8Clicked();
									break;
								
								/* Message ********************************************************************/
								case 9:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[9] = 1;
								
									FUN_ICON9Clicked();
									break;
								
								/* Photos ******************************************************************/
								case 10:					
									//��¼ͼ�걻���µı�־
									ICON_Clicked[10] = 1;
								
									FUN_ICON10Clicked();
									break;
								
								/* USB *******************************************************************/
								case 11:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[11] = 1;
								
									FUN_ICON11Clicked();
									break;		
								/* Calculator *******************************************************************/
								case 12:
									//��¼ͼ�걻���µı�־
									ICON_Clicked[12] = 1;
								
									FUN_ICON12Clicked();
									break;                           
								}
							 break;
						}
					break;
			}
			break;
			
		/* �ػ���Ϣ*/
		case WM_PAINT:
			{
				GUI_SetBkColor(DTCOLOR);
				GUI_Clear();
				
				//��ͼ�겻�Ǳ����£�ѡ��Ϊ-1��ͼ�꣬�����ͼ���ѡ�п�
				if(selecting == 0)
		       ICONVIEW_SetSel(WM_GetDialogItem(WinPara.hWinMid, GUI_ID_ICONVIEW0),-1);

			}			
		break;			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/**
  * @brief  ����bsp��ʼ���ı�־��鲢���������Ϣ
  * @param  ��  
  * @retval ��
  */
void emWinMainApp(void)
{
	
	
	  GUI_Delay(5);

}
	
/*
*********************************************************************************************************
*	�� �� ��: _cbBkWindow
*	����˵��: ���洰�ڵĻص����� 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	char text_buffer[20]={0};
	WM_HWIN hItem;
  uint8_t sec, min, hour;

	switch (pMsg->MsgId) 
	{
		/* �ػ���Ϣ*/
		case WM_PAINT:		
				GUI_SetBkColor(DTCOLOR);
				GUI_Clear();
		break;
		
		case WM_TIMER:
			
		  if(UserApp_Running==1)
      {
        WM_RestartTimer(pMsg->Data.v, 1000);
        break; 
      }      
		
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_STATUS_CPU_USAGE);
				//������Ƿ����
				if(hItem != 0)
				{
					sprintf(text_buffer,"CPU:% 2d%%",(uint8_t)(OSStatTaskCPUUsage/100));
					TEXT_SetText(hItem,text_buffer);
					WM_RestartTimer(pMsg->Data.v, 500);
				}
				
				if(!(bsp_result&BSP_RTC))
				{
					//printf("app timer\n");
					RTC_GetTime(RTC_Format_BIN, &rtc_time.RTC_Time);
					sec    =  rtc_time.RTC_Time.RTC_Seconds;
					min    =  rtc_time.RTC_Time.RTC_Minutes;
					hour   =  rtc_time.RTC_Time.RTC_Hours;
					sprintf(text_buffer,"%02d:%02d:%02d",hour,min,sec);
					TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1),text_buffer);
				}
			break;
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/**
* @brief  ����������ʾGUI������
* @param  ��  
* @retval ��
*/
void  AppTaskDesktop (void  )
{
	FRAMEWIN_SKINFLEX_PROPS FRAMEWIN_pProps;
  RADIO_SKINFLEX_PROPS    RADIO_pProps;

	GUI_SetColor(GUI_BLACK);
	GUI_Clear();
	
	//׼������2�����ڣ�������ʹ�õ����û���������������ڻص�������ʹ��
	WinPara.xSizeLCD = LCD_GetXSize();				            //LCD��Ļ�ߴ�
	WinPara.ySizeLCD = LCD_GetYSize();				            //LCD��Ļ�ߴ�
	WinPara.xPosWin	 = MIDWIN_xPos;							          //���ڵ���ʼλ��
	WinPara.yPosWin  = MIDWIN_yPos;							          //���ڵ���ʼλ��
	WinPara.xSizeWin = WinPara.xSizeLCD;						      //���ڳߴ�
	WinPara.ySizeWin = WinPara.ySizeLCD-WinPara.yPosWin;	//���ڳߴ�

	//ʹ��FLEXƤ��
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	
	/***************************����Ƥ��*****************************/
  FRAMEWIN_GetSkinFlexProps(&FRAMEWIN_pProps,FRAMEWIN_SKINFLEX_PI_ACTIVE);
  FRAMEWIN_pProps.aColorFrame[0]=GUI_BLACK;
  FRAMEWIN_pProps.aColorFrame[1]=GUI_BLACK;
  FRAMEWIN_pProps.aColorFrame[2]=GUI_BLACK;
  FRAMEWIN_pProps.Radius=0;
  FRAMEWIN_pProps.aColorTitle[0]=GUI_BLACK;
  FRAMEWIN_pProps.aColorTitle[1]=GUI_BLACK;
  FRAMEWIN_SetSkinFlexProps(&FRAMEWIN_pProps,FRAMEWIN_SKINFLEX_PI_ACTIVE);
  FRAMEWIN_SetDefaultClientColor(APPBKCOLOR);
  FRAMEWIN_SetDefaultFont(&FONT_XINSONGTI_25);
  FRAMEWIN_SetDefaultTextColor(1,TEXTCOLOR);
  FRAMEWIN_SetDefaultTitleHeight(35);
  FRAMEWIN_SetDefaultTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);

  WINDOW_SetDefaultBkColor(APPBKCOLOR);
  
  EDIT_SetDefaultBkColor(EDIT_CI_ENABLED,APPBKCOLOR);
  EDIT_SetDefaultTextColor(EDIT_CI_ENABLED,TEXTCOLOR);
  
  LISTBOX_SetDefaultBkColor(LISTBOX_CI_UNSEL,APPBKCOLOR);
  LISTBOX_SetDefaultBkColor(LISTBOX_CI_SEL,GUI_BLUE);
  LISTBOX_SetDefaultBkColor(LISTBOX_CI_SELFOCUS,GUI_BLUE);
  LISTBOX_SetDefaultBkColor(LISTBOX_CI_DISABLED,APPBKCOLOR);
  LISTBOX_SetDefaultTextColor(LISTBOX_CI_UNSEL,TEXTCOLOR);
  LISTBOX_SetDefaultTextColor(LISTBOX_CI_SEL,TEXTCOLOR);
  LISTBOX_SetDefaultTextColor(LISTBOX_CI_SELFOCUS,TEXTCOLOR);
  LISTBOX_SetDefaultTextColor(LISTBOX_CI_DISABLED,TEXTCOLOR);
  
  TEXT_SetDefaultTextColor(TEXTCOLOR);
  TEXT_SetDefaultFont(&FONT_XINSONGTI_25);
	   

  RADIO_SetDefaultTextColor(TEXTCOLOR);
  RADIO_GetSkinFlexProps(&RADIO_pProps,RADIO_SKINFLEX_PI_PRESSED);
  RADIO_pProps.aColorButton[3]=GUI_YELLOW;
	RADIO_pProps.ButtonSize = 28;//����Ϊż��
  RADIO_SetSkinFlexProps(&RADIO_pProps,RADIO_SKINFLEX_PI_PRESSED);
	
	RADIO_GetSkinFlexProps(&RADIO_pProps,RADIO_SKINFLEX_PI_UNPRESSED);
	RADIO_pProps.ButtonSize = 28;//����Ϊż��
  RADIO_SetSkinFlexProps(&RADIO_pProps,RADIO_SKINFLEX_PI_UNPRESSED);
  RADIO_SetDefaultFont(&FONT_XINSONGTI_25);


  CHECKBOX_SetDefaultTextColor(TEXTCOLOR);
	

	//�������ڻص�����
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);
	//�������ڶ�ʱ�����Զ�ʱ����CPUʹ������Ϣ
	WM_CreateTimer(WM_HBKWIN, 0, 500, 0);

	/* �������� ״̬����������*/
	CreateTopWin();
	CreateMidWin();
	GUI_Delay(1000);
	
//										ICON_Clicked[7] = 1;
//								
//									FUN_ICON7Clicked();

  while(1)
  {			
    emWinMainApp();
  }
}

/*********************************************END OF FILE**********************/

