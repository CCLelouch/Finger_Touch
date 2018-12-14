/**
  ******************************************************************************
  * &file    bsp_key.c
  * &author  CC
	* &previous versions V2.0.0
  * &version V3.1.0
  * &date    2018-11-14
  * &brief   
  ******************************************************************************
  * &attention
  *	ʹ��ǰ����#define 	KEY16
	*					��#define		KEY4
	* Key_ReadStr.ReadFlag == 1 && Key_ReadStr.Active.Press == ShortPress ����ͬʱ����
	* ʵ���˶��������Լ�4*4������̣��Լ���Ч��ƽ�Ŀ���
  ******************************************************************************
  */ 
#ifndef _FINGERPRINT_H_
#define _FINGERPRINT_H_
#include "GPIO.H"
#include "as608.h"
#include "delay.h" 
#include "bsp_key.h" 

typedef enum
{
  recognize,
  entry,
  clear
}FingerStaType;
extern FingerStaType FingerSta;

uint16_t GetKeyValue(void);
void Add_FR(void);
void Del_FR(void);
void FingerPrintConnect(void);
void ShowErrMessage(u8 ensure);
void press_FR(void);

void FingerProcess(void);






#endif /*_FINGERPRINT_H_*/ 











