/* ****************************************************************************************************
 * lib_console.c for console print out
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	07 Juli 2018	Tom				- creation of lib_console.c
 *
 */

#ifndef _LIB_CONSOLE_H_
#define _LIB_CONSOLE_H_

#ifdef __cplusplus
extern "C" {
#endif


/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdio.h>

/* system */
#include <stdarg.h>

/*project*/
#include "lib_console_init_itf.h"


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
typedef struct console_hdl_attr *console_hdl_t;

/* *******************************************************************
 * function declarations
 * ******************************************************************/
int lib_console__init(struct lib_console_cfg const * const _cfg_map, unsigned int _cfg_cnt);
int lib_console__get_device_number(void);
int lib_console__open(console_hdl_t *_hdl, unsigned int _device_num);
int lib_console__print_debug_message(console_hdl_t _hdl, const char * const _format, ...);
int lib_console__vprint_debug_message(console_hdl_t _hdl, const char * const _format, va_list _ap);
int lib_console__putchar(console_hdl_t _hdl, char _c);
int lib_console__getline(console_hdl_t _hdl, char *_lineptr, size_t *_n);
int lib_console__getdelim(console_hdl_t _hdl, char *_lineptr, size_t *_n, char _delimiter);

#ifdef __cplusplus
}
#endif

#endif /* _LIB_CONSOLE_H_ */
