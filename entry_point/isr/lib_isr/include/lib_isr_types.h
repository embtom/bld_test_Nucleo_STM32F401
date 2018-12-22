/* ****************************************************************************************************
 * lib_isr_types.h within the following project: lib_isr
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:
 * ****************************************************************************************************/

#ifndef _LIB_ISR_TYPES_H_
#define _LIB_ISR_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdint.h>

/* system */

/* own libs */

/* project */
#include "isr_hdl__types.h"

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
typedef struct isr_descriptor *lib_isr_hdl_t;

/* *********************************************************
 * typedef for function pointers on interrupt service
 * routines (ISR)
 * ********************************************************/
typedef void (isr_handler_t)(IRQn_Type _isr_vector, unsigned int _vector, void *_arg);


#ifdef __cplusplus
}
#endif

#endif /* _LIB_ISR_TYPES_H_ */
