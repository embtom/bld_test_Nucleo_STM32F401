/*
 * isr_hdl__types.h
 *
 *  Created on: 08.03.2016
 *      Author: thomas
 */

#ifndef SH_LIB_ISR_ISR_HDL__TYPES_H_
#define SH_LIB_ISR_ISR_HDL__TYPES_H_

#ifdef STM32HAL_F1
	#include <stm32f1xx.h>
#elif STM32HAL_F4
	#include <stm32f4xx.h>
#else
	#error No Architecture is set at lib_isr
#endif


#endif /* SH_LIB_ISR_ISR_HDL__TYPES_H_ */
