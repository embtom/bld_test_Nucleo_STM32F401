/* ****************************************************************************************************
 * lib_console_init_itf.h within the following project: bld_device_cmake_Nucleo_STM32F401_
 *	
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		thomas
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	Aug 26, 2018			thomas			- creation of lib_console_init_itf.h
 *  
 */



#ifndef INT_IO_TTY_LIB_CONSOLE_SH_LIB_CONSOLE_EOS_LIB_CONSOLE_INIT_ITF_H_
#define INT_IO_TTY_LIB_CONSOLE_SH_LIB_CONSOLE_EOS_LIB_CONSOLE_INIT_ITF_H_

/* *******************************************************************
 * includes
 * ******************************************************************/

/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_LIB_CONSOLE__CFG_PORT(_port, _baudrate) 	\
{													\
	.port = _port, 									\
	.baudrate = _baudrate							\
}

#define M_LIB_CONSOLE_INIT__CFG_MAP(__cfg_map_var, __cfg_map_table)			\
	static struct lib_console_cfg __attribute__ ((section(".text"))) __cfg_map_var[] = __cfg_map_table;

#define M_LIB_CONSOLE_INIT__CFG_CNT(__cfg_map_var)		\
	sizeof(__cfg_map_var)/sizeof(*__cfg_map_var)

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
struct lib_console_cfg {
	void *port;
	unsigned int baudrate;
};



#endif /* INT_IO_TTY_LIB_CONSOLE_SH_LIB_CONSOLE_EOS_LIB_CONSOLE_INIT_ITF_H_ */
