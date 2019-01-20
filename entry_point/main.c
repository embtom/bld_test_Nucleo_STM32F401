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

/* c-runtime */
#include <stdint.h>
#include <string.h>

/* system */
#include <FreeRTOS.h>
#include <task.h>

/* frame */
#include <lib_thread.h>
#include <lib_clock.h>
#include <lib_log.h>
#include <lib_ttyportmux.h>

#include <lib_convention__errno.h>


#include <lib_serial.h>

#include <app_test.h>
/* project */
#include "boardConfig.h"
#include "main.h"

/* *******************************************************************
 * defines
 * ******************************************************************/
#define TEST_MSG "Hello TEST OUT\r\n"

/* *******************************************************************
 * Static Function Prototypes
 * ******************************************************************/
static void Error_Handler(void);
static void* bld_device__main_thread(void* _arg);

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static char s_buffer[20] = {0};
static uint32_t s_timestamp;
extern lib_serial_hdl s_serial_hdl;

/* *******************************************************************
 * Global Functions
 * *****************************d*************************************/
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
  ret = boardConfig_InitSystemClocks();
	if (ret < EOK) {
    Error_Handler();
  }

  boardConfig_InitPins();

	clock = HAL_RCC_GetSysClockFreq();
	clock = HAL_RCC_GetHCLKFreq();
	clock = HAL_RCC_GetPCLK1Freq();
	clock = HAL_RCC_GetPCLK2Freq();

  ret = boardConfig__InitDrivers();
	if (ret < EOK) {
    Error_Handler();
  }

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
	size_t len;
  char test_inBuffer[20] = {0};

	s_timestamp = lib_clock__get_time_ms();

	ret = app_test__init();
	if (ret < EOK) {
 		return NULL;
	}

	while (1) 
  {
		lib_thread__msleep(5000);
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


