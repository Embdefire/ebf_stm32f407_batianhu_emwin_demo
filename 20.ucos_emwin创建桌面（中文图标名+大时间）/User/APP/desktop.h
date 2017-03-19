#ifndef __DESKTOP_H
#define	__DESKTOP_H

#include "stm32f4xx.h"
#include "GUI.h"
#include "Dialog.h"

//ͼ�����ָ�ʽ
#define ICON_NAME_FORMAT	ICON_NAME_CH_GBK

#define ICON_NAME_EN				0	//ֻʹ��Ӣ��
#define ICON_NAME_CH_GBK		1 //ʹ��GBK���루ֱ���ڳ��������������ַ��������е�ʱ���ú���ת����UTF8��
#define ICON_NAME_CH_UTF8		2	//ֱ��ת���õ�utf8����


//������ɫ
#define DTCOLOR         0X200000
//ͼ�걳����ɫ
#define ICON_COLOR     GUI_LIGHTCYAN
//״̬��������ɫ
#define STATUS_TEXT_COLOR		 GUI_WHITE

//ͼ��ߴ缰λ��
#define ICONVIEW_Width      90  /* �ؼ�ICONVIEWͼ��Ŀ�                         */  
#define ICONVIEW_Height     90  /* �ؼ�ICONVIEWͼ��ĸߣ�����ͼ���Y������    */  
#define ICONVIEW_XSpace     25  /* �ؼ�ICONVIEWͼ���X������                  */ 
#define ICONVIEW_YSpace     20  /* �ؼ�ICONVIEWͼ���Y������                  */ 
#define ICONVIEW_XPos       16  /* �ؼ�ICONVIEW������ͼ���X�����������              */
#define ICONVIEW_YPos       20  /* �ؼ�ICONVIEW������ͼ���Y�����������              */
#define MIDWIN_yPos         (40+160)  /* �м䴰��Y������ʼλ�ü��������ֿ��ø߶�  */
#define MIDWIN_xPos          0  /* �м䴰��X������ʼλ��                    */

#define TIMEWIN_xPos				70 //ʱ�䴰�ڵ�x��ʼ
#define TIMEWIN_yPos				60 //ʱ�䴰�ڵ�y��ʼ
#define TIMEWIN_xSize				340 //ʱ�䴰�ڵ�x��С
#define TIMEWIN_ySize			  100 //ʱ�䴰�ڵ�y��С
//GUI_ID
#define GUI_ID_STATUS_CPU_USAGE 		(GUI_ID_USER+10)

//���ڼ�¼���洰�ڲ���
typedef struct WIN_PARA{			//����ʹ�õ����û���������������ڻص�������ʹ��
	int xSizeLCD, ySizeLCD;			//LCD��Ļ�ߴ�
	int xPosWin,  yPosWin;			//���ڵ���ʼλ��
	int xSizeWin, ySizeWin;			//���ڳߴ�	
	WM_HWIN hWinMid;				//����ʾ���ľ�����в���	
}WIN_PARA;

/* ��������ICONVIEWͼ��Ĵ��� */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;


//APP ��ͼ������
extern GUI_CONST_STORAGE GUI_BITMAP bmIdea;
extern GUI_CONST_STORAGE GUI_BITMAP bmCalculator;
extern GUI_CONST_STORAGE GUI_BITMAP bmClock;
extern GUI_CONST_STORAGE GUI_BITMAP bmCompass;
extern GUI_CONST_STORAGE GUI_BITMAP bmMessages;
extern GUI_CONST_STORAGE GUI_BITMAP bmMusic;
extern GUI_CONST_STORAGE GUI_BITMAP bmPhone;
extern GUI_CONST_STORAGE GUI_BITMAP bmPhotos;
extern GUI_CONST_STORAGE GUI_BITMAP bmVideos;
extern GUI_CONST_STORAGE GUI_BITMAP bmSafari;
extern GUI_CONST_STORAGE GUI_BITMAP bmWeather;
extern GUI_CONST_STORAGE GUI_BITMAP bmwifi;
extern GUI_CONST_STORAGE GUI_BITMAP bmSiri;
extern GUI_CONST_STORAGE GUI_BITMAP bmCamera;
extern GUI_CONST_STORAGE GUI_BITMAP bmUSB;


/*��������*/
void  AppTaskDesktop (void  );

#endif /* __DESKTOP_H */
