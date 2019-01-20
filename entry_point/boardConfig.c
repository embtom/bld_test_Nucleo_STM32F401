/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2019 Thomas Willetal 
 * (https://github.com/tom3333)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */ 

/* *******************************************************************
 * includes
 * ******************************************************************/

/* frame */
#include <lib_convention__errno.h>
#include <lib_timer.h>
#include <lib_timer_init_STM32.h>
#include <lib_serial.h>
#include <lib_serial_stm32.h>
#include <lib_console.h>
#include <lib_console_factory.h>
#include <lib_clock.h>
#include <lib_ttyportmux.h>

#include "stm32f4xx_hal.h"

/* project */
#include "boardConfig.h"

/* *******************************************************************
 * defines
 * ******************************************************************/
#define USART_TX_Pin GPIO_PIN_2
#define USART_RX_Pin GPIO_PIN_3

#define M_LIB_TIMER__CFG_MAP {							\
	M_LIB_TIMER__CFG_TIM_ISR(TIM1, TIM1_UP_TIM10_IRQn), \
	M_LIB_TIMER__CFG_TIM(TIM2),							\
	M_LIB_TIMER__CFG_TIM(TIM3),							\
	M_LIB_TIMER__CFG_TIM(TIM5)							\
}

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
M_LIB_TIMER_INIT__CFG_MAP(s_timer_cfg_map, M_LIB_TIMER__CFG_MAP);
lib_serial_hdl s_serial_hdl;
console_hdl_t s_console_hdl;

static struct ttyStreamMap s_streamMapping[TTYSTREAM_CNT] = {
    M_STREAM_MAPPING_ENTRY(TTYDEVICE_console),  /* TTY_STREAM_critical */\
    M_STREAM_MAPPING_ENTRY(TTYDEVICE_console),  /* TTY_STREAM_error */\
    M_STREAM_MAPPING_ENTRY(TTYDEVICE_console),  /* TTY_STREAM_warning */\
    M_STREAM_MAPPING_ENTRY(TTYDEVICE_console),  /* TTY_STREAM_info */\
    M_STREAM_MAPPING_ENTRY(TTYDEVICE_console),  /* TTY_STREAM_info */\
    M_STREAM_MAPPING_ENTRY(TTYDEVICE_console),  /*TTY_STREAM_CONTROL*/
};


/* *********************************************d**********************
 * global functions definition
 * ******************************************************************/
/* ************************************************************************//**
 * \brief	System Clock Configuration
 *
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSI)
 *            SYSCLK(Hz)                     = 84000000
 *            HCLK(Hz)                       = 84000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSI Frequency(Hz)              = 16000000
 *            PLL_M                          = 16
 *            PLL_N                          = 336
 *            PLL_P                          = 4
 *            PLL_Q                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale2 mode
 *            Flash Latency(WS)              = 2
 * \return void
 * ****************************************************************************/
int boardConfig_InitSystemClocks(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef rccperiphclkinit = {0};

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();
  
    /*  The voltage scaling allows optimizing the power consumption when the device is 
        clocked below the maximum system frequency, to update the voltage scaling value 
        regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /* Enable HSI Oscillator and activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 0x10;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        return -EHAL_ERROR;
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
    clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        return -EHAL_ERROR;
    }
    return EOK;  
}

/* ************************************************************************//**
 * \brief	Initialization of the board PIN config
 * \return	EOK, if successful, ret< EOK if not successful
 * ****************************************************************************/
void boardConfig_InitPins()
{ 
    __HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = USART_TX_Pin;
    //GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USART_RX_Pin;
    //GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}   

/* ************************************************************************//**
 * \brief	Initialization of the device drivers 
 * \return	EOK, if successful, ret< EOK if not successful
 * ****************************************************************************/
int boardConfig__InitDrivers()
{
    int ret;
    lib_serial_hdl uart2Hdl;
    struct port_config uart2Init[] = {STM32_PORT_CONFIG_DMA(USART2, DMA1 ,Stream6, DMA_CHANNEL_4, Stream5,DMA_CHANNEL_4) };

    lib_clock__init();
	lib_timer__init(&s_timer_cfg_map[0],M_LIB_TIMER_INIT__CFG_CNT(s_timer_cfg_map));
  
    s_serial_hdl = lib_serial_create_stm32(&uart2Init[0]);
    if (s_serial_hdl == NULL) {
        return -ESTD_FAULT;
    }
    //ret |= lib_serial_open(s_serial_hdl,BAUDRATE_115200,DATA_FORMAT_8_NONE_1);
    s_console_hdl = lib_console_factory__getInstance(s_serial_hdl);
    if (s_serial_hdl == NULL) {
        return -ESTD_FAULT;
    }
    
    ret = lib_console__open(s_console_hdl, BAUDRATE_115200, DATA_FORMAT_8_NONE_1);
    if (ret < EOK) {
        return -ESTD_FAULT;
    }
    
    lib_ttyportmux__init(&s_streamMapping[0], sizeof(s_streamMapping));


    return ret;
}