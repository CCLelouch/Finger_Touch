/**
  ******************************************************************************
  * &file    loop.c
  * &author  cc
  * &version V1.0.0
  * &date    2018-04-06
  * &brief   
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */
	
#include "head.h"

void Init(void);
void Initial_Run(void);
void loop(void);

int main()
{
	Init();
	Initial_Run();
	loop();	
	return 0;
}


/***************************************** USER CODE **********************************************/
void Init(void)//硬件资源初始化
{ 
  GPIO_Config(RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_5,GPIO_Speed_50MHz, GPIO_Mode_Out_PP);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
  

  
  USART_Config();
  DMA_UART_Config();
  Key_GPIO_Config();
  Delay_Init();
  NVIC_Config(TIM2_IRQn,1,3);
	TIM2_Init(199,7199);
}
void LedFlash(void)
{
	PBOut(5)=0;
	Delay_usms(500 ms);	
	PBOut(5)=1;
	Delay_usms(500 ms);	
}
void Initial_Run(void)//软件初始化函数
{
  printf("good\r\n");
}
uint16_t n=0;
uint8_t buf[13];
void loop(void)				//循环主函数
{	 
	LedFlash();
	PS_StaGPIO_Init();	//初始化FR读状态引脚
  

    
  FingerPrintConnect();
	while(1)
	{
		FingerProcess();
	} 	
}

void TIM2_IRQHandler(void)
{
//  printf("good\r\n");
  if(TIM_GetITStatus(TIM2,TIM_IT_Update) !=RESET )
	{
    Key_Scan(Scan);
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
/***************************************** USER CODE END ******************************************/


/******************************************************************************/

/**************************** BY CC ******* END OF FILE ***********************/

