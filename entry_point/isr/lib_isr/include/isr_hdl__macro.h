/* ****************************************************************************************************
 * lib_isr_pool.h within the following project: lib_isr
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:
 * ****************************************************************************************************/

/* ****************************************************************************************************/

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
