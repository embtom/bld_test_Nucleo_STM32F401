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

#ifndef LIB_ISR_CONFIG_H
#define LIB_ISR_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* *******************************************************************
 * includes
 * ******************************************************************/

#include <isr_hdl__macro.h>

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
enum isr_id
{
	ISR_ID_tim1_up,
	ISR_ID_tim1_trg_com,
	ISR_ID_tim1_cc,
	ISR_ID_tim2,
	ISR_ID_tim3,
	ISR_ID_tim4,
	ISR_ID_uart2,
	ISR_ID_dma_1_stream6,
	ISR_ID_dma_1_stream5,
	ISR_ID_ext1,
	ISR_ID_CNT
};
/* *******************************************************************
 * defines
 * ******************************************************************/
#define LIB_ISR_CONFIG_TABLE \
{				\
	DESC_ENTRY(TIM1_UP_TIM10_IRQn,ISR_ID_tim1_up, 6,0), 			\
	DESC_ENTRY(TIM1_TRG_COM_TIM11_IRQn,ISR_ID_tim1_trg_com, 6,0),	\
	DESC_ENTRY(TIM1_CC_IRQn,ISR_ID_tim1_cc, 6 ,0),					\
	DESC_ENTRY(TIM2_IRQn, ISR_ID_tim2, 2, 0),						\
	DESC_ENTRY(TIM3_IRQn, ISR_ID_tim3, 2, 0),						\
	DESC_ENTRY(TIM4_IRQn, ISR_ID_tim4, 2, 0),						\
	DESC_ENTRY(USART2_IRQn, ISR_ID_uart2, 6, 0),					\
	DESC_ENTRY(DMA1_Stream6_IRQn, ISR_ID_dma_1_stream6, 6, 0),		\
	DESC_ENTRY(DMA1_Stream5_IRQn, ISR_ID_dma_1_stream5, 6, 0),		\
	DESC_ENTRY(EXTI0_IRQn, ISR_ID_ext1, 6, 0)						\
}

#ifdef __cplusplus
}
#endif

#endif /* LIB_ISR_CONFIG_H */
