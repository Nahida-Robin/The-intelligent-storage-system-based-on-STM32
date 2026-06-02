/** 
 * @file Display.c
 * @brief 通过调用OLED接口显示状态，方便调试
 * @author Nahida
 * @date 2026.5.26
 */

#include "stm32f4xx.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include "stm32f4xx_hal.h"
#include "Password.h"

#define PASSWORD_MAX_LENGTH 8

extern uint8_t Temph;
extern uint8_t Templ;
extern uint8_t Humih;
extern uint8_t Humil;
extern uint8_t Succeses;
extern uint8_t Vertify_State;
extern volatile uint8_t Userword[PASSWORD_MAX_LENGTH];
extern volatile uint8_t NewPassWord[PASSWORD_MAX_LENGTH];
extern volatile uint8_t Password[PASSWORD_MAX_LENGTH];
extern uint8_t Userword_Index;
extern uint8_t Password_Index;
extern uint8_t Password_Length;
extern uint8_t Read_Flag;
extern uint8_t View_Flag;
extern uint8_t View_Pages;
extern uint8_t View_Index;

uint8_t UW_Memory[16][9] = {0};
uint8_t TM_Memory[16][9] = {0};
uint8_t UW_LEN = 0;
uint8_t TM_LEN = 0;


/**
  *@brief 通过OLED实时显示锁的状态
  *@param state 1：开 0：关
  *@retval NULL
  */
void Display_ShowLockState(void)
{
	if(Vertify_State)
	{
		OLED_ShowString(1, 1, "State:Open ");
	}
	else
	{
		OLED_ShowString(1, 1, "State:Close");
	}
}

/**
  *@brief 通过OLED实时显示开锁次数
  *@param freq 次数
  *@retval NULL
  */
void Display_ShowLockFreq(void)
{
	OLED_ShowString(2, 1, "Freq:");
	OLED_ShowString(2, 8, "     ");
	OLED_ShowNum(2, 6, Succeses, 2);
}

/**
  *@brief 通过OLED显示主页面
  *@param NULL
  *@retval NULL
  */
void Display_ShowMainMenu(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "State:");
	OLED_ShowString(2, 1, "Freq:");
	OLED_ShowString(3, 1, "Tem:");
	OLED_ShowString(4, 1, "Hum:");
}

/**
  *@brief 通过OLED刷新温湿度数据
  *@param NULL
  *@retval NULL
  */
void Display_ShowDHT(void)
{
	OLED_ShowNum(3, 5, Temph, 2);
	OLED_ShowNum(4, 5, Humih, 2);
}

/**
  *@brief 通过OLED显示密码正确
  *@param NULL
  *@retval NULL
  */
void Display_ShowOK(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Vertify OK");
	HAL_Delay(300);
}

/**
  *@brief 通过OLED显示输入密码界面UI
  *@param NULL
  *@retval NULL
  */
void Display_InputInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Please Press");
	OLED_ShowString(3, 1, "11 To Confirm");
	OLED_ShowString(4, 1, "14 To Delete");
	
}

/**
  *@brief 通过OLED实时显示输入的密码
  *@param NULL
  *@retval NULL
  */
void Display_ShowInput(void)
{
	OLED_ShowNum(2, Userword_Index, Userword[Userword_Index - 1], 1);
	OLED_ShowString(2, Userword_Index + 1, "     ");
}

/**
  *@brief 通过OLED显示开锁后的UI
  *@param NULL
  *@retval NULL
  */
void Display_ShowOpen(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Already Open");
	HAL_Delay(500);
	OLED_Clear();
	OLED_ShowString(1, 1, "13 To Select");
	OLED_ShowString(2, 1, "12 To Close");
	OLED_ShowString(3, 1, "14 To Change");
	OLED_ShowString(4, 1, "15 To View");
}

/**
  *@brief 通过OLED显示更改密码界面的UI
  *@param NULL
  *@retval NULL
  */
void Display_ChaInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Press New PWD");
	OLED_ShowString(3, 1, "11 To Confirm");
	OLED_ShowString(4, 1, "14 To Delete");
}

/**
  *@brief 通过OLED实时显示更改的密码
  *@param NULL
  *@retval NULL
  */
void Display_ShowChaPwd(void)
{
	OLED_ShowNum(2, Password_Index, NewPassWord[Password_Index - 1], 1);
	OLED_ShowString(2, Password_Index + 1, "   ");
}

/**
  *@brief 通过OLED显示引导view的界面
  *@param NULL
  *@retval NULL
  */
void Display_ViewInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Password recs");
}

/**
  *@brief 读取历史密码
  *@param NULL
  *@retval -1 历史记录为空
  *         1 成功
  */
