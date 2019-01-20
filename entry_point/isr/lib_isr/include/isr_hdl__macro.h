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

#ifndef _LIB_ISR__MACRO_H_
#define _LIB_ISR__MACRO_H_


#ifdef __cplusplus
extern "C" {
#endif

/* *******************************************************************
 * defines
 * ******************************************************************/

// internal macro to create the actual entry
#define DESC_ENTRY(__isr_number, __isr_id, __preempt_prio, __sub_prio)	\
{	.isr_number			= __isr_number,		/*isr_number*/			\
	.isr_id				= __isr_id,			/*isr_id*/				\
	.preempt_priority	= __preempt_prio,	/*preempt_priority*/	\
	.sub_priority		= __sub_prio,		/*sub_priority*/		\
	.isr_handler		= NULL,				/*isr_handler*/			\
	.isr_arg            = NULL				/*isr_arg*/				\
}


#ifdef __cplusplus
}
#endif

#endif /* _LIB_ISR__MACRO_H_ */
