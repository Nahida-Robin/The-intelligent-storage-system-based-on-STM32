/** 
 * @file Serial.c
 * @brief 串口功能
 * @author Nahida
 * @date 2026.5.26
 */
 
#include "stm32f4xx.h"                  // Device header
#include "Password.h"
#include "TaskDriver.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart1;
extern uint8_t Vertify_State;
extern uint8_t Temph;
extern uint8_t Templ;
extern uint8_t Humih;
extern uint8_t Humil;
extern uint8_t Warnings;
extern uint8_t Succeses;
volatile uint8_t Rx_Cmd_Ready = 0;
uint8_t Uart_Rx_Byte = 0;
uint8_t Uart_Rx_Buf[64];
uint8_t Uart_Rx_Index = 0;

/**
  *@brief 串口重定向
  *@param NULL
  *@retval ch 字节
  */
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,HAL_MAX_DELAY);
	return ch;	
}

/**
  *@brief 串口初始化 
  *@param NULL
  *@retval NULL
  */
void Serial_Init(void)
{
	HAL_UART_Receive_IT(&huart1, &Uart_Rx_Byte, 1);
}

/**
  *@brief 串口接收回调
  *@param byte 接收到的字节
  *@retval NULL
  */
void Serial_RXCallBack(uint8_t byte)
{
	if(Rx_Cmd_Ready == 1){return;}
	
	if(byte == '\n' || byte == '\r')
	{
		if(Uart_Rx_Index > 0)
		{
			Rx_Cmd_Ready = 1;
			Uart_Rx_Buf[Uart_Rx_Index] = '\0';
			Uart_Rx_Index = 0;
		}
	}
	else if(Uart_Rx_Index < 64 -1)
	{
		Uart_Rx_Buf[Uart_Rx_Index++] = byte;
	}
}	

/**
  *@brief 串口打印状态信息
  *@param NULL
  *@retval NULL
  */
void Serial_ShowStatus(void)
{
	printf("[Status]Lock:%s Temp:%d.%d Humi:%d.%d Unlockcnt:%d Warnings:%d", 
					Vertify_State ? "Open" : "Close", 
					Temph, Templ, Humih, Humil, Succeses, Warnings);
}

/**
  *@brief 串口打印温湿度信息
  *@param NULL
  *@retval NULL
  */
void Serial_ShowDHT(void)
{
	printf("[DHT]Temp:%d.%d Humi:%d.%d", Temph, Templ, Humih, Humil);
}

/**
  *@brief 串口打印开关信息
  *@param NULL
  *@retval NULL
  */
void Serial_ShowLock(void)
{
	printf("[Lock]Lock:%s", Vertify_State ? "Open" : "Close");
}

/**
  *@brief 串口打印开锁次数
  *@param NULL
  *@retval NULL
  */
void Serial_ShowSuccess(void)
{
	printf("[Success]Unlockcnt:%d", Succeses);
}

/**
  *@brief 串口打印错误次数
  *@param NULL
  *@retval NULL
  */
void Serial_ShowWarning(void)
{
	printf("[Warning]Warnings:%d", Warnings);
}

/**
  *@brief 串口打印历史密码记录
  *@param NULL
  *@retval NULL
  */
void Serial_ShowRecord(void)
{
	uint8_t Uw_Mem[256] = {0};
	uint8_t Tm_Mem[64] = {0};
	uint16_t Uw_Len = 0;
	uint16_t Tm_Len = 0;
	if(Read_Memory(Uw_Mem, 256, &Uw_Len, Tm_Mem, 64, &Tm_Len) == 1)
	{
		uint16_t Uw_Index, Tm_Index = 0;
		uint8_t Rec_Num = 0;
		if(Uw_Len == 0 || Tm_Len == 0)
		{
			printf("[Error]No Records");
		}
		printf("[Records]\r\n");
		while(Uw_Index < Uw_Len && Tm_Index < Tm_Len)
		{
			uint8_t Hour = Tm_Mem[Tm_Index++];
			uint8_t Min = Tm_Mem[Tm_Index++];
			uint8_t Sec = Tm_Mem[Tm_Index++];
			if(Tm_Index < Tm_Len && Tm_Mem[Tm_Len] == 'Z'){Tm_Index++;}
			printf("Rec%d Time:%02d:%02d:%02d Pwd:", ++Rec_Num, Hour, Min, Sec);
			
			while(Uw_Index < Uw_Len && Uw_Mem[Uw_Index] != 'A')
			{
				printf("%d", Uw_Mem[Uw_Index++]);
			}
			if(Uw_Index < Uw_Len && Uw_Mem[Uw_Index] == 'A'){Uw_Index++;}
			printf("\r\n");
		}
	}
	else
	{
		printf("[Error]Read Error");
	}
	printf("Records END\r\n");
}

/**
  *@brief 串口关索
  *@param NULL
  *@retval NULL
  */
void Serial_LockClose(void)
{
	if(Vertify_State == 1)
	{
		extern void Lock_Close(void);
		Lock_Close();
		printf("[Lock]Lock Close");
	}
	else
	{
		printf("[Lock]Already Close");
	}
}

/**
  *@brief 串口开锁
  *@param NULL
  *@retval NULL
  */
void Serial_LockOpen(void)
{
	if(Vertify_State == 0)
	{
		extern void Lock_Open(void);
		Lock_Open();
		printf("[Lock]Lock Open");
	}
	else
	{
		printf("[Lock]Already Open");
	}
}

/**
  *@brief 串口打印帮助信息
  *@param NULL
  *@retval NULL
  */
void Serial_ShowHelp(void)
{
	printf("[HELP]Commands: STATUS, ");
}

/**
  *@brief 串口接收命令处理信息
  *@param NULL
  *@retval NULL
  */
void Serial_ProcessCmd(void)
{
	if(Rx_Cmd_Ready == 0){return;}
	Rx_Cmd_Ready = 0;
	
	if(strcmp((const char*)Uart_Rx_Buf, "STATUS") == 0)
	{
		Serial_ShowStatus();
	}
	else if(strcmp((const char*)Uart_Rx_Buf, "DHT") == 0)
	{
		Serial_ShowDHT();
	}
	else if(strcmp((const char*)Uart_Rx_Buf, "OPEN") == 0)
	{
		Serial_LockOpen();
	}
	else if(strcmp((const char*)Uart_Rx_Buf, "CLOSE") == 0)
	{
		Serial_LockClose();
	}
	else if(strcmp((const char*)Uart_Rx_Buf, "SUCCESS") == 0)
	{
		Serial_ShowSuccess();
	}
	else if(strcmp((const char*)Uart_Rx_Buf, "WARNING") == 0)
	{
		Serial_ShowWarning();
  }
	else if(strcmp((const char*)Uart_Rx_Buf, "RECORD") == 0)
	{
		Serial_ShowRecord();
  }
	else if(strcmp((const char*)Uart_Rx_Buf, "HELP") == 0)
	{
		Serial_ShowHelp();
  }
	else
	{
		printf("[Err]Invaild Command Send HELP To Get Commands");
	}
}
