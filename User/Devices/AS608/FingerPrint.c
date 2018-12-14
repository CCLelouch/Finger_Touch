/**
  ******************************************************************************
  * &file    FingerPrint.c
  * &author  CC
	* &previous versions V0.0.0
  * &version V1.0.0
  * &date    2018-11-14
  * &brief   
  ******************************************************************************
  * &attention
  *
	*			
	*
	* 
  ******************************************************************************
  */
#include "FingerPrint.h"
#include "math.h"



SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чģ�����

u8 ensure;
FingerStaType FingerSta;
void LedFlash(void);

//��ʾȷ���������Ϣ
void ShowErrMessage(u8 ensure)
{
	printf((char *)EnsureMessage(ensure));
}

void FingerPrintConnect(void)
{
	while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
    printf("HandShakeing\r\n");
	}
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
	ensure=PS_ReadSysPara(&AS608Para);  //��AS608ģ����� 
	if(ensure==0x00)
	{
		printf("RemainNum:%d    Level:%d",AS608Para.PS_max-ValidN,AS608Para.PS_level);//��ʾʣ��ָ�������Ͱ�ȫ�ȼ�
	}
  else
    ShowErrMessage(ensure);	
}

uint16_t GetKeyValue(void)
{
  uint16_t value = 0;
  uint8_t index=0;

  while(index<3)
  {
    getKey();
    if(KeyBuffer.Name<Key_4&&KeyBuffer.Name!= Key_Up&&KeyBuffer.ReadFlag == 1)
    {
      value += (uint16_t)KeyBuffer.Name*pow(10,2-index);index++;printf("%d\r\n",value);
    }
    else if(KeyBuffer.Name<Key_8&&KeyBuffer.Name>Key_4&&KeyBuffer.ReadFlag == 1)
    {
      value += ((uint16_t)KeyBuffer.Name-1)*pow(10,2-index);index++;printf("%d\r\n",value);
    }
    else if(KeyBuffer.Name<Key_12&&KeyBuffer.Name>Key_8&&KeyBuffer.ReadFlag == 1)
    {
      value += ((uint16_t)KeyBuffer.Name-2)*pow(10,2-index);index++;printf("%d\r\n",value);
    }
    else if(KeyBuffer.Name==Key_14&&KeyBuffer.ReadFlag == 1)
    {
      value += 0;index++;printf("%d\r\n",value);
    }
    Delay_usms(20 ms);

    if(index==3)
    {
      printf("%d\r\n",value);
      return value;
    }
  }
  if(KeyBuffer.ReadFlag==0)
  {
    return 0xffff;
  }
}
//¼ָ��
void Add_FR(void)
{
	u8 i=0,ensure ,processnum=0;
	u16 ID;
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;

				printf("Pleas touch finger!\r\n");//�밴��ָ
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//��������
					if(ensure==0x00)
					{

						printf("Fingerprint correct\r\n");//ָ����ȷ
						i=0;
						processnum=1;//�����ڶ���						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;

				printf("Pleas touch finger again!\r\n");//�ٰ�һ����ָ
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//��������			
					if(ensure==0x00)
					{

						printf("Fingerprint correct\r\n");//ָ����ȷ
						i=0;
						processnum=2;//����������
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:

				printf("Compare twice fingerprint\r\n");//�Ա�����ָ��
				ensure=PS_Match();
				if(ensure==0x00) 
				{

					printf("Twice fingerprint are same\r\n");//����ָ����һ����
					processnum=3;//�������Ĳ�
				}
				else 
				{

					printf("Compare fail,pleas touch again!\r\n");//�Ա�ʧ�ܣ������°���ָ
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//���ص�һ��		
				}
				Delay_usms(1000 ms);
				break;

			case 3:

				printf("Generate fingerprint template\r\n");//����һ��ָ��ģ��
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{

					printf("Generate fingerprint success\r\n");//����ָ��ģ��ɹ�
					processnum=4;//�������岽
				}else {processnum=0;ShowErrMessage(ensure);}
				Delay_usms(1000 ms);
				break;
				
			case 4:	

				printf("Intput ID and save with ENTER!\r\n");//����ID������Enter������
				printf("0=< ID <=299");	
        
				do
					ID= GetKeyValue();
				while(!(ID<300));//����DI����С��300
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��/*5==ID*/
				if(ensure==0x00) 
				{			
					printf("Add fingerprint success!!!\r\n");//���ָ�Ƴɹ�
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
					printf("%d",AS608Para.PS_max-ValidN);//��ʾʣ��ָ�Ƹ���
					 Delay_usms(1500 ms);//��ʱ�������ʾ	

					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		Delay_usms(800 ms);
		if(i==5)//����5��û�а���ָ���˳�
		{

			break;	
		}				
	}
}


//ˢָ��
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;

	ensure=PS_GetImage();
	if(ensure==0x00)/*00��ʾָ��ִ����ϣ���ȡͼ��ɹ���*/
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				printf("Search fingerprint success\r\n");//����ָ�Ƴɹ�				
				printf("Match ID:%d  Match score:%d\r\n",seach.pageID,seach.mathscore);//��ʾƥ��ָ�Ƶ�ID�ͷ���
		
//        LedFlash();
//        LedFlash();
			}
			else 
			{
				ShowErrMessage(ensure);		
			}
	  }
		else
			ShowErrMessage(ensure);
	}	
}

//ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;
//	u16 ID;

	printf("Delete fingerprint");//��ʾɾ��ָ��
	printf("Input ID and touch Enter!");//��ʾ����ID�����¡�Enter��
	printf("0=< ID <=299");
	Delay_usms(50 ms);
//  do
//    ID= GetKeyValue();
//  while(!(ID<300));//����DI����С��300
//	if(ID==0xFFFF)

//	else if(ID==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
//	else 
//		ensure=PS_DeletChar(ID,1);//ɾ������ָ��
	if(ensure==0)
	{

		printf("Delete fingerprint success!!!");//ɾ��ָ�Ƴɹ�		
	}
  else
		ShowErrMessage(ensure);	
  Delay_usms(1500 ms);//��ʱ�������ʾ
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	printf("%d",AS608Para.PS_max-ValidN);//��ʾʣ��ָ�Ƹ���
}


void FingerProcess(void)
{
  switch(FingerSta)
  {
    case recognize:
      if(PS_Sta)	 //���PS_Sta״̬���������ָ����
      {	
          press_FR();//ˢָ��			
          printf("waite");
      }
      getKey();
      if(KeyBuffer.Name==Key_4&&KeyBuffer.ReadFlag == 1)
      {
        FingerSta = entry; 
      }
      else if(KeyBuffer.Name==Key_13&&KeyBuffer.ReadFlag == 1)
      {
        FingerSta = clear;
      }
      break;
    case entry:
      if(687 == GetKeyValue())/*687�ſ��Ը�����*/
      {  
        Add_FR();		//¼ָ��	        
        printf("entry\r\n");
      } 
      Delay_usms(10);
      FingerSta = recognize;
      break;
    case clear:
      Del_FR();
      FingerSta = recognize;
      break;
  }
}








