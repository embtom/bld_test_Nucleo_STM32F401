/* ****************************************************************************************************
 * Trace.c within the following project: dev_swo_STM32F1
 *	
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		thomas
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	10.05.2016			thomas			- creation of Trace.c
 *  
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
