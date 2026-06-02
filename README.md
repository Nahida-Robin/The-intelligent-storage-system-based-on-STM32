# STM32F407VET6 嵌入式项目

基于 **STM32CubeMX** 生成的 **STM32F407VET6** 嵌入式项目，使用 **Keil MDK-ARM** 开发环境与 **HAL 库**。

## 项目概述

这是一个 STM32 裸机嵌入式项目，实现了以下功能模块：
老一辈艺术家坚持手搓 抵制珍妮机！！（bushi）
- **矩阵键盘扫描** — 通过驱动获取按键输入
- **OLED 显示** — 在 OLED 屏幕上显示信息
- **状态机任务调度** — 简单的状态机任务驱动管理
- **定时器** — TIM2、TIM3 外设配置
- **串口通信** — USART1 串口通信
- **纳西妲万岁** — Long Live Nahida

## 硬件平台

| 项目 | 说明 |
|------|------|
| **MCU** | STM32F407VET6 |
| **封装** | LQFP100 |
| **主频** | 168 MHz（HSE + PLL） |
| **开发环境** | Keil MDK-ARM (MDK-ARM) |
| **配置工具** | STM32CubeMX |
| **HAL 库** | STM32F4xx HAL 驱动 |
| **OLED屏** | 使用0.96寸OLED，1.3寸X需+2，屏幕驱动使用江协科技的 |
| **矩阵键盘** | 行序从下至上，列序从右到左 |

## 项目结构

```
├── Core/                   # 用户核心代码
│   ├── Inc/                # 头文件 (.h)
│   │   ├── main.h
│   │   ├── gpio.h
│   │   ├── tim.h
│   │   ├── usart.h
│   │   ├── Delay.h
│   │   └── TaskDriver.h 
│   └── Src/                # 源文件 (.c)
│       ├── main.c          # 主程序入口
│       ├── gpio.c
│       ├── tim.c
│       ├── usart.c
│       ├── Delay.c         # 延时相关
│       ├── TaskDriver.c    # 任务驱动
│       ├── stm32f4xx_it.c
│       ├── stm32f4xx_hal_msp.c
│       └── system_stm32f4xx.c
├── Drivers/                #驱动库
│   ├── STM32F4xx_HAL_Driver/
│   ├── CMSIS/
|   └── Hardware            #硬件驱动库
|       ├──dht11.c          #DHT11驱动
|       ├──dht11.h
|       ├──Display.c        #二次封装库 用于OLED显示
|       ├──Display.h
|       ├──Matrix.c         #矩阵键盘驱动
|       ├──Matrix.c
|       ├──Nowtime.c        #用于获取系统当前时间
|       ├──Nowtime.h
|       ├──OLED.c           #OLED驱动-江协科技
|       ├──OLED.h
|       ├──OLED_Font.h      #OLED字库
|       ├──Password.c       #密码相关驱动
|       ├──Password.h
|       ├──Serial.c         #串口驱动
|       ├──Serial.h
|       ├──StepMotor.c      #步进电机驱动
|       └──StepMotor.h
├── MDK-ARM/                # Keil 工程文件
├── test.ioc                # STM32CubeMX 配置文件
├── keilkill.bat            # 清理 Keil 生成文件的脚本
└── .mxproject              # CubeMX 项目配置
```

## 快速开始

### 环境要求

- [Keil MDK-ARM](https://www.keil.com/download/) (v5 及以上)
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) (用于修改外设配置)
- 串口调试工具（如 PuTTY、MobaXterm，可选）

### 使用步骤

1. **克隆仓库**
   ```bash
   git clone https://github.com/Nahida-Robin/The-intelligent-storage-system-based-on-STM32.git
   ```

2. **打开工程**
   - 进入 `MDK-ARM/` 目录，双击 `.uvprojx` 文件打开 Keil 工程

3. **编译 & 下载**
   - 在 Keil 中点击 **Build (F7)** 编译
   - 使用调试器（如 J-Link、ST-Link）下载到目标板

4. **修改外设配置**（可选）
   - 使用 STM32CubeMX 打开 `test.ioc` 文件修改配置，重新生成代码

## 功能说明

- **main.c** — 初始化系统时钟（168MHz）、GPIO、定时器、USART，然后进入主循环执行矩阵键盘扫描和 OLED 显示
- **TaskDriver** — 任务驱动模块，管理多任务调度逻辑
- **Delay** — 延时功能模块
- **矩阵键盘 & OLED** — 读取矩阵键盘按键值并在 OLED 上显示

## 清理构建文件

运行根目录下的 `keilkill.bat` 可清理 Keil 生成的目标文件。

```
.\keilkill.bat
```

## 许可证

本项目基于 **MIT 许可证** 开源 — 详见 [LICENSE](LICENSE) 文件。
