/**
  ******************************************************************************
  * @file    MusicDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ���ֵ�Ӧ�ô���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

// USER START (Optionally insert additional includes)
#include "includes.h"
#include "DIALOG.h"
#include "./Bsp/wm8978/bsp_wm8978.h"
#include "string.h"
#include "mp3dec.h"
// USER END

#define LRC_GBK   1  //1��ʾLRC���ΪGBK���룬0��ʾUTF8����
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
// USER START (Optionally insert additional defines)
/*--------------------  (ʹ��U2C.exeС��������)  ------------------------------------*/
/*
*  U2C.exeС����ʹ�÷������½�һ��TXT�ı��ļ��������ת���ı������ΪUTF8�����ʽ�ļ���
*  ��U2C.exeС���ߣ��ȵ����ļ��ļ���Ȼ��ѡ��"Convert"����ת������ɺ���ͬ·�������
*  ͬ��.c�ļ���
*/
#define UTF8_MUSICDLG       "\xe9\x9f\xb3\xe4\xb9\x90\xe6\x92\xad\xe6\x94\xbe\xe5\x99\xa8"  //���ֲ�����
#define UTF8_LYRIC          "\xe6\xad\x8c\xe8\xaf\x8d" //���
#define UTF8_SONG           "\xe6\xad\x8c\xe6\x9b\xb2" //����
#define UTF8_SDCARDERROR    "SD\xe5\x8d\xa1\xe8\xaf\xbb\xe5\x8f\x96\xe5\xa4\xb1\xe8\xb4\xa5"//SD��ȡʧ��
/*-----------------------------------------------------------------------------------*/

#define MUSIC_MAX_NUM           20
#define FILE_NAME_LEN           100			
#define MUSIC_NAME_LEN          100		
#define LYRIC_MAX_SIZE          200				
#define MAX_LINE_LEN            200				
#define _DF1S                   0x81
#define BUFFER_SIZE             1024*8
#define COMDATA_SIZE					1024*5
#define COMDATA_UTF8_SIZE				1024*2
#define MP3BUFFER_SIZE          2304		
#define INPUTBUF_SIZE           3000	

/* ����״̬ */
typedef enum
{
	STA_IDLE = 0,	/* ����״̬ */
	STA_PLAYING,	/* ����״̬ */
	STA_STOP,	    /* ֹͣ״̬ */
  STA_SWITCH,   /* �и�״̬ */
}PLAYER_STATE;

typedef struct
{
	PLAYER_STATE ucStatus;			/* ״̬��0��ʾ������1 ��ʾ������ */	
	uint8_t ucVolume;			      /* ��ǰ�������� */
  uint8_t ucMute;             /* ������־ 1��������0������  */
	uint32_t ucFreq;			      /* ����Ƶ�� */
  uint32_t ucbps;            /* ������ ��ÿ�봫�Ͷ��ٸ�λ */
}MUSIC_TYPE;


typedef __packed struct 
{ 
	uint8_t 	indexsize; 	
  uint8_t		length_tbl[LYRIC_MAX_SIZE]; 
 	uint16_t	addr_tbl[LYRIC_MAX_SIZE];										
 	int32_t		curtime;										
  int32_t		oldtime;	
  int32_t 	time_tbl[LYRIC_MAX_SIZE];		
}LYRIC;

// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
extern GUI_CONST_STORAGE GUI_BITMAP bmback;
extern GUI_CONST_STORAGE GUI_BITMAP bmnext;
extern GUI_CONST_STORAGE GUI_BITMAP bmplay;
extern GUI_CONST_STORAGE GUI_BITMAP bmstop;
extern GUI_CONST_STORAGE GUI_BITMAP bmvoice;
extern GUI_CONST_STORAGE GUI_BITMAP bmvolume_mute;
extern GUI_CONST_STORAGE GUI_BITMAP bmvolume_medium;

extern FATFS   fs[2];													  /* Work area (file system object) for logical drives */
extern FRESULT f_result; 
extern FIL     f_file;
extern UINT    f_num;

static uint8_t play_index = 0;

static GUI_HMEM hMEM_Music;
static GUI_HMEM hCOMMemory;

static uint8_t *comdata;

#if LRC_GBK
static GUI_HMEM hCOMUTF8Memory;
static uint8_t *comdataUTF8;
#endif


static char *pMEM_Music;
static uint16_t *buffer_Music[2];
static uint8_t *inputbuf;
static uint8_t chgsch=0;
static uint8_t clksch=0;

static MUSIC_TYPE Music;
static uint8_t Isread_Music=0;
static uint8_t bufflag_Music=0;//0 -> use musicbuf[0][]; 1 -> use musicbuf[1][]

static char path[100]="0:";																		
static uint8_t  file_num = 0;																	
static unsigned char playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];
static unsigned char lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];

static LYRIC *lrc;

static HMP3Decoder		Mp3Decoder;			/* mp3������ָ��	*/
static MP3FrameInfo		Mp3FrameInfo;		/* mP3֡��Ϣ  */

// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateMusic[] = {
  { FRAMEWIN_CreateIndirect, "music",     0,               0,   0,   480, 854, 0, 0x64, 0 },
  { TEXT_CreateIndirect,     "lyric1",    GUI_ID_TEXT0,    10, 560, 450, 60, 0, 0x0,  0 },//��һ���
  { TEXT_CreateIndirect,     "lyric2",    GUI_ID_TEXT1,    10, 620, 450, 80,  0, 0x0,  0 },//��ǰ���
  { TEXT_CreateIndirect,     "lyric3",    GUI_ID_TEXT2,    10, 710, 450, 60, 0, 0x0,  0 },//��һ���
  
  { TEXT_CreateIndirect,     "mucis",     GUI_ID_TEXT3,    0,   0,   450, 35,  0, 0x0,  0 },//���ڲ��ŵĸ�����
  { TEXT_CreateIndirect,     "ptime",     GUI_ID_TEXT4,    360,  423, 50,  20,  0, 0x0,  0 },//��ǰ���ŵ���ʱ��˵��
  { TEXT_CreateIndirect,     "/",     		GUI_ID_TEXT7,    395,  423, 20,  20,  0, 0x0,  0 },//�ָ���
	{ TEXT_CreateIndirect,     "atime",     GUI_ID_TEXT5,    400, 423, 50,  20,  0, 0x0,  0 },//������ʱ��˵��
  { TEXT_CreateIndirect,     "volume",    GUI_ID_TEXT6,    350, 497, 50,  20,  0, 0x0,  0 },//��������˵��
  
  { LISTBOX_CreateIndirect,  "musiclist", GUI_ID_LISTBOX0, 10,   40,  450, 335, 0, 0x0,  0 },//�����б�
  { SLIDER_CreateIndirect,   "schedule",  GUI_ID_SLIDER0,  30,  375, 410, 50,  0, 0x0,  0 },//���Ž���
  { SLIDER_CreateIndirect,   "volume",    GUI_ID_SLIDER1,  140, 490, 200, 30,  0, 0x0,  0 },//������
  { BUTTON_CreateIndirect,   "play",      GUI_ID_BUTTON0,  200, 435, 40,  40,  0, 0x0,  0 },//���Ű�ť  
  { BUTTON_CreateIndirect,   "back",      GUI_ID_BUTTON1,  120, 435, 40,  40,  0, 0x0,  0 },//��һ�װ�ť
  { BUTTON_CreateIndirect,   "next",      GUI_ID_BUTTON2,  280, 435, 40,  40,  0, 0x0,  0 },//��һ�װ�ť 
  { BUTTON_CreateIndirect,   "volume",    GUI_ID_BUTTON3,  92,  498, 30,  25,  0, 0x0,  0 },//������ť
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
/**
  * @brief  scan_files �ݹ�ɨ��sd���ڵĸ����ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
	char file_name[FILE_NAME_LEN];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//���ļ���֧��
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 

  
  res = f_opendir(&dir, path); //��Ŀ¼
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      res = f_readdir(&dir, &fno); 										//��ȡĿ¼�µ�����
      if (res != FR_OK || fno.fname[0] == 0) break; 	//Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      if(strstr(path,"recorder")!=NULL)continue;       //�ӹ�¼���ļ�
      if (*fn == '.') continue; 											//���ʾ��ǰĿ¼������			
      if (fno.fattrib & AM_DIR) 
			{ 																							//Ŀ¼���ݹ��ȡ
        sprintf(&path[i], "/%s", fn); 							//�ϳ�����Ŀ¼��
        res = scan_files(path);											//�ݹ���� 
        if (res != FR_OK) 
					break; 																		//��ʧ�ܣ�����ѭ��
        path[i] = 0; 
      } 
			else 
			{ 
				//printf("%s/%s\r\n", path, fn);								//����ļ���
				if(strstr(fn,".wav")||strstr(fn,".WAV")||strstr(fn,".mp3")||strstr(fn,".MP3"))//�ж��Ƿ�mp3��wav�ļ�
				{
					if ((strlen(path)+strlen(fn)<FILE_NAME_LEN)&&(file_num<MUSIC_MAX_NUM))
					{
						sprintf(file_name, "%s/%s", path, fn);						
						memcpy(playlist[file_num],file_name,strlen(file_name));
						memcpy(lcdlist[file_num],fn,strlen(fn));						
						file_num++;//��¼�ļ�����
					}
				}//if mp3||wav
      }//else
    } //for
  } 
  return res; 
}



static uint16_t getonelinelrc(uint8_t *buff,uint8_t *str,int16_t len)
{
	uint16_t i;
	for(i=0;i<len;i++)
	{
		*(str+i)=*(buff+i);
		if((*(buff+i)==0x0A)||(*(buff+i)==0x00))
		{
			*(buff+i)='\0';
			*(str+i)='\0';
			break;
		}
	}
	return (i+1);
}
static void lrc_chg_suffix(uint8_t *name,const char *sfx)
{		    	     
	while(*name!='\0')name++;
	while(*name!='.')name--;
	*(++name)=sfx[0];
	*(++name)=sfx[1];
	*(++name)=sfx[2];
	*(++name)='\0';
}
static void lrc_sequence(LYRIC	*lyric)
{
	uint16_t i=0,j=0;
	uint16_t temp=0;
	if (lyric->indexsize == 0)return;
	
	for(i = 0; i < lyric->indexsize - 1; i++)
	{
		for(j = i+1; j < lyric->indexsize; j++)
		{
			if(lyric->time_tbl[i] > lyric->time_tbl[j])
			{
				temp = lyric->time_tbl[i];
				lyric->time_tbl[i] = lyric->time_tbl[j];
				lyric->time_tbl[j] = temp;

				temp = lyric->addr_tbl[i];
				lyric->addr_tbl[i] = lyric->addr_tbl[j];
				lyric->addr_tbl[j] = temp;
			}
		}
	}	
}
static void lyric_analyze(LYRIC	*lyric,uint8_t *strbuf)
{
	uint8_t strtemp[MAX_LINE_LEN]={0};
	uint8_t *pos=0;
	uint8_t sta=0,strtemplen=0;
	uint16_t lrcoffset=0;
	uint16_t str_len=0,i=0;
	
	pos=strbuf;
	str_len=strlen((const char *)strbuf);
	if(str_len==0)return;
	i=str_len;
	while(--i)
	{
		if(*pos=='[')
			sta=1;
		else if((*pos==']')&&(sta==1))
			sta=2;
	  else if((sta==2)&&(*pos!=' '))
		{
			sta=3;
			break;
		}
		pos++; 
	}
	if(sta!=3)return;	
	lrcoffset=0;
	lyric->indexsize=0;
	while(lrcoffset<=str_len)
	{
		i=getonelinelrc(strbuf+lrcoffset,strtemp,MAX_LINE_LEN);
		lrcoffset+=i;
//		printf("lrcoffset:%d,i:%d\n",lrcoffset,i);
		strtemplen=strlen((const char *)strtemp);
		pos=strtemp;
		while(*pos!='[')
			pos++;
		pos++;
		if((*pos<='9')&&(*pos>='0'))
		{
			lyric->time_tbl[lyric->indexsize]=(((*pos-'0')*10+(*(pos + 1)-'0'))*60+((*(pos+3)-'0')*10+(*(pos+4)-'0')))*100+((*(pos+6)-'0')*10+(*(pos+7)-'0'));
			lyric->addr_tbl[lyric->indexsize]=(uint16_t)(lrcoffset-strtemplen+10); 
			lyric->length_tbl[lyric->indexsize]=strtemplen-10;
			lyric->indexsize++;
		}		
//		else
//				continue;		
	}
}



void MUSIC_I2S_DMA_TX_Callback(void)
{
	if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //��ǰ��ȡMemory1����
	{
		bufflag_Music=0;
	}
	else
	{
		bufflag_Music=1;
	}
	Isread_Music=1;
}

/**
  * @brief   WAV��ʽ��Ƶ����������
	* @note   
  * @param  ��
  * @retval ��
  */
