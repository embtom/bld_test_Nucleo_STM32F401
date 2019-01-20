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
#include <string.h>

/* frame */
#include <mini-printf.h>
#include <lib_convention__errno.h>
#include <FreeRTOS.h>

/* project */
#include "swo.h"
#include "Trace.h"


/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_TRACE__NUMBER_OF_DEVICE				1
#define OS_INTEGER_TRACE_PRINTF_TMP_ARRAY_SIZE	64


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
struct trace_swo_hdl_attr {
	unsigned int initialized;
	char buf[OS_INTEGER_TRACE_PRINTF_TMP_ARRAY_SIZE];
};


/* *******************************************************************
 * function definition
 * ******************************************************************/
int lib_trace_swo__open(trace_swo_hdl_t *_hdl, unsigned int _device_num)
{
	int ret_val;
	trace_swo_hdl_t hdl;

	if (_hdl == NULL) {
		ret_val = -EPAR_NULL;
		goto ERR_INIT;
	}

	if (_device_num >= M_TRACE__NUMBER_OF_DEVICE) {
		ret_val = -ESTD_NODEV;
		goto ERR_INIT;
	}

	hdl = (trace_swo_hdl_t)pvPortMalloc(sizeof(struct trace_swo_hdl_attr));
	if (hdl == NULL) {
		ret_val = -ESTD_NOMEM;
		goto ERR_INIT;
	}

	memset(&hdl->buf[0],0, sizeof(hdl->buf));
	hdl->initialized = 1;
	*_hdl = hdl;
	return EOK;

	ERR_INIT:
	return ret_val;
}

int lib_trace_swo__prinf(trace_swo_hdl_t _hdl, const char * const _format, ...)
{
	int ret_val;
	va_list ap;

	if (_hdl == NULL) {
		ret_val = -EPAR_NULL;
		goto ERR_INIT;
	}

	if (!_hdl->initialized) {
		ret_val = -EEXEC_NOINIT;
		goto ERR_INIT;
	}

	va_start (ap, _format);
	ret_val = mini_vsnprintf(&_hdl->buf[0],sizeof(_hdl->buf),_format,ap);
	va_end (ap);

	if (ret_val < EOK) {
		goto ERR_VPRINTF;
	}

	SWO_PrintString(&_hdl->buf[0],ret_val);
	return EOK;

	ERR_VPRINTF:
	ERR_INIT:
	return ret_val;

}


int lib_trace_swo__vprintf(trace_swo_hdl_t _hdl, const char * const _format, va_list _ap)
{
	int ret_val;
	va_list ap;

	if (_hdl == NULL) {
		ret_val = -EPAR_NULL;
		goto ERR_INIT;
	}

	if (!_hdl->initialized) {
		ret_val = -EEXEC_NOINIT;
		goto ERR_INIT;
	}

	ret_val = mini_vsnprintf((char*)&_hdl->buf,sizeof(_hdl->buf),_format, _ap);
	if (ret_val < EOK) {
		goto ERR_VPRINTF;
	}

	SWO_PrintString(&_hdl->buf[0],ret_val);
	return EOK;

	ERR_VPRINTF:
	ERR_INIT:
	return ret_val;
}

int lib_trace_swo__putchar(trace_swo_hdl_t _hdl, char _c)
{
	int ret_val;

	if (_hdl == NULL) {
		ret_val = -EPAR_NULL;
		goto ERR_INIT;
	}

	if (!_hdl->initialized) {
		ret_val = -EEXEC_NOINIT;
		goto ERR_INIT;
	}

	SWO_PrintString(&_c,1);
	return EOK;

	ERR_VPRINTF:
	ERR_INIT:
	return ret_val;
}
