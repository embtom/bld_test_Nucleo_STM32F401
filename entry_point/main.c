/* ****************************************************************************************************
 * main.c : Main program body of bld_device_cmake_LINUX
 *
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		thomas
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	26.08.2018		thomas			- creation of lib_timer.c
 *
 */

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdint.h>
#include <string.h>

/* system */
#include <FreeRTOS.h>
#include <task.h>

/* frame */
#include <lib_thread.h>
#include <lib_clock.h>
#include <lib_ser.h>
#include <lib_menu.h>
#include <lib_log.h>
#include <lib_tty_portmux.h>

#include <lib_console_init_itf.h>
#include <lib_console.h>
#include <lib_timer.h>
#include <lib_timer_init_STM32.h>

#include <lib_convention__errno.h>
#include <test_lib_thread.h>

#include <app_test.h>
/* project */
#include "main.h"

/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_LIB_TIMER__CFG_MAP {							\
	M_LIB_TIMER__CFG_TIM_ISR(TIM1, TIM1_UP_TIM10_IRQn), \
	M_LIB_TIMER__CFG_TIM(TIM2),							\
	M_LIB_TIMER__CFG_TIM(TIM3),							\
	M_LIB_TIMER__CFG_TIM(TIM5)							\
}

#define M_LIB_CONSOLE__CFG_MAP { 						\
	M_LIB_CONSOLE__CFG_PORT((void*)USART2, 115200)		\
}

#define USART_TX_Pin GPIO_PIN_2
#define USART_RX_Pin GPIO_PIN_3
#define TEST_MSG "Hello TEST OUT\r\n"

/* *******************************************************************
 * Static Function Prototypes
 * ******************************************************************/
static void SystemClock_Config(void);
static void Error_Handler(void);

static void* bld_device__main_thread(void* _arg);


/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static char s_buffer[20] = {0};
static uint32_t s_timestamp;

M_LIB_TIMER_INIT__CFG_MAP(s_timer_cfg_map, M_LIB_TIMER__CFG_MAP);
M_LIB_CONSOLE_INIT__CFG_MAP(s_lib_console_cfg_map, M_LIB_CONSOLE__CFG_MAP);

static struct tty_stream_mapping s_port_stream_mapping[	TTY_STREAM_CNT] = {
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_console),  /* TTY_STREAM_critical */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_console),  /* TTY_STREAM_error */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_console),  /* TTY_STREAM_warning */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_console),  /* TTY_STREAM_info */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_console),  /* TTY_STREAM_info */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_console),  /*TTY_STREAM_CONTROL*/
};

/* *******************************************************************
 * Global Functions
 * ******************************************************************/
int main(void)
{
	int ret;
	uint32_t clock;
	thread_hdl_t main_thd;

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
	HAL_Init();
  
	clock = HAL_RCC_GetSysClockFreq();
	clock = HAL_RCC_GetHCLKFreq();
	clock = HAL_RCC_GetPCLK1Freq();
	clock = HAL_RCC_GetPCLK2Freq();

	/* Configure the System clock to 84 MHz */
	SystemClock_Config();

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

	clock = HAL_RCC_GetSysClockFreq();
	clock = HAL_RCC_GetHCLKFreq();
	clock = HAL_RCC_GetPCLK1Freq();
	clock = HAL_RCC_GetPCLK2Freq();

	lib_clock__init();
	lib_timer__init(&s_timer_cfg_map[0],M_LIB_TIMER_INIT__CFG_CNT(s_timer_cfg_map));
	lib_console__init(&s_lib_console_cfg_map[0],M_LIB_CONSOLE_INIT__CFG_CNT(s_lib_console_cfg_map));

	lib_thread__create(&main_thd, &bld_device__main_thread, (void*)10, +2, "main");
	vTaskStartScheduler();

	/* Infinite loop */
	while (1)
	{
	}
}


/* *******************************************************************
 * static function definitions
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
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef rccperiphclkinit = {0};

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
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
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }  
}

/* ************************************************************************//**
 * \brief Error handler
 *
 *        This function is executed in case of error occurrence.
 * ****************************************************************************/
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

static void* bld_device__main_thread(void* _arg)
{
	char buffer[20] = {0};
	int ret;
	int count = 0;
	ser_hdl_t ser_2_hdl;
	size_t len;


	s_timestamp = lib_clock__get_time_ms();

  lib_tty_portmux__init(&s_port_stream_mapping[0], sizeof(s_port_stream_mapping));
//	ret = lib_tty_portmux__init(0,0);

	ret = app_test__init();
	if (ret < EOK)
		return NULL;

	while (1) {
		count++;
		memset(&buffer[0], 0, sizeof(buffer));

    len = sizeof(buffer);
    lib_thread__msleep(2000);
		// ret = lib_console__getline(&buffer[0], &len);
		// if (ret == EOK) {
		// 	lib_console__print_debug_message("Kontent %s with len %u\n",buffer,len);
		// }
	}
}



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


