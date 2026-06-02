/** 
 * @file TaskDriver.c
 * @brief 驱动层，调用各个功能函数 所有UI界面均可按16返回上一级
 * @author Nahida
 * @date 2026.5.25
 */

#include "stm32f4xx.h"                  // Device header

#include "Matrix.h"
#include "Password.h"
#include "NowTime.h" 
#include "OLED.h"
#include "Delay.h"
#include "dht11.h"
#include "Serial.h"
#include "Display.h"
#include "StepMotor.h"

#define TempAlarm 25

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;
extern uint8_t Vertify_State;
extern uint8_t Succeses;
extern uint8_t Uart_Rx_Byte;
uint8_t Temph = 0;
uint8_t Templ = 0;
uint8_t Humih = 0;
uint8_t Humil = 0;
volatile uint8_t Sensor_Read_Flag = 0;
uint8_t Read_Flag = 0;
uint8_t View_Flag = 0;
uint8_t View_Pages = 1;
uint8_t View_Index = 1;

typedef enum{
	STATE_IDLE = 0,
	STATE_VERTIFY,
	STATE_OPEN,
	STATE_CHAPWD,
	STATE_VIEW,
	STATE_SEL
}State_t;

State_t State = STATE_IDLE;

/**
  *@brief 任务驱动初始化，初始化各功能
  *@param NULL
  *@retval NULL
  */
void TaskDriver_Init()
{
	DWT_Delay_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	Now_Time_Init();
	Serial_Init();
	OLED_Init();
	HAL_Delay(20);
	Display_ShowMainMenu();
}

/**
  *@brief 开锁并记录成功次数
  *@param NULL
  *@retval NULL
  */
void Lock_Open(void)
{
	StepMotor_Forward_90_Degree(1);
	Succeses++;
	
}

/**
  *@brief 关索并清除开锁状态标志位
  *@param NULL
  *@retval NULL
  */
void Lock_Close(void)
{
	StepMotor_Forward_90_Degree(-1);
	Vertify_State = 0;
}

/**
  *@brief 温度报警检测
  *@param NULL
  *@retval NULL
  */
void TemAlarm_Check(void)
{
	if(Temph > TempAlarm)
	{
		printf("TempAlarm");
	}
}

/**
  *@brief 检测错误次数
  *@param NULL
  *@retval NULL
  */
void Err_Check(void)
{
	uint8_t Err = Open_Error();
	if(Err == 3)
	{
		printf("Normal Alarm");
	}
	else if(Err == 4)
	{
		printf("Special Alarm");
	}
}

/**
  *@brief 空闲任务函数 刷新各个数据 等待按键按下进入密码输入
  *@param NULL
  *@retval NULL
  */
void IDLE_Task(void)
{
	Display_ShowLockFreq();
	Display_ShowLockState();
	Display_ShowDHT();
	uint8_t Key = Matrix_GetNum();
	
	if(Key == 11)
	{
		State = STATE_VERTIFY;
		Display_InputInt();
		return;
	}
}

/**
  *@brief 密码验证函数 调用password接口 按返回值进行不同处理
  *@param NULL
  *@retval NULL
  */
void VERTIFY_Task(void)
{
	int8_t Result = Password_Vertify();
	
	switch(Result)
	{
		case 1:
			Display_ShowInput();
			break;
		case 2:
//			Lock_Open();
			Vertify_State = 1;
		  Display_ShowOK();
		  State = STATE_OPEN;
		  Display_ShowOpen();
		  return;
		case  6:
			State = STATE_IDLE;
			Display_ShowMainMenu();
			break;
		case -1:
			Display_ShowLenErr();
		  Display_InputInt();
			break;
		case -2:
			Err_Check();
		  Display_ShowPwdErr();
		  Display_InputInt();
			break;
		case -3:
			Display_ShowMemErr();
		  Display_InputInt();
			break;
	}
	
//	if(Result == 1)
//	{
//		Display_ShowInput();
//	}
//	else if(Result == 2)
//	{
//		Lock_Open();
//		Display_ShowOK();
//		State = STATE_OPEN;
//		Display_ShowOpen();
//		return;
//	}
//	else if(Result == -1)
//	{
//		Display_ShowLenErr();
//		Display_InputInt();
//	}
//	else if(Result == -2)
//	{
//		Err_Check();
//		Display_ShowPwdErr();
//		Display_InputInt();
//	}
//	else if(Result == -3)
//	{
//		Display_ShowMemErr();
//		Display_InputInt();
//	}
}

/**
  *@brief 开锁状态函数 等待按键按下 进入空闲 更改密码 查看历史记录
  *@param NULL
  *@retval NULL
  */
