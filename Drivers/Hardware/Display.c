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
extern uint8_t View_Flag;


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
  *@brief 通过OLED显示历史密码
  *@param NULL
  *@retval -1 历史记录为空
  *         1 成功
  */
int8_t Display_ShowView(void)
{
	if(View_Flag){return -1;}
	View_Flag = 1;
	uint8_t Uw_Mem[256] = {0};
	uint8_t Tm_Mem[64] = {0};
	uint16_t Uw_Len = 0;
	uint16_t Tm_Len = 0;
	uint8_t Uw_Line = 2, Tm_Line = 2;
	if(Read_Memory(Uw_Mem, 256, &Uw_Len, Tm_Mem, 64, &Tm_Len) == 1)
	{
		uint16_t Uw_Index, Tm_Index = 0;
		uint8_t Tm_Id = 10;
		uint8_t Uw_Id = 1;
		if(Uw_Len == 0 || Tm_Len == 0)
		{
			OLED_Clear();
			OLED_ShowString(1, 1, "No Records");
			HAL_Delay(300);
			return -1;
		}
		while(Uw_Index < Uw_Len && Uw_Mem[Uw_Index] != 'A')
		{
			OLED_ShowNum(Uw_Line, Uw_Id++, Uw_Mem[Uw_Index++], 1);
			if(Tm_Mem[Tm_Index] == 'Z'){Tm_Index++;Tm_Line++;}
			if(Uw_Id == 9){Uw_Id = 1;}
		}
		while(Tm_Index < Tm_Len && Tm_Mem[Tm_Index] != 'Z')
		{
			OLED_ShowNum(Tm_Line, Tm_Id++, Tm_Mem[Tm_Index++], 1);
			if(Uw_Index < Uw_Len && Uw_Mem[Uw_Index] == 'A'){Uw_Index++; Uw_Line++;}
			if(Tm_Id == 16){Tm_Id = 10;}
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
