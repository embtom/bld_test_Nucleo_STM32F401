/* ****************************************************************************************************
 * lib_isr_config.h configuration of the ISR abstraction
 *	
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/


#ifndef LIB_ISR_CONFIG_H
#define LIB_ISR_CONFIG_H

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

#endif /* LIB_ISR_CONFIG_H */
