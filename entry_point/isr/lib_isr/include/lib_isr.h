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

#ifndef _LIB_ISR_H_
#define _LIB_ISR_H_

#ifdef __cplusplus
extern "C" {
#endif
/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */

/* system */

/* project */
#include "lib_isr_types.h"

/* *******************************************************************
 * GLOBAL FUNCTION DECLARATIONS
 * ******************************************************************/

/* ************************************************************************//**
 * \brief  Request to register a ISR handler
 * \remark The lib dependent ISR routines are abstracted via a
 * 		   "lib_isr" layer. If a device driver is using a interrupt
 * 			source, the handler have to be registered at the abstraction layer.
 * 			Note: Only a cleaned "lib_isr" handler can be registered again.
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
int lib_isr__attach (lib_isr_hdl_t *_hdl, IRQn_Type _isr_number, isr_handler_t *_handler,  void *_arg);

/* ************************************************************************//**
 * \brief  Cleanup a registered ISR handler
 * \remark The lib dependent ISR routines are abstracted via a
 * 			"lib_isr" layer. If a device driver is no longer using
 * 			 a interrupt source, the registered handler have to be reseted at
 * 			 the abstraction layer.
 * ---------
 * \param   _device      : device to cleanup
 * ---------
 * \return	EOK if successful
 * 			EINVAL 	  	Requested device to cleanup is not valid
 *
 * ****************************************************************************/
int lib_isr__detach (lib_isr_hdl_t *_hdl);

#ifdef __cplusplus
}
#endif

#endif /* _LIB_ISR_H_ */