void wavplayer(const char *wavfile,WM_HWIN hWin)
{
	WavHead wav;
  DWORD pos;
  static uint8_t timecount;
  uint16_t curtime,alltime;
  char timestr[6]={0};
  static uint8_t lyriccount=0;

	f_result=f_open(&f_file,wavfile,FA_READ);
	if(f_result!=FR_OK)
	{
		printf("Open wavfile fail!!!->%d\r\n",f_result);
		f_result = f_close (&f_file);
		return;
	}
	f_result = f_read(&f_file,&wav,sizeof(WavHead),&f_num);//��ȡWAV�ļ�ͷ
  
  /* ��ʼ��ȫ�ֱ��� */
	Music.ucFreq =  wav.dwSamplesPerSec;
	Music.ucbps =  Music.ucFreq*32;
  
  alltime=f_file.fsize*8/Music.ucbps;
  sprintf(timestr,"%02d:%02d",alltime/60,alltime%60);
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5),timestr);
  
  SLIDER_SetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER0),0);
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"");
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),UTF8_LYRIC);
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),"");
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4),"00:00");   

  GUI_Delay(50);
	wm8978_Reset();		/* ��λWM8978����λ״̬ */		
  GUI_Delay(50);
  
	Music.ucStatus = STA_PLAYING;		/* ����״̬ */

	/* ����WM8978оƬ������ΪDAC�����Ϊ���� */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* ����������������ͬ���� */
	wm8978_SetOUT1Volume(Music.ucVolume);

	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
	
	I2S_GPIO_Config();
	I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,Music.ucFreq);
	I2S_DMA_TX_Callback=MUSIC_I2S_DMA_TX_Callback;			//�ص�����ָwav_i2s_dma_callback
	I2S_Play_Stop();
	
	f_result = f_read(&f_file,buffer_Music[0],BUFFER_SIZE*2,&f_num);
	f_result = f_read(&f_file,buffer_Music[1],BUFFER_SIZE*2,&f_num);
	
	I2Sx_TX_DMA_Init(buffer_Music[0],buffer_Music[1],BUFFER_SIZE);
	Isread_Music=0;
	bufflag_Music=0;
	chgsch=0;
  lyriccount=0;
  timecount=0;
  
	I2S_Play_Start();
	/* ����������ѭ���� */
	while((Music.ucStatus==STA_PLAYING)||(Music.ucStatus==STA_STOP))
	{
		while(Isread_Music==0)
		{
      if(chgsch==0) 
      { 
        if(timecount>=10)      
        { 
          curtime=f_file.fptr*8/Music.ucbps;                                        //��ȡ��ǰ���Ž���(��λ��s)
          sprintf(timestr,"%02d:%02d",curtime/60,curtime%60);
          TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4),timestr);                  //����ʱ����ʾ 
          SLIDER_SetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER0),curtime*255/alltime); //����ʱ�������    

          lrc->curtime = curtime;
					//+100����ǰ��ʾ����ʾ��Ҫ����ʱ��
          if((lrc->oldtime <= lrc->curtime*100+100)&&(lrc->indexsize>10))
          {
						#if LRC_GBK
							com_gbk2utf8(( char *)&comdata[lrc->addr_tbl[lyriccount]-1],(char *)comdataUTF8);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),(const char *)comdataUTF8);
						#else
	            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),(const char *)&comdata[lrc->addr_tbl[lyriccount]-1]);
						#endif

            if(lyriccount>0)
            {
							#if LRC_GBK
								com_gbk2utf8(( char *)&comdata[lrc->addr_tbl[lyriccount-1]-1],(char *)comdataUTF8);
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),(const char *)comdataUTF8);
							#else
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),(const char *)&comdata[lrc->addr_tbl[lyriccount-1]-1]);
							#endif

            }
            else
              TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"");
            
            if(lyriccount < lrc->indexsize-1)
            {
							#if LRC_GBK
								com_gbk2utf8(( char *)&comdata[lrc->addr_tbl[lyriccount+1]-1],(char *)comdataUTF8);
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),(const char *)comdataUTF8);
							#else
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),(const char *)&comdata[lrc->addr_tbl[lyriccount+1]-1]);
							#endif							
            }
            else
              TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),"");
						
            do{
              lyriccount++;					
              if(lyriccount>=lrc->indexsize)
              {
                lrc->oldtime=0xffffff;
                break;
              }
              lrc->oldtime=lrc->time_tbl[lyriccount];
            }while(lrc->oldtime<=(lrc->curtime*100));
          }
          
          timecount=0;
        }
      }
      else
      {
        uint8_t temp=0;
        temp=SLIDER_GetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER0));
        pos=f_file.fsize/255*temp;
        if(pos<sizeof(WavHead))pos=sizeof(WavHead);
        if(wav.wBitsPerSample==24)temp=12;
        else temp=8;
        if((pos-sizeof(WavHead))%temp)
        {
          pos+=temp-(pos-sizeof(WavHead))%temp;
        }        
        f_lseek(&f_file,pos);
        lrc->oldtime=0;
        lyriccount=0;
        chgsch=0;
      }
      if(tpad_flag)WM_DeleteWindow(hWin);
      GUI_Delay(1);
		}
    timecount++;
		if(bufflag_Music==0)
		{
			f_result = f_read(&f_file,buffer_Music[0],BUFFER_SIZE*2,&f_num);	
		}
		else
		{
			f_result = f_read(&f_file,buffer_Music[1],BUFFER_SIZE*2,&f_num);
		}
