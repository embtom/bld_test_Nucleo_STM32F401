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
#include <stdlib.h>

/* system */
#include <stdint.h>
#include <errno.h>
#include <lib_convention__errno.h>

/* own libs */

/* project */
#include "lib_isr.h"
#include "isr_hdl.h"


/* *******************************************************************
 * GLOBAL FUNCTION DEFINITIONS
 * ******************************************************************/

/* ************************************************************************//**
 * \brief  Request to register a ISR handler
 * ---------
 * \remark The lib dependent ISR routines are abstracted via a
 * 		   "lib_isr" layer. If a device driver is using a interrupt
 * 			source, the handler have to be registered at the abstraction layer.
 * 			Note: Only a cleaned "lib_isr" handler can be registered
 * 				 	   again.
 * ---------
 * \param   _device      : device to register
 * \param	_handler[in] : ISR event handler to register
 * \param   _arg	[in] : pointer to handler routine argument (can be NULL if not required)
 * ---------
 * \return	EOK if successful
 * 			EPAR_NULL   NULL pointer check for the isr_handler
 * 			EINVAL 	  	Requested device to register a handler is not valid
 * 			EAGAIN  	Requested device to register is already in use
 * ****************************************************************************/
int lib_isr__attach (lib_isr_hdl_t *_hdl, IRQn_Type _isr_number, isr_handler_t *_handler,  void *_arg)
{
	struct isr_descriptor *descriptor;

	if (_handler == NULL)
		return -EPAR_NULL;

	descriptor = isr_hdl__request_isr_source(_isr_number);
	if (descriptor == NULL)
	{
		return -ESTD_INVAL;
	}


	if(descriptor->isr_handler != NULL)
	{	/* requested handler is already registered */
		return -ESTD_AGAIN;
	}

	SET_ISR_PRIORITY(descriptor->isr_number, descriptor->preempt_priority, descriptor->sub_priority);
	//=>Everything OK register event handler
	descriptor->isr_arg = _arg;
	descriptor->isr_handler = _handler;

	ENABLE_ISR(descriptor);

	*_hdl = descriptor;
	return EOK;
}

/* ************************************************************************//**
 * \brief  Cleanup a registered ISR handler
 * ---------
 * \remark The lib dependent ISR routines are abstracted via a
 * 		   "lib_isr" layer. If a device driver is no longer using
 * 			a interrupt source, the registered handler have to be reseted at
 * 			the abstraction layer.
 * ---------
 * \param   _device      : device to cleanup
 * ---------
 * \return	EOK if successful
 * 			EINVAL 	  	Requested device to cleanup is not valid
 *
 * ****************************************************************************/
int lib_isr__detach (lib_isr_hdl_t *_hdl)
{
	struct isr_descriptor *descriptor = *_hdl;
	DISABLE_ISR(descriptor);

	//=>Everything OK isr resource was found and can be unloaded
	descriptor->isr_arg = NULL;
	descriptor->isr_handler = NULL;

	return EOK;
}