int8_t Display_ReadView(void)
{
	if(Read_Flag){return -1;}
	Read_Flag = 1;
	uint8_t Uw_Mem[256] = {0};
	uint8_t Tm_Mem[64] = {0};
	uint16_t Uw_Len = 0;
	uint16_t Tm_Len = 0;
	uint8_t Uw_Line = 1;
	uint8_t Tm_Line = 1;
	if(Read_Memory(Uw_Mem, 256, &Uw_Len, Tm_Mem, 64, &Tm_Len) == 1)
	{
		UW_LEN = Uw_Len;
		TM_LEN = Tm_Len;
		uint8_t Uw_Index = 0, Tm_Index = 0;
		if(Uw_Len == 0 || Tm_Len == 0)
		{
			OLED_Clear();
			OLED_ShowString(1, 1, "No Records");
			HAL_Delay(300);
			return -1;
		}
		View_Pages = Tm_Len / 4 + 1;
		while(Uw_Index < Uw_Len && Tm_Index < Tm_Len && Uw_Line <= 4)
		{
			while(Uw_Index < Uw_Len && Uw_Mem[Uw_Index] != 'A')
			{
				UW_Memory[Uw_Line][Uw_Index] = Uw_Mem[Uw_Index];
				Uw_Index++;
			}
			if(Uw_Index < Uw_Len && Uw_Mem[Uw_Index] == 'A')
			{
				UW_Memory[Uw_Line++][Uw_Index] = 'A';
				Uw_Index = 0;
			}
			while(Tm_Index < Tm_Len && Tm_Mem[Tm_Index] != 'Z')
			{
				TM_Memory[Tm_Line][Tm_Index] = Tm_Mem[Tm_Index];
				Tm_Index++;
			}
			if(Tm_Index < Tm_Len && Tm_Mem[Tm_Index] == 'Z')
			{
				TM_Memory[Tm_Line++][Tm_Index] = 'Z';
				Tm_Index = 0; 
			}
		}
	}
	else
	{
		OLED_ShowString(1, 1, "                 ");
		OLED_ShowString(1, 1, "Read Error");
	}
	return 1;
}

/**
  *@brief 通过OLED显示历史密码记录 可翻页
  *@param NULL
  *@retval NULL
  */
void Display_ShowHistory(void)
{
	if(View_Flag)return;
	View_Flag = 1;
	OLED_Clear();
	OLED_ShowString(1, 1, "Record: Page:");
	OLED_ShowNum(1, 14, View_Index, 1);
	uint16_t Uw_Index = 0, Tm_Index = 0;
	uint8_t U_Len = 0, T_Len = 0;
	uint8_t UShow_Index = 0, TShow_Index = 0;
	uint8_t Line = 2;
	Uw_Index = (View_Index - 1) * 3;
	Tm_Index = (View_Index - 1) * 3;	
	U_Len = View_Index * 3;	
	T_Len = View_Index * 3;	
	while(Uw_Index < U_Len && Tm_Index < T_Len)
	{
		uint8_t U_Col = 1, T_Col = 10;
		while(Uw_Index < U_Len && UW_Memory[Uw_Index][UShow_Index] != 'A')
		{
			OLED_ShowNum(Line, U_Col++, UW_Memory[Uw_Index][UShow_Index], 1);
			UShow_Index++;
		}
		if(Uw_Index < Line && UW_Memory[Uw_Index][UShow_Index] == 'A')
		{
			Uw_Index++;
		}
		while(Tm_Index < T_Len && TM_Memory[Tm_Index][TShow_Index] != 'Z')
		{
			OLED_ShowNum(Line, T_Col, TM_Memory[Tm_Index][TShow_Index], 2);
			T_Col += 2;
			TShow_Index++;
		}
		if(Tm_Index < T_Len && TM_Memory[Tm_Index][TShow_Index] == 'Z')
		{
			Tm_Index++;
		}
		Line++;
	}
}

/**
  *@brief 通过OLED显示引导密码选择的函数
  *@param NULL
  *@retval NULL
  */
void Display_SelInt(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "14 To Selexct");
}

/**
  *@brief 通过OLED显示密码选择UI 
  *@param NULL
  *@retval NULL
  */
void Display_Showsel(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Now Password");
	OLED_ShowString(3, 1, "11 To Confirm");
	OLED_ShowString(4, 1, "13 To Resel");
	uint8_t PW_Index = 0;
	while(PW_Index < Password_Length)
	{
		PW_Index++;
		OLED_ShowNum(2, PW_Index, Password[PW_Index - 1], 1);
	}
}

/**
  *@brief 通过OLED显示密码选择失败
  *@param NULL
  *@retval NULL
  */
void Display_ShowSelErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Sel Err");
}

/**
  *@brief 通过OLED显示长度错误
  *@param NULL
  *@retval NULL
  */
void Display_ShowLenErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Len Err");
	HAL_Delay(300);
}

/**
  *@brief 通过OLED显示密码错误
  *@param NULL
  *@retval NULL
  */
void Display_ShowPwdErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Pwd Err");
	HAL_Delay(300);
}

/**
  *@brief 通过OLED 显示存储密码错误
  *@param NULL
  *@retval NULL
  */
void Display_ShowMemErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Mem Err");
	HAL_Delay(300);
}

/**
  *@brief 通过OLED显示更改密码成功
  *@param NULL
  *@retval NULL
  */
void Display_ShowChaOK(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Change OK");
	HAL_Delay(300);
}

/**
  *@brief 通过OLED显示长度错误
  *@param NULL
  *@retval NULL
  */
void Display_ShowChaErr(void)
{
	OLED_Clear();
	OLED_ShowString(1, 1, "Change Err");
	OLED_ShowString(2, 1, "Len Err");
	HAL_Delay(300);
}
