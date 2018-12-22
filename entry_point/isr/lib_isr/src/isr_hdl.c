/* ****************************************************************************************************
 * lib_gpio_STM32F1.c within the following project: lib_gpio_STM32F1
 *
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

/* system */

/* frame */
#include <lib_isr.h>

/* project */
#include <lib_isr_config.h>
#include "isr_hdl.h"


/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static struct isr_descriptor isr_descriptors[] = LIB_ISR_CONFIG_TABLE;


/* *******************************************************************
 * GLOBAL FUNCTION DEFINITIONS
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
struct isr_descriptor* isr_hdl__request_isr_source(IRQn_Type _isr_number)
{
	int i;

	for (i = 0; i < sizeof(isr_descriptors)/sizeof(*isr_descriptors); i++)
	{
		if (_isr_number == isr_descriptors[i].isr_number){
			return &isr_descriptors[i];
		}
	}
	return NULL;
}


/* ************************************************************************//**
 * \brief  Processing of the registered Interrupt Service Routine (ISR)
 * \remark The ISR processing routine is responsible to call the corresponding
 * 				registered ISR routine. The connection between ISR and the
 * 				ISR routine to attach is specified at the "LIB_ISR_CONFIG_TABLE".
 *
 * ****************************************************************************/
void isr_hdl__isr_processing(enum isr_id _isr_id, unsigned int _vector)
{
	struct isr_descriptor *descriptor;


	descriptor = &isr_descriptors[_isr_id];


	// Check if platform specific ISR configuration is corresponding with the triggered ISR
	if (descriptor->isr_id != _isr_id)
		return;

//	// Check if an ISR routine is configured
	if(descriptor->isr_handler == NULL)
		return;

	// Call of the registered isr routine
	(*(descriptor->isr_handler))(descriptor->isr_number,_vector, descriptor->isr_arg);
}

