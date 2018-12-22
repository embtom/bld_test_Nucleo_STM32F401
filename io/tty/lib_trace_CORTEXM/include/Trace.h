/* ****************************************************************************************************
 * Trace.h
 *
 * The trace device is an independent output channel, intended for debug purposes.
 *
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	07 Juli 2016	Tom				- creation of Trace.h
 *
 */

#ifndef DIAG_TRACE_H_
#define DIAG_TRACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* *******************************************************************
 * includes
 * ******************************************************************/
#include <stdarg.h>
#include <unistd.h>


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
typedef struct trace_swo_hdl_attr *trace_swo_hdl_t;

/* *******************************************************************
 * function declarations
 * ******************************************************************/

int lib_trace_swo__open(trace_swo_hdl_t *_hdl, unsigned int _device_num);
int lib_trace_swo__prinf(trace_swo_hdl_t _hdl, const char * const _format, ...);
int lib_trace_swo__vprintf(trace_swo_hdl_t _hdl, const char * const _format, va_list _ap);
int lib_trace_swo__putchar(trace_swo_hdl_t _hdl, char _c);

#ifdef __cplusplus
}
#endif


#endif // DIAG_TRACE_H_
