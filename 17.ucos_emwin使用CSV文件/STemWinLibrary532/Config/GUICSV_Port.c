/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   XBF��ʽ����emwin�����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./fatfs/drivers/fatfs_flash_spi.h"
#include "GUICSV_Port.h"   
#include "ff.h"

/*�ֿ�洢·��*/
#if (CSV_SOURCE == USE_FLASH_CSV)

	static const uint32_t  CSV_XINSONGTI_25_ADDR	 = (30*4096);
	static const uint32_t  CSV_XINSONGTI_19_ADDR	 = (680*4096);

#elif (CSV_SOURCE == USE_SDCARD_CSV)

	static const char  CSV_STORAGE_ROOT_DIR[] = "0:";
	static const char LANG_CSV_ADDR[] = 	 "0:srcdata/LANG_CSV.csv";
	static const char CSV_XINSONGTI_19_ADDR[] = 	 "0:srcdata/������19.xbf";

#elif (CSV_SOURCE == USE_FLASH_FILESYSTEM_CSV)
	
	static const char CSV_STORAGE_ROOT_DIR[] =  		"1:";
	static const char CSV_XINSONGTI_25_ADDR[] = 	 "1:������25.xbf";
	static const char CSV_XINSONGTI_19_ADDR[] = 	 "1:������19.xbf";

#endif


/*�ֿ�ṹ��*/
//GUI_XBF_DATA 	XBF_XINSONGTI_25_Data;
//GUI_FONT     	CSV_XINSONGTI_25;

//GUI_XBF_DATA 	XBF_XINSONGTI_19_Data;
//GUI_FONT     	CSV_XINSONGTI_19;

/*�洢����ʼ����־*/
static uint8_t storage_init_flag = 0;


/*�ֿ�洢���ļ�ϵͳʱ��Ҫʹ�õı���*/
#if (CSV_SOURCE == USE_SDCARD_CSV || CSV_SOURCE == USE_FLASH_FILESYSTEM_CSV)
	static FIL fnew;										/* file objects */
	static FATFS fs;										/* Work area (file system object) for logical drives */
	static FRESULT res; 
	static UINT br;            				/* File R/W count */
#endif

 /**
  * @brief  ��ȡ�������ݵĻص�����
  * @param  Offset��Ҫ��ȡ��������XBF�ļ��е�ƫ��λ��
  * @param  NumBytes��Ҫ��ȡ���ֽ���
	* @param  pVoid���Զ������ݵ�ָ��
  * @param  pBuffer���洢��ȡ���ݵ�ָ��
  * @retval 0 �ɹ�, 1 ʧ��
  */
// int _cb_CSV_GetData( void * pVoid, const U8 ** ppData, unsigned NumBytes, U32 Off)
//{
//#if (CSV_SOURCE == USE_FLASH_CSV)
//	
//	if (storage_init_flag == 0)
//	{
//		//��ʼ��flash
//		TM_FATFS_FLASH_SPI_disk_initialize();
//		storage_init_flag = 1;
//	}
//	
//	//��pVoid�л�ȡ�ֿ�Ĵ洢��ַ(pvoid��ֵ��GUI_XBF_CreateFont�д���)
//	SPI_FLASH_BufferRead(pBuffer,*(uint32_t *)pVoid+Offset,NumBytes);
//	
//	return 0;
//	
//#elif (CSV_SOURCE == USE_SDCARD_CSV || CSV_SOURCE == USE_FLASH_FILESYSTEM_CSV)

//	if (storage_init_flag == 0)
//	{
//		//����sd���ļ�ϵͳ
//		res = f_mount(&fs,CSV_STORAGE_ROOT_DIR,1);
//		storage_init_flag = 1;
//	}
//	
//	//��pVoid�л�ȡ�ֿ�Ĵ洢��ַ(pvoid��ֵ��GUI_XBF_CreateFont�д���)
//	res = f_open(&fnew , (char *)pVoid, FA_OPEN_EXISTING | FA_READ);

//	if ( res == FR_OK ) 
//	{
//			f_lseek (&fnew, Off);		//ָ��ƫ��
//		
//			//��ȡ����
//			res = f_read( &fnew, ppData, NumBytes, &br );		 
//			
//			f_close(&fnew);
//			
//			return 0;  
//	}    
//	else
//			return 1; 

//#endif

//}


/**
  * @brief  PNG/BITMAPS��ȡ�ⲿͼƬ�ļ����ݵĻص���������ΪGUI_xx_DrawEx�Ļص�����
  * @param  p: �ⲿ����ʱ�����ָ�룬һ��ʹ���ļ�ָ��.
  * @param  NumBytesReq:������ֽ�����
  * @param 	ppData����ȡ��������Ҫ����ָ�롣����ʹ�� ��GetData���������ָ��ָ���λ�á�
  * @param  Off ������˱��Ϊ 1��Ӧ������ָ�����õ�����������ʼλ��
  * @retval ���ؿ��õ������ֽ���
  */
static int _cb_CSV_GetData(void * p, const U8 **ppData, unsigned int NumBytesReq, U32 Off) 
{
	
	UINT    NumBytesRead;
	U8     * pData;
  FIL *p_file;
	FRESULT result;  

	p_file=(FIL *)p;
	pData  = (U8 *)*ppData;
	
	
	result =f_lseek(p_file, Off);
	if (result != FR_OK)
	{
		return 0;
	}
//  printf("lseek:result->(%d) off->(%ld) address->(%d)\n",result,Off,FileAddress);
	/*
	* ��ȡ���ݵ�ָ���λ��
	*/
	result = f_read(p_file, pData, NumBytesReq, &NumBytesRead);
//  printf("read:result->(%d) numreq->(%d),numread->(%d)\n",result,NumBytesReq,NumBytesRead);
	/*
	* ���ض�ȡ������Ч�ֽ���
	*/
	return NumBytesRead;
}

  U8           NumLanguagesCSV;
static	FIL file;
static FATFS fs;													/* Work area (file system object) for logical drives */

 /**
  * @brief  ����XBF����
  * @param  ��
  * @retval ��
  */
void Create_LANG_CSV(void) 
{	
	FRESULT result;  

	const char *readText;
	
	f_mount(&fs,"0:",1);	

	
	result = f_open(&file, LANG_CSV_ADDR, FA_OPEN_EXISTING | FA_READ );

	
	NumLanguagesCSV = GUI_LANG_LoadCSVEx(_cb_CSV_GetData, &file);




	
}



/*********************************************END OF FILE**********************/