//		printf("read data:bufflag->%d\n",bufflag);
		Isread_Music=0;
		if((f_result!=FR_OK)||(f_file.fptr==f_file.fsize))
		{
      
			I2S_Play_Stop();
      SLIDER_SetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER0),0);
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"");
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),UTF8_LYRIC);
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),"");
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4),"00:00");
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5),"00:00"); 
      
			printf("��������߶�ȡ�����˳�...\r\n");
      if(play_index<file_num-1)
				play_index++;
			else
				play_index = 0;
      Music.ucStatus = STA_SWITCH;
			f_result=FR_OK;
			f_file.fptr=0;
			f_close(&f_file);
			I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
			wm8978_Reset();	/* ��λWM8978����λ״̬ */
			return;
		}		
	}
}


/**
  * @brief  ��ȡMP3ID3V2�ļ�ͷ�Ĵ�С
  * @param  ����MP3�ļ���ͷ�����ݣ�����10���ֽ�
  * @retval ID3V2�Ĵ�С
  */
uint32_t mp3_GetID3V2_Size(unsigned char *buf)
{
 uint32_t ID3V2_size;
	
 if(buf[0] == 'I' && buf[1] == 'D' && buf[2] =='3')//����ID3V2
 {
 	 ID3V2_size = (buf[6]<<21) | (buf[7]<<14) | (buf[8]<<7) | buf[9];
 }
 else//������ID3V2
	 ID3V2_size = 0;

 return ID3V2_size;

}
/**
  * @brief   MP3��ʽ��Ƶ����������
	* @note   
  * @param  ��
  * @retval ��
  */