void OPEN_Task()
{
	uint8_t Key = Matrix_GetNum();
	
	switch(Key)
	{
		case 12:
//			Lock_Close();
			Vertify_State = 0;
		  State = STATE_IDLE; 	
		  Display_ShowMainMenu();
			break;
		case 13:
			State = STATE_SEL;
		  Display_SelInt();
			break;
		case 14:
			State = STATE_CHAPWD;
		  Display_ChaInt();
			break;
		case 15:
			State = STATE_VIEW;	
			Display_ViewInt();
			break;
		case 16:
			State = STATE_VERTIFY;
//			Lock_Close();
			Vertify_State = 0;
			Display_InputInt();
			break;
	}
	
//	if(Key == 12)
//	{
//		Lock_Close();
//		State = STATE_IDLE;
//		Display_ShowMainMenu();
//	}
//	else if(Key == 13)
//	{
//		State = STATE_SEL;
//		Display_SelInt();
//	}
//	else if(Key == 14)
//	{
//		State = STATE_CHAPWD;
//		Display_ChaInt();
//	}
//	else if(Key == 15)
//	{
//		State = STATE_VIEW;
//		Display_ViewInt();
//	}
}

/**
  *@brief 更改密码函数 调用password接口 根据返回值判断更改成功与否
  *@param NULL
  *@retval NULL
  */
void CHAPWD_Task()
{
	int8_t Result = Password_Change();
	
	switch(Result)
	{
		case 1:
			Display_ShowChaPwd();
		  return;
		case 5:
			Display_ShowChaOK();
		  State = STATE_OPEN;
		  Display_ShowOpen();
		  break;
		case -1:
			Display_ShowChaErr();
		  State = STATE_OPEN;
		  Display_ShowOpen();
		  break;
		case 6:
			State = STATE_OPEN;
		  Display_ShowOpen();
		  break;
	}
	
//	if(Result == 1)
//	{
//		Display_ShowChaPwd();
//		return;
//	}
//	else if(Result == 5)
//	{
//		Display_ShowChaOK();
//		State = STATE_OPEN;
//		Display_ShowOpen();
//	}
//	else if(Result == -1)
//	{
//		Display_ShowChaErr();
//		State = STATE_OPEN;
//		Display_ShowOpen();
//	}
//	else if(Result == 6)
//	{
//		State = STATE_OPEN;
//		Display_ShowOpen();
//	}
}

/**
  *@brief 查看历史记录函数 显示历史密码输入和时间
  *@param NULL
  *@retval NULL
  */
void VIEW_Task()
{
	int8_t Result = Display_ReadView();
	Display_ShowHistory();
	uint8_t Key = Matrix_GetNum();
	switch(Key)
	{
		case 11:
			if(View_Pages >= 1 && View_Index < View_Pages)
			{
				View_Index++;
				View_Flag = 0;
				Display_ShowHistory();
			}
			break;
		case 12:
			if(View_Pages >= 1 && View_Index > 1)
			{
				View_Index--;
				View_Flag = 0;
				Display_ShowHistory();
			}
			break;
		case 16:
			Read_Flag = 0;
		  State = STATE_OPEN;
		  Display_ShowOpen();
			break;
	}
}


/**
  *@brief 密码选择函数
  *@param NULL
  *@retval NULL
  */
void PWSEL_Task(void)
{
	int8_t Result = Password_Select();
	switch(Result)
	{
		case 1:
			Display_Showsel();
			break;
		case 7:
			State = STATE_OPEN;
		  Display_ShowOpen();
			break;
		case 8:
			Display_SelInt();
		  return;
		case 6:
			State = STATE_OPEN;	
		  Display_ShowOpen();
			break;
	}
//	if(Result == 1)
//	{
//		Display_Showsel();
//	}
//	else if(Result == 8)
//	{
//		State = STATE_OPEN;
//		Display_ShowOpen();
//	}
//	else if(Result == 9)
//	{
//		Display_SelInt();
//		return;
//	}
//	else if(Result == 6)
//	{
//		State = STATE_OPEN;
//		Display_ShowOpen();
//	}
	
}

/**
  *@brief TIM2定时器中断回调 每2s溢出中断一次
  *@param htim 定时器句柄
  *@retval NULL
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    Sensor_Read_Flag = 1;
  }
}

/**
  *@brief 串口接收中断回调
  *@param huart 串口句柄
  *@retval NULL
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        Serial_RXCallBack(Uart_Rx_Byte);
        HAL_UART_Receive_IT(&huart1, &Uart_Rx_Byte, 1);
    }
}


/**
  *@brief 任务驱动运行函数 主函数里循环执行此函数
  *@param NULL
  *@retval NULL
  */
void TaskDriver_Run(void)
{
//	if(Sensor_Read_Flag)
//	{
//		Sensor_Read_Flag = 0;
//		DHT_Read(&Temph, &Templ, &Humih, &Humil);
//	}
	
	Serial_ProcessCmd();
	
	switch(State)
	{
		case STATE_IDLE: IDLE_Task();break;
		case STATE_VERTIFY: VERTIFY_Task();break;
		case STATE_OPEN: OPEN_Task();break;
		case STATE_CHAPWD: CHAPWD_Task();break;
		case STATE_VIEW: VIEW_Task();break;
		case STATE_SEL: PWSEL_Task();break;
	}
	
}


