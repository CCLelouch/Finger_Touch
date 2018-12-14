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



SysPara AS608Para;//指纹模块AS608参数
u16 ValidN;//模块内有效模板个数

u8 ensure;
FingerStaType FingerSta;
void LedFlash(void);

//显示确认码错误信息
void ShowErrMessage(u8 ensure)
{
	printf((char *)EnsureMessage(ensure));
}

void FingerPrintConnect(void)
{
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
    printf("HandShakeing\r\n");
	}
	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure!=0x00)
	ensure=PS_ReadSysPara(&AS608Para);  //读AS608模块参数 
	if(ensure==0x00)
	{
		printf("RemainNum:%d    Level:%d",AS608Para.PS_max-ValidN,AS608Para.PS_level);//显示剩余指纹数量和安全等级
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
//录指纹
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

				printf("Pleas touch finger!\r\n");//请按手指
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//生成特征
					if(ensure==0x00)
					{

						printf("Fingerprint correct\r\n");//指纹正确
						i=0;
						processnum=1;//跳到第二步						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;

				printf("Pleas touch finger again!\r\n");//再按一次手指
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//生成特征			
					if(ensure==0x00)
					{

						printf("Fingerprint correct\r\n");//指纹正确
						i=0;
						processnum=2;//跳到第三步
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:

				printf("Compare twice fingerprint\r\n");//对比两次指纹
				ensure=PS_Match();
				if(ensure==0x00) 
				{

					printf("Twice fingerprint are same\r\n");//两次指纹是一样的
					processnum=3;//跳到第四步
				}
				else 
				{

					printf("Compare fail,pleas touch again!\r\n");//对比失败，请重新按手指
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//跳回第一步		
				}
				Delay_usms(1000 ms);
				break;

			case 3:

				printf("Generate fingerprint template\r\n");//产生一个指纹模板
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{

					printf("Generate fingerprint success\r\n");//生成指纹模板成功
					processnum=4;//跳到第五步
				}else {processnum=0;ShowErrMessage(ensure);}
				Delay_usms(1000 ms);
				break;
				
			case 4:	

				printf("Intput ID and save with ENTER!\r\n");//输入ID并按“Enter”保存
				printf("0=< ID <=299");	
        
				do
					ID= GetKeyValue();
				while(!(ID<300));//输入DI必须小于300
				ensure=PS_StoreChar(CharBuffer2,ID);//储存模板/*5==ID*/
				if(ensure==0x00) 
				{			
					printf("Add fingerprint success!!!\r\n");//添加指纹成功
					PS_ValidTempleteNum(&ValidN);//读库指纹个数
					printf("%d",AS608Para.PS_max-ValidN);//显示剩余指纹个数
					 Delay_usms(1500 ms);//延时后清除显示	

					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		Delay_usms(800 ms);
		if(i==5)//超过5次没有按手指则退出
		{

			break;	
		}				
	}
}


//刷指纹
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;

	ensure=PS_GetImage();
	if(ensure==0x00)/*00表示指令执行完毕（获取图像成功）*/
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//搜索成功
			{				
				printf("Search fingerprint success\r\n");//搜索指纹成功				
				printf("Match ID:%d  Match score:%d\r\n",seach.pageID,seach.mathscore);//显示匹配指纹的ID和分数
		
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

//删除指纹
void Del_FR(void)
{
	u8  ensure;
//	u16 ID;

	printf("Delete fingerprint");//显示删除指纹
	printf("Input ID and touch Enter!");//显示输入ID并按下“Enter”
	printf("0=< ID <=299");
	Delay_usms(50 ms);
//  do
//    ID= GetKeyValue();
//  while(!(ID<300));//输入DI必须小于300
//	if(ID==0xFFFF)

//	else if(ID==0xFF00)
		ensure=PS_Empty();//清空指纹库
//	else 
//		ensure=PS_DeletChar(ID,1);//删除单个指纹
	if(ensure==0)
	{

		printf("Delete fingerprint success!!!");//删除指纹成功		
	}
  else
		ShowErrMessage(ensure);	
  Delay_usms(1500 ms);//延时后清除显示
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
	printf("%d",AS608Para.PS_max-ValidN);//显示剩余指纹个数
}


void FingerProcess(void)
{
  switch(FingerSta)
  {
    case recognize:
      if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
      {	
          press_FR();//刷指纹			
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
      if(687 == GetKeyValue())/*687才可以改密码*/
      {  
        Add_FR();		//录指纹	        
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