void mp3player(const char *mp3file,WM_HWIN hWin)
{
  DWORD pos;
  static uint8_t timecount;
  uint16_t curtime,alltime;
  char timestr[6]={0};
  static uint8_t lyriccount=0;
	int	read_offset = 0;				/* ��ƫ��ָ�� */
	int	bytes_left = 0;					/* ʣ���ֽ��� */	
  uint8_t *read_ptr;
  uint32_t frames=0;
	int err=0, i=0, outputSamps=0;	
	uint32_t time_sum = 0; //���㵱ǰ�Ѳ��ŵ���ʱ��λ��
	uint16_t frame_size;
	uint32_t ID3V2_size;
	
	//���ò�����
	Music.ucFreq = 0;
  
	f_result=f_open(&f_file,mp3file,FA_READ);
	if(f_result!=FR_OK)
	{
		printf("Open mp3file fail!!!->%d\r\n",f_result);
		f_result = f_close (&f_file);
		return;
	}

	//��ʼ��MP3������
	Mp3Decoder = MP3InitDecoder();	
	if(Mp3Decoder==0)
	{
		printf("��ʼ��helix������豸\n");
		return;	/* ֹͣ���� */
	}

  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5),"00:00");  
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4),"00:00");  
  SLIDER_SetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER0),0);
  
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"");
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),UTF8_LYRIC);
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),""); 
   
  
  GUI_Delay(50);  
	wm8978_Reset();		/* ��λWM8978����λ״̬ */		
  GUI_Delay(50);
	Music.ucStatus = STA_PLAYING;		/* ����״̬ */
  Music.ucFreq=I2S_AudioFreq_Default;

	/* ����WM8978оƬ������ΪDAC�����Ϊ���� */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* ����������������ͬ���� */
	wm8978_SetOUT1Volume(Music.ucVolume);

	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
	
	I2S_GPIO_Config();
	I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,Music.ucFreq);
	I2S_DMA_TX_Callback=MUSIC_I2S_DMA_TX_Callback;			//�ص�����ָwav_i2s_dma_callback
	I2Sx_TX_DMA_Init(buffer_Music[0],buffer_Music[1],MP3BUFFER_SIZE);
  I2S_Play_Stop();
  
	Isread_Music=0;
	bufflag_Music=0;
	chgsch=0;
  lyriccount=0;
  timecount=0;
  	
	f_result=f_read(&f_file,inputbuf,INPUTBUF_SIZE,&f_num);
	if(f_result!=FR_OK)
	{
		printf("��ȡ%sʧ�� -> %d\r\n",mp3file,f_result);
		MP3FreeDecoder(Mp3Decoder);
		return;
	}		
	
	//��ȡID3V2�Ĵ�С����ƫ������λ��
	ID3V2_size = mp3_GetID3V2_Size(inputbuf);
	f_lseek(&f_file,ID3V2_size);	
	
	f_result=f_read(&f_file,inputbuf,INPUTBUF_SIZE,&f_num);
	if(f_result!=FR_OK)
	{
		printf("��ȡ%sʧ�� -> %d\r\n",mp3file,f_result);
		MP3FreeDecoder(Mp3Decoder);
		return;
	}	
  read_ptr=inputbuf;		
  
	/* ����������ѭ���� */
	while((Music.ucStatus==STA_PLAYING)||(Music.ucStatus==STA_STOP))
	{
    read_offset = MP3FindSyncWord(read_ptr, bytes_left);
    if(read_offset < 0)																		//û���ҵ�ͬ����
		{
			f_result=f_read(&f_file,inputbuf,INPUTBUF_SIZE,&f_num);
			if(f_result!=FR_OK)
			{
				printf("��ȡ%sʧ�� -> %d\r\n",mp3file,f_result);
				break;
			}
			read_ptr=inputbuf;
			bytes_left=f_num;
			continue;																						
		}
		read_ptr += read_offset;					//ƫ����ͬ���ֵ�λ��
		bytes_left -= read_offset;				//ͬ����֮������ݴ�С	
		if(bytes_left < 1024)							//��������
		{
      
			/* ע������ط���Ϊ���õ���DMA��ȡ������һ��Ҫ4�ֽڶ���  */
			i=(uint32_t)(bytes_left)&3;									//�ж϶�����ֽ�
			if(i) i=4-i;														//��Ҫ������ֽ�
			memcpy(inputbuf+i, read_ptr, bytes_left);	//�Ӷ���λ�ÿ�ʼ����
			read_ptr = inputbuf+i;										//ָ�����ݶ���λ��
			f_result = f_read(&f_file, inputbuf+bytes_left+i, INPUTBUF_SIZE-bytes_left-i, &f_num);//��������
			bytes_left += f_num;										//��Ч��������С
		}
		err = MP3Decode(Mp3Decoder, &read_ptr, &bytes_left, (short *)buffer_Music[bufflag_Music], 0);					//��ʼ���� ������mp3����ṹ�塢������ָ�롢��������С�������ָ�롢���ݸ�ʽ
		frames++;	
		time_sum +=26;//ÿ֡26ms

    if (err != ERR_MP3_NONE)									//������
		{
			switch (err)
			{
				case ERR_MP3_INDATA_UNDERFLOW:
					printf("ERR_MP3_INDATA_UNDERFLOW\r\n");
					f_result = f_read(&f_file, inputbuf, INPUTBUF_SIZE, &f_num);
					read_ptr = inputbuf;
					bytes_left = f_num;
					break;		
				case ERR_MP3_MAINDATA_UNDERFLOW:
					/* do nothing - next call to decode will provide more mainData */
					printf("ERR_MP3_MAINDATA_UNDERFLOW\r\n");
					break;		
				default:
					printf("UNKNOWN ERROR:%d\r\n", err);		
					// ������֡
					if (bytes_left > 0)
					{
						bytes_left --;
						read_ptr ++;
					}	
					break;
			}
			Isread_Music=1;
		}
    else		//�����޴���׼�������������PCM
		{
			MP3GetLastFrameInfo(Mp3Decoder, &Mp3FrameInfo);		//��ȡ������Ϣ				
			/* �����DAC */
			outputSamps = Mp3FrameInfo.outputSamps;							//PCM���ݸ���
			 			
			if (outputSamps > 0)
			{
				if (Mp3FrameInfo.nChans == 1)	//������
				{
					//������������Ҫ����һ�ݵ���һ������
					for (i = outputSamps - 1; i >= 0; i--)
					{
						buffer_Music[bufflag_Music][i * 2] = buffer_Music[bufflag_Music][i];
						buffer_Music[bufflag_Music][i * 2 + 1] = buffer_Music[bufflag_Music][i];
					}
					outputSamps *= 2;
				}//if (Mp3FrameInfo.nChans == 1)	//������
			}//if (outputSamps > 0)
					
		/* ���ݽ�����Ϣ���ò����� */
			if (Mp3FrameInfo.samprate != Music.ucFreq)	//������ 
			{
				Music.ucFreq = Mp3FrameInfo.samprate;
				Music.ucbps=Mp3FrameInfo.bitrate;        //�����ʣ�mp3һ��Ϊ320Kbps

				//����ÿ֡�Ĵ�С
			  frame_size = (((Mp3FrameInfo.version == MPEG1)? 144:72)*Mp3FrameInfo.bitrate)/Mp3FrameInfo.samprate+Mp3FrameInfo.paddingBit;

				//������ʱ�䣬�ļ���С��ȥID3V2��ID3V1������ÿ֡��С�������֡������Ŀ��ÿ֡ʱ��26ms�������ʱ��
				//alltime=((f_file.fsize-ID3V2_size-128)/frame_size)*26/1000;    
				
				//�����Ϲ�ʽ�Ļ�����ʹ�ý�1����������1��Ĳ��� +1��				
				alltime=(((f_file.fsize-ID3V2_size-128)/frame_size)*26+1000)/1000;         
				

        sprintf(timestr,"%02d:%02d",alltime/60,alltime%60);
        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5),timestr);
        
				printf(" \r\n Bitrate       %dKbps", Mp3FrameInfo.bitrate/1000);
				printf(" \r\n Samprate      %dHz", Music.ucFreq);
				printf(" \r\n BitsPerSample %db", Mp3FrameInfo.bitsPerSample);
				printf(" \r\n nChans        %d", Mp3FrameInfo.nChans);
				printf(" \r\n Layer         %d", Mp3FrameInfo.layer);
				printf(" \r\n Version       %d", Mp3FrameInfo.version);
				printf(" \r\n OutputSamps   %d", Mp3FrameInfo.outputSamps);
				printf("\r\n");			

				if(Music.ucFreq >= I2S_AudioFreq_Default)	//I2S_AudioFreq_Default = 2��������֡��ÿ�ζ�Ҫ������
				{
					I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,Music.ucFreq);						//���ݲ������޸�iis����
					I2Sx_TX_DMA_Init(buffer_Music[0],buffer_Music[1],outputSamps);//MP3BUFFER_SIZE);
				}
				I2S_Play_Start();
			}
			
		}//else ��������
    
		while(Isread_Music==0)
		{
      if(chgsch==0) 
      { 
        if(timecount>=20)      
        { 
					curtime = time_sum/1000; 
          sprintf(timestr,"%02d:%02d",curtime/60,curtime%60);
          TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4),timestr);                  //����ʱ����ʾ 
					
					if(clksch != 1)  //��������������ˣ��򲻸��¸�ֵ       
						SLIDER_SetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER0),curtime*255/alltime); //����ʱ�������    
						
          lrc->curtime = curtime;          
					//+100����ǰ��ʾ����ʾ��Ҫ����һ��ʱ��
          if((lrc->oldtime <= lrc->curtime*100+100)&&(lrc->indexsize>10))
          {
						#if LRC_GBK
							com_gbk2utf8(( char *)&comdata[lrc->addr_tbl[lyriccount]-1],(char *)comdataUTF8);
							TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),(const char *)comdataUTF8);
						#else
	            TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),(const char *)&comdata[lrc->addr_tbl[lyriccount]-1]);
						#endif

            if(lyriccount>0)
            {
							#if LRC_GBK
								com_gbk2utf8(( char *)&comdata[lrc->addr_tbl[lyriccount-1]-1],(char *)comdataUTF8);
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),(const char *)comdataUTF8);
							#else
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),(const char *)&comdata[lrc->addr_tbl[lyriccount-1]-1]);
							#endif

            }
            else
              TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"");
            
            if(lyriccount < lrc->indexsize-1)
            {
							#if LRC_GBK
								com_gbk2utf8(( char *)&comdata[lrc->addr_tbl[lyriccount+1]-1],(char *)comdataUTF8);
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),(const char *)comdataUTF8);
							#else
								TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),(const char *)&comdata[lrc->addr_tbl[lyriccount+1]-1]);
							#endif							
            }
            else
              TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),"");
            
            do{
              lyriccount++;					
              if(lyriccount>=lrc->indexsize)
              {
                lrc->oldtime=0xffffff;
                break;
              }
              lrc->oldtime=lrc->time_tbl[lyriccount];
            }while(lrc->oldtime<=(lrc->curtime*100));
          }
          
          timecount=0;
        }
      }
      else
      {
        uint8_t temp=0;	
				
				//���ݽ�������������λ��				
        temp=SLIDER_GetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER0));        
				
				//�����������ʾ��ʱ��
				time_sum = (float)alltime/255*temp*1000;  	
				//����ʱ������ļ�λ�ò���ת����λ��
				pos = ID3V2_size + (time_sum/26)*frame_size;
        f_result = f_lseek(&f_file,pos);
				
        lrc->oldtime=0;
        lyriccount=0;
        chgsch=0;
      }
      if(tpad_flag)WM_DeleteWindow(hWin);
      GUI_Delay(1);
		}
    timecount++;		
		Isread_Music=0;
    
		if((f_result!=FR_OK)||(f_file.fptr==f_file.fsize))
		{
      MP3FreeDecoder(Mp3Decoder);
			I2S_Play_Stop();
      SLIDER_SetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER0),0);
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),"");
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),UTF8_LYRIC);
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),"");
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4),"00:00");
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5),"00:00"); 
      
			printf("��������߶�ȡ�����˳�...\r\n");
      if(play_index<file_num-1)
				play_index++;
			else
				play_index = 0;
      Music.ucStatus = STA_SWITCH;
			f_result=FR_OK;
			f_file.fptr=0;
			f_close(&f_file);
			I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
			wm8978_Reset();	/* ��λWM8978����λ״̬ */
			return;
		}		
	}
}

// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogMusic(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  uint8_t i=0,j=0,k=0;
  char music_name[FILE_NAME_LEN];
	uint16_t UCname[30]={0},gbkdata=0;
  char tmp[4];  
  // USER END

  switch (pMsg->MsgId) {
   case WM_DELETE:
		// USER START (Optionally insert additional code for further widget initialization)   
		APP_TRACE_DBG(("MusicDLG delete\n"));		
//		Flag_ICON1 = 0;
		UserApp_Running = 0;
    tpad_flag=0;
    Music.ucStatus = STA_IDLE;		/* ����״̬ */
   	I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
		wm8978_Reset();	/* ��λWM8978����λ״̬ */
    buffer_Music[0]=NULL;
    buffer_Music[1]=NULL;
    pMEM_Music=NULL;
    GUI_ALLOC_Free(hMEM_Music);    

#if LRC_GBK
	  GUI_ALLOC_Free(hCOMMemory);    
		GUI_ALLOC_Free(hCOMUTF8Memory);    
#endif

		// USER END
	break;
  case WM_INIT_DIALOG:
  //
  // Initialization of 'music'
  //
  hItem = pMsg->hWin;
  FRAMEWIN_SetText(hItem,UTF8_MUSICDLG);
  FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
  //
  // Initialization of 'lyric1'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
  TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_BOTTOM);
  TEXT_SetFont(hItem, &FONT_XINSONGTI_19);
  TEXT_SetText(hItem,"");
  //
  // Initialization of 'lyric2'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
  TEXT_SetFont(hItem, &FONT_XINSONGTI_25);
  TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetWrapMode(hItem,GUI_WRAPMODE_CHAR);
	TEXT_SetText(hItem,UTF8_LYRIC);
  TEXT_SetTextColor(hItem,GUI_ORANGE);
  //
  // Initialization of 'lyric3'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
  TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_TOP);
	TEXT_SetWrapMode(hItem,GUI_WRAPMODE_CHAR);
  TEXT_SetFont(hItem, &FONT_XINSONGTI_19);
	TEXT_SetText(hItem,"");
  //
  // Initialization of 'mucis'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT3);
  TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
	TEXT_SetTextColor(hItem,GUI_GREEN);
  TEXT_SetFont(hItem, &FONT_XINSONGTI_25);
	TEXT_SetText(hItem,UTF8_SONG);
  //
  // Initialization of 'PlayTime'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT4);	
  TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
  TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_TOP);
  TEXT_SetText(hItem,"00:00");
	
	//
  // Initialization of '/'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT7);	
  TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
  TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_TOP);
  TEXT_SetText(hItem,"/");

  //
  // Initialization of 'AllTime'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT5);
  TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
  TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_TOP);
  TEXT_SetText(hItem,"00:00");
  //
  // Initialization of 'volume'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT6);
  TEXT_SetFont(hItem, GUI_FONT_16_ASCII);
  TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_TOP);
  sprintf(tmp,"%2d%%",Music.ucVolume*100/63);
  TEXT_SetText(hItem,tmp);
  //
  // Initialization of 'Schedule'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0);
  SLIDER_SetRange(hItem,0,0xff);
  SLIDER_SetValue(hItem,0);
  SLIDER_SetWidth(hItem,5);
  SLIDER_SetNumTicks(hItem,0);
  //
  // Initialization of 'Volume'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1);
  SLIDER_SetRange(hItem,1,63);
  SLIDER_SetValue(hItem,Music.ucVolume);
	//
	// Initialization of 'Play'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
	BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmplay,10,10);
	BUTTON_SetText(hItem,"");
	//
	// Initialization of 'Back'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
	BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmback,10,10);
	BUTTON_SetText(hItem,"");
	//
	// Initialization of 'Next'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2);
	BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmnext,10,10);
	BUTTON_SetText(hItem,"");	
  //
	// Initialization of 'volume'
	//
	hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON3);
	BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmvolume_medium,5,5);
	BUTTON_SetText(hItem,"");
  //
  // Initialization of 'musiclist'
  //
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTBOX0);
  LISTBOX_SetMulti(hItem,0);
	LISTBOX_SetFont(hItem, &FONT_XINSONGTI_25);
	LISTBOX_SetAutoScrollV(hItem,1);       
  LISTBOX_SetItemSpacing(hItem,5);	
  LISTBOX_SetTextAlign(hItem,GUI_TA_LEFT|GUI_TA_VCENTER);

  // USER START (Optionally insert additional code for further widget initialization)
  file_num=0;
  for(i=0;i<MUSIC_MAX_NUM;i++)
  {
    for(j=0;j<FILE_NAME_LEN;j++)
      playlist[i][j]=0;
    for(j=0;j<MUSIC_NAME_LEN;j++)
      lcdlist[i][j]=0;
  }
  scan_files(path);
  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTBOX0);
  if(file_num)
	{
    for(i=0;i<file_num;i++)
    {
      for(j=0,k=0;(j<51)&&lcdlist[i][j]!='\0';k++)
      {
        if(lcdlist[i][j]>0x80)
        {
          gbkdata=lcdlist[i][j+1]+lcdlist[i][j]*256;
          UCname[k]=ff_convert(gbkdata,1);
          //printf("%d:%04X\n",j,UCname[k]);
          j+=2;
        }
        else
        {
          UCname[k]=0x00ff&lcdlist[i][j];
          //printf("%d:%04X\n",j,UCname[k]);
          j+=1;
        }
      }
      UCname[k]='\0';
      //printf("%d.%s\n",i+1,lcdlist[i]);			
      GUI_UC_ConvertUC2UTF8(UCname,MUSIC_NAME_LEN,music_name,FILE_NAME_LEN);
      LISTBOX_AddString(hItem,music_name);
    }
  }
  else
  {
    LISTBOX_AddString(hItem,"no music file");
    LISTBOX_SetSel(hItem,-1);
  }
  
    // USER END
  break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_LISTBOX0: // Notifications sent by 'musiclist'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
				if(Music.ucStatus == STA_IDLE)
				{
					Music.ucStatus = STA_PLAYING;
					BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmstop,10,10);
				}
				else
					Music.ucStatus = STA_SWITCH;
					play_index=LISTBOX_GetSel(WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTBOX0));
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_SLIDER0: // Notifications sent by 'schedule'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
			  //�����������������ʱֹͣ���½�����
				clksch=1;
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)        
        //���Ž��������û��޸ĵı�־
        chgsch=1;
				clksch=0;
			// USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)        
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON0: // Notifications sent by 'play'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
		    if(Music.ucStatus != STA_PLAYING)
		    {
			    BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmstop,10,10);
			    Music.ucStatus = STA_PLAYING;
          I2S_Play_Start();
		    }
		    else if(Music.ucStatus == STA_PLAYING)
		    {
		    	BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmplay,10,10);
		    	Music.ucStatus = STA_STOP;
          I2S_Play_Stop();
		    }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_SLIDER1: // Notifications sent by 'volume'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)       
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1);
		    Music.ucVolume = SLIDER_GetValue(hItem);
        sprintf(tmp,"%2d%%",Music.ucVolume*100/63);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT6),tmp);

        hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON3);
        if(Music.ucVolume==1)
        {
          wm8978_OutMute(1);
          Music.ucMute=1;
          BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmvolume_mute,5,5);
        }
        else
        {
          wm8978_OutMute(0);
          Music.ucMute=0;
          BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmvolume_medium,5,5);
          /* ����������������ͬ���� */
          wm8978_SetOUT1Volume(Music.ucVolume);
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON1: // Notifications sent by 'back'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTBOX0);
        if(Music.ucStatus != STA_IDLE) 
        {
          Music.ucStatus = STA_SWITCH;
        }
        play_index--;
        if(play_index>file_num)play_index=file_num-1;
        LISTBOX_SetSel(hItem,play_index);
        //printf("paly_index=%d\n",play_index);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON2: // Notifications sent by 'next'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_LISTBOX0);
        if(Music.ucStatus != STA_IDLE) 
        {
          Music.ucStatus = STA_SWITCH;
        }
        play_index++;
        if(play_index>=file_num)play_index=0;
        LISTBOX_SetSel(hItem,play_index);
        //printf("paly_index=%d\n",play_index);      
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON3: // Notifications sent by 'volume'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON3);
		    if(Music.ucMute==0)
		    {
			    BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmvolume_mute,5,5);
          Music.ucMute=1;
		    }
		    else
		    {          
		    	BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmvolume_medium,5,5);
          Music.ucMute=0;
		    }
        wm8978_OutMute(Music.ucMute);
        //printf("paly_index=%d\n",play_index);      
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  case WM_PAINT:		
  	 GUI_SetBkColor(APPBKCOLOR);
		 GUI_Clear();
	
	  GUI_SetColor(GUI_YELLOW);	
	  GUI_DrawRect(5,35,480-15,530);
			
	 	GUI_SetColor(GUI_YELLOW);
    GUI_DrawRect(5,540,480-15,800);	

		break;
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}



