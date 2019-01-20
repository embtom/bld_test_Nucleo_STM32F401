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

#ifndef _LIB_ISR_POOL_H_
#define _LIB_ISR_POOL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdint.h>

/* system */
#ifdef STM32HAL_F1
	#include <stm32f1xx.h>
	#include <stm32f1xx_hal_cortex.h>
#elif STM32HAL_F4
	#include <stm32f4xx.h>
	#include <stm32f4xx_hal_cortex.h>
#else
	#error No Architecture is set at lib_clock
#endif

/* own libs */

/* project */
#include "lib_isr.h"
#include "lib_isr_config.h"

/* *******************************************************************
 * defines
 * ******************************************************************/
#define SET_ISR_PRIORITY(_isr_number, _preempt_priority, sub_priority)  HAL_NVIC_SetPriority(_isr_number, _preempt_priority, sub_priority);
#define ENABLE_ISR(_descriptor)		{ HAL_NVIC_EnableIRQ((*_descriptor).isr_number); }
#define DISABLE_ISR(_descriptor)	{ HAL_NVIC_DisableIRQ((*_descriptor).isr_number); }

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

struct isr_descriptor
{
	const IRQn_Type isr_number;
	const enum isr_id isr_id;
	const uint32_t preempt_priority;
	const uint32_t sub_priority;
	isr_handler_t* isr_handler;			// pointer to attached ISR handler function
	void *isr_arg;						// storage variable for generic ISR handler argument
};

/* *******************************************************************
 * GLOBAL FUNCTION DECLARATIONS
 * ******************************************************************/

/* ************************************************************************//**
 * \brief Requests the ISR handler memory location to the corresponding isr_device
 * \remark	It returns the isr_device corresponding memory location,
 * 				used to store the address of the attached ISR routine.
 *
 * \param   _device     : hardware _device
 * \return	Address to the isr_handler memory location
 * 			NULL on error (errno set)>
 * 				 		   ENODEV    Requested memory location not available
 * ****************************************************************************/
struct isr_descriptor* isr_hdl__request_isr_source(IRQn_Type _isr_number);

/* ************************************************************************//**
 * \brief  Processing of the registered Interrupt Service Routine (ISR)
 * \remark The ISR processing routine is responsible to call the corresponding
 * 				registered ISR routine. The connection between ISR and the
 * 				ISR routine to attach is specified at the "LIB_ISR_CONFIG_TABLE".
 *
 * ****************************************************************************/
void isr_hdl__isr_processing(enum isr_id _isr_id, unsigned int _vector);


#ifdef __cplusplus
}
#endif

#endif /* _LIB_ISR_POOL_H_ */