/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       Createmusic
*/
void FUN_ICON1Clicked(void) 
{
  uint16_t i=0;
  WM_HWIN hWin;
  char str[MUSIC_NAME_LEN]={0};
  char music_name[FILE_NAME_LEN]={0};
	char name_showlcd[MUSIC_NAME_LEN]={0};
  APP_TRACE_DBG(("MusicDLG create\n"));
  Music.ucVolume = 35;		/* ȱʡ���� */
  Music.ucMute=0;
    
	/* �����ļ�ϵͳ -- SD�� */
  f_result = f_mount(&fs[0],"0:",1);	/* Mount a logical drive */
	if(f_result!=FR_OK)
    bsp_result |= BSP_SD;
  else 
    bsp_result &=~BSP_SD;
    
  /* ���WM8978оƬ���˺������Զ�����CPU��GPIO */
  if (wm8978_Init()==0)
    bsp_result|=BSP_WM8978;
  else
    bsp_result&=~BSP_WM8978;  
  
  hWin = GUI_CreateDialogBox(_aDialogCreateMusic, GUI_COUNTOF(_aDialogCreateMusic), _cbDialogMusic, WM_HBKWIN, 0, 0);
  
  /* ����һ���ڴ�ռ� ���ҽ������� */
  hMEM_Music = GUI_ALLOC_AllocZero(BUFFER_SIZE*5+sizeof(LYRIC));
  /* �����뵽�ڴ�ľ��ת����ָ������ */
  pMEM_Music = GUI_ALLOC_h2p(hMEM_Music);	
	
	hCOMMemory = GUI_ALLOC_AllocZero(COMDATA_SIZE);
  /* �����뵽�ڴ�ľ��ת����ָ������ */
  comdata = GUI_ALLOC_h2p(hCOMMemory);

#if LRC_GBK	
	hCOMUTF8Memory = GUI_ALLOC_AllocZero(COMDATA_UTF8_SIZE);
  /* �����뵽�ڴ�ľ��ת����ָ������ */
  comdataUTF8 = GUI_ALLOC_h2p(hCOMUTF8Memory);
#endif
	
  if(pMEM_Music==NULL)
  {
    bsp_result|=BSP_ALLOC;
    buffer_Music[0]=NULL;
    buffer_Music[1]=NULL;
    
  }
  else
  {
    buffer_Music[0]=(uint16_t *)pMEM_Music;
    buffer_Music[1]=(uint16_t *)(pMEM_Music+BUFFER_SIZE*2);
    lrc=(LYRIC *)(pMEM_Music+BUFFER_SIZE*5);
    bsp_result&=~BSP_ALLOC;
  }
  
  Music.ucStatus = STA_IDLE;		/* ����״̬ */	
  
  if((bsp_result&BSP_SD)||(bsp_result&BSP_WM8978))
  {		
    GUI_Delay(100);
    if(bsp_result&BSP_WM8978)
      sprintf(str,"The WM8978 drive cannot work!");  
    else if(bsp_result&BSP_SD)
      sprintf(str,UTF8_SDCARDERROR); 
    else
      sprintf(str,"one drive cannot work!");
    ErrorDialog(hWin,"Music Error",str);
    while(1)
    {
       if(tpad_flag)WM_DeleteWindow(hWin);
       if(!UserApp_Running)return;
       GUI_Delay(10);
    }
  }
  
  while(UserApp_Running)
  {
    if(Music.ucStatus==STA_PLAYING)
		{      
      LISTBOX_SetSel(WM_GetDialogItem(hWin, GUI_ID_LISTBOX0),play_index);
      i=0;
      while(lcdlist[play_index][i]!='\0')
      {
        name_showlcd[i]=lcdlist[play_index][i];
        i++;
      }
      name_showlcd[i]='\0';				
      com_gbk2utf8(name_showlcd,str);
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3),str);
      i=0;
      while(playlist[play_index][i]!='\0')
      {
        music_name[i]=playlist[play_index][i];
        i++;
      }				
      music_name[i]='\0';
      lrc_chg_suffix((uint8_t *)music_name,"lrc");
      com_data2null(comdata,COMDATA_SIZE);
      i=0;
      while(i<LYRIC_MAX_SIZE)
      {
        lrc->addr_tbl[i]=0;
        lrc->length_tbl[i]=0;
        lrc->time_tbl[i]=0;
        i++;
      }
      lrc->indexsize=0;
			lrc->oldtime=0;
			lrc->curtime=0;
      f_result=f_open(&f_file,music_name,FA_READ);
      if((f_result==FR_OK)&&(f_file.fsize<COMDATA_SIZE))
      {					
        f_result=f_read(&f_file,comdata,f_file.fsize,&f_num);		
        if(f_result==FR_OK) 
        {
          OS_ERR      err;
          OSSchedLock(&err);
          lyric_analyze(lrc,comdata);
          lrc_sequence(lrc);
          OSSchedUnlock(&err);            
        }
      }
			f_close(&f_file);	      
      i=0;
			while(playlist[play_index][i]!='\0')
			{
				music_name[i]=playlist[play_index][i];
				i++;
			}
			music_name[i]='\0';
      if(strstr(music_name,".wav")||strstr(music_name,".WAV"))
      {
        buffer_Music[0]=(uint16_t *)pMEM_Music;
        buffer_Music[1]=(uint16_t *)(pMEM_Music+BUFFER_SIZE*2);
        lrc=(LYRIC *)(pMEM_Music+BUFFER_SIZE*5);
        wavplayer(music_name,hWin);
      }
      else
      {
        buffer_Music[0]=(uint16_t *)pMEM_Music;
        buffer_Music[1]=(uint16_t *)(pMEM_Music+BUFFER_SIZE);
        inputbuf=(uint8_t *)(pMEM_Music+BUFFER_SIZE*2);
        lrc=(LYRIC *)(pMEM_Music+BUFFER_SIZE*5);
        mp3player(music_name,hWin);
      }
    }
    else if(Music.ucStatus==STA_SWITCH)
    {	
      GUI_Delay(100);
      Music.ucStatus = STA_PLAYING;
      BUTTON_SetBitmapEx(WM_GetDialogItem(hWin, GUI_ID_BUTTON0),BUTTON_BI_UNPRESSED,&bmstop,10,10);
    }
    if(tpad_flag)WM_DeleteWindow(hWin);
    GUI_Delay(10);
  }
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
