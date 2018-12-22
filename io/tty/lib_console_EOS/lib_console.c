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

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/* frame */
#include <lib_convention__errno.h>
#include <lib_convention__macro.h>
#include <lib_ser.h>
#include <lib_thread.h>
#include <mini-printf.h>
#include <FreeRTOS.h>

/* project */
#include "lib_console_init_itf.h"
#include "lib_console.h"


/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_LIB_CONSOLE__TX_BUFFER_SIZE 	200
#define M_LIB_CONSOLE__RX_BUFFER_SIZE	50


/* *******************************************************************
 * Configuration
 * ******************************************************************/
#define M_LIB_CONSOLE__INTER_FRAME_TIMEOUT		2

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
struct console_hdl_attr {
	unsigned int initialized;
	char tx_console_buffer[M_LIB_CONSOLE__TX_BUFFER_SIZE];
	char* rx_console_buffer_addr;
	size_t rx_console_buffer_size;
	unsigned int rx_console_last_pos;
	char rx_console_delimiter;
	unsigned int char_del_active;
	ser_hdl_t ser_console_hdl;
	sem_hdl_t tx_serial_sem;		// semaphore for managing the buffer read access
	sem_hdl_t rx_serial_sem;
	mutex_hdl_t	tx_serial_mtx;
	thread_hdl_t rx_serial_worker_thd;
	sem_hdl_t rx_serial_worker_sem;
	unsigned int rx_serial_running;
	enum receive_event rx_type;
	unsigned int number_received_data;
	unsigned int correction_offset;
};


/* *******************************************************************
 * static data
 * ******************************************************************/
static struct lib_console_cfg *s_lib_console_cfg_map;
static unsigned int s_lib_console_cfg_count;
static unsigned int s_lib_console_initialized = 0;
static volatile unsigned int s_interruptTransmit = 0;

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static int lib_console__send (console_hdl_t _hdl, uint8_t *_data_send, unsigned int _length);
static void lib_console__tx_finished(ser_hdl_t *_ser_hdl);
static void* lib_console__rx_worker(void * _arg);
static enum receive_mode lib_console__rx_finished(ser_hdl_t *_ser_hdl, enum receive_event _type , struct rx_info const * const _info);


/* *******************************************************************
 * Static Inline Function
 * ******************************************************************/
static inline unsigned int lib_console__process_console(console_hdl_t _hdl, char *_buffer, size_t _rcv_length, size_t _already_processed);

static inline int lib_console__setup_getdelim_receiver(console_hdl_t _hdl, char *const _rx_buffer, size_t _rx_buffer_size, char _delimiter)
{
	int ret_val;

	if((_hdl == NULL) || (_rx_buffer == NULL) || (_rx_buffer_size < 1)) {
		return -ESTD_INVAL;
	}

	_hdl->rx_console_buffer_addr = _rx_buffer;
	_hdl->rx_console_buffer_size = _rx_buffer_size;
	_hdl->rx_console_delimiter = _delimiter;
	_hdl->number_received_data = 0;
	_hdl->rx_console_last_pos = 0;
	_hdl->char_del_active = 0;
	_hdl->correction_offset = 0;

	ret_val = lib_ser__prepare_receiver(_hdl->ser_console_hdl, (uint8_t*)_rx_buffer, _rx_buffer_size, M_LIB_CONSOLE__INTER_FRAME_TIMEOUT, lib_console__rx_finished);
	if (ret_val < EOK) {
		return -ESTD_IO;
	}

	return EOK;
}

/* *******************************************************************
 * Global Functions
 * ******************************************************************/

/* ************************************************************************//**
 * \brief	Init of the lib_console component
 *
 *
 * \return	EOK				Success
 *
 * ****************************************************************************/
int lib_console__init(struct lib_console_cfg const * const _cfg_map, unsigned int _cfg_cnt)
{
	int ret, line;

	if ((_cfg_map == NULL) || (_cfg_cnt == 0)) {
		line = __LINE__;
		return -ESTD_INVAL;
	}

	if (s_lib_console_initialized>0) {
		s_lib_console_initialized++;
		return EOK;
	}


	s_lib_console_cfg_map = (struct lib_console_cfg*)_cfg_map;
	s_lib_console_cfg_count = _cfg_cnt;
	s_lib_console_initialized = 1;
	return EOK;

}

int lib_console__get_device_number(void)
{
	if ((s_lib_console_cfg_map == NULL) || (s_lib_console_cfg_count == 0)) {
		return -EEXEC_NOINIT;
	}

	return s_lib_console_cfg_count;
}


/* ************************************************************************//**
 * \brief	Initialization of the lib_console
 *
 * \param   _baudrate   : serial baudrate
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_console__open(console_hdl_t *_hdl, unsigned int _device_num)
{
	int ret_val;
	console_hdl_t hdl;

	if (_hdl == NULL) {
		ret_val = -EPAR_NULL;
		goto ERR_INIT;
	}

	if (_device_num >= s_lib_console_cfg_count) {
		ret_val = -ESTD_NODEV;
		goto ERR_INIT;
	}

	//todo missing reinit check

	hdl = (console_hdl_t)pvPortMalloc(sizeof(struct console_hdl_attr));
	if (hdl == NULL) {
		ret_val = -ESTD_NOMEM;
		goto ERR_INIT;
	}

	memset(&hdl->tx_console_buffer[0],0, sizeof(hdl->tx_console_buffer));

	ret_val = lib_ser__open(&hdl->ser_console_hdl, s_lib_console_cfg_map[_device_num].port, s_lib_console_cfg_map[_device_num].baudrate, DATA_FORMAT_8_NONE_1);
	if(ret_val < EOK) {
		goto ERR_DEV_SER_INIT;
	}

	// initialize read semaphore: the initial number of enqueued elements is 0
	ret_val = lib_thread__sem_init(&hdl->tx_serial_sem, 0);
	if (ret_val < EOK) {
		goto ERR_SEM_TX_INIT;
	}

	ret_val = lib_thread__sem_init(&hdl->rx_serial_sem, 0);
	if (ret_val < EOK) {
		goto ERR_SEM_RX_INIT;
	}

	ret_val = lib_thread__mutex_recursive_init(&hdl->tx_serial_mtx);
	if (ret_val < EOK) {
		goto ERR_MTX_INIT;
	}

	ret_val = lib_thread__sem_init(&hdl->rx_serial_worker_sem,0);
	if (ret_val < EOK) {
		goto ERR_SEM_RX_WORKER;
	}

	ret_val = lib_thread__create(&hdl->rx_serial_worker_thd,&lib_console__rx_worker,(void*)hdl,2,"wrk_console");
	if (ret_val < EOK) {
		goto ERR_RX_THD_CREATE;
	}

	hdl->initialized = 1;
	*_hdl = hdl;
	return EOK;

	ERR_RX_THD_CREATE:
		lib_thread__sem_destroy(&hdl->rx_serial_worker_sem);
	ERR_SEM_RX_WORKER:
		lib_thread__mutex_destroy(&hdl->tx_serial_mtx);
	ERR_MTX_INIT:
		lib_thread__sem_destroy(&hdl->rx_serial_sem);
	ERR_SEM_RX_INIT:
		lib_thread__sem_destroy(&hdl->tx_serial_sem);
	ERR_SEM_TX_INIT:
		lib_thread__join(&hdl->rx_serial_worker_thd, NULL);
//		dev_ser__cleanup(M_DEV_SERIAL_DEVICE);
	ERR_DEV_SER_INIT:

	ERR_INIT:

	return ret_val;
}

/* ************************************************************************//**
 * \brief Printout a message on the serial console
 *
 * \param   _format 		"printf" style formatted string argument
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_console__print_debug_message(console_hdl_t _hdl, const char * const _format, ...)
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

	va_start(ap,_format);
	ret_val = lib_console__vprint_debug_message(_hdl, _format, ap);
	va_end(ap);

	if (ret_val < EOK){
		goto ERR_VPRINTF;
	}
	return EOK;

	ERR_VPRINTF:
	ERR_INIT:
	return ret_val;
}

/* ************************************************************************//**
 *  \brief	Printout a variable argument list message on the serial console
 *
 * \param   _format 		"printf" style formatted string argument
 * \param	_ap				variable argument list
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_console__vprint_debug_message(console_hdl_t _hdl, const char * const _format, va_list _ap)
{
	int ret_val;
	unsigned int length;

	if (_hdl == NULL) {
		ret_val = -EPAR_NULL;
		goto ERR_INIT;
	}

	if (!_hdl->initialized) {
		ret_val = -EEXEC_NOINIT;
		goto ERR_INIT;
	}

	lib_thread__mutex_lock(_hdl->tx_serial_mtx);

	memset(&_hdl->tx_console_buffer[0],0,M_LIB_CONSOLE__TX_BUFFER_SIZE);

	length = mini_vsnprintf(&_hdl->tx_console_buffer[0],M_LIB_CONSOLE__TX_BUFFER_SIZE, (const char*) _format,_ap);
	if(length > M_LIB_CONSOLE__TX_BUFFER_SIZE - 1) {
		lib_thread__mutex_unlock(_hdl->tx_serial_mtx);
		return -ESTD_NOMEM;
	}

	// append "\r" in case of new line
	if(_hdl->tx_console_buffer[length - 1] == '\n'){
		_hdl->tx_console_buffer[length] = '\r';
		length++;
	}

	ret_val = lib_console__send(_hdl,(uint8_t *) &_hdl->tx_console_buffer[0], length);
	if(ret_val < EOK) {
		lib_thread__mutex_unlock(_hdl->tx_serial_mtx);
		return ret_val;
	}
	lib_thread__mutex_unlock(_hdl->tx_serial_mtx);
	return length;

	ERR_VPRINTF:
	ERR_INIT:
	return ret_val;


}

/* ************************************************************************//**
 *  \brief	Printout a character on the serial console
 *
 * \param   _c 		Character to print
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_console__putchar(console_hdl_t _hdl, char _c)
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

	lib_thread__mutex_lock(_hdl->tx_serial_mtx);
	ret_val = lib_ser__send(_hdl->ser_console_hdl,&_c,1, NULL);
	lib_thread__mutex_unlock(_hdl->tx_serial_mtx);
	return ret_val;

	ERR_VPRINTF:
	ERR_INIT:
	return ret_val;
}
/* ************************************************************************//**
 *  \brief	Read of a full console log until the newline is reached
 *
 * \param   _lineptr[OUT]	pointer to storage location
 * \param	_n[IN|OU]		pointer to buffer length
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_console__getline(console_hdl_t _hdl, char *_lineptr, size_t *_n)
{
	return lib_console__getdelim(_hdl, _lineptr,_n,'\r');
}

/* ************************************************************************//**
 *  \brief	 Read string until the delimitaion character is found
 *
 * \param   _lineptr[OUT]	pointer to storage location
 * \param	_n[IN|OU]		pointer to buffer length
 * \param	_delimiter		delimiter character to read line
 * \return	EOK if successful, or negative errno value on error
 *
 * ****************************************************************************/
int lib_console__getdelim(console_hdl_t _hdl, char *_lineptr, size_t *_n, char _delimiter)
{
	int ret_val;
	size_t max_lineptr_size;

	if (_hdl == NULL) {
		ret_val = -EPAR_NULL;
		goto ERR_INIT;
	}

	if (!_hdl->initialized) {
		ret_val = -EEXEC_NOINIT;
		goto ERR_INIT;
	}

	if ((_lineptr == NULL) || (_n == NULL)) {
		ret_val = -ESTD_INVAL;
		goto ERR_INIT;
	}

	max_lineptr_size = *_n;
	ret_val = lib_console__setup_getdelim_receiver(_hdl, _lineptr, max_lineptr_size, _delimiter);
	if (ret_val < EOK) {
		return ret_val;
	}

	ret_val = lib_thread__sem_wait(_hdl->rx_serial_sem);
	if (ret_val < EOK) {
		return ret_val;
	}
	
	*_n = _hdl->number_received_data -1;

	return EOK;

	ERR_VPRINTF:
	ERR_INIT:
	return ret_val;
}

/* *******************************************************************
 * static function definitions
 * ******************************************************************/
static int lib_console__send (console_hdl_t _hdl, uint8_t *_data_send, unsigned int _length)
{
	int ret_val;

	if (_hdl == NULL) {
		ret_val = -EPAR_NULL;
		goto ERR_PAR_CHECK;
	}

	if (!_hdl->initialized) {
		ret_val = -EEXEC_NOINIT;
		goto ERR_PAR_CHECK;
	}

	if(_data_send == NULL) {
		ret_val = -EPAR_NULL;
		goto ERR_PAR_CHECK;
	}

	if (_length == 0)
		return EOK;

	while (s_interruptTransmit) {
		;
	}


	ret_val = lib_thread__mutex_lock(_hdl->tx_serial_mtx);
	switch (ret_val)
	{
		case EOK : break;
		case -EEXEC_INVCXT : 
			ret_val = EOK;
			break;
		default:
		    goto ERR_MTX_LOCK;
	}
	
	ret_val = lib_ser__send(_hdl->ser_console_hdl, _data_send, _length, &lib_console__tx_finished);
	if(ret_val < EOK) {
		goto ERR_SER_SEND;
	}

	ret_val = lib_thread__sem_wait(_hdl->tx_serial_sem);
	switch (ret_val)
	{
		case EOK : break;
		case -EEXEC_INVCXT : 
			s_interruptTransmit = true;
			ret_val = EOK;
			break;
		default:
			break;
	}
	
	lib_thread__mutex_unlock(_hdl->tx_serial_mtx);
	return ret_val;

	ERR_SER_SEND:
	lib_thread__mutex_unlock(_hdl->tx_serial_mtx);

	ERR_MTX_LOCK:
	ERR_PAR_CHECK:
	return ret_val;
}


static void lib_console__tx_finished(ser_hdl_t *_ser_hdl)
{
	console_hdl_t hdl = (console_hdl_t)GET_CONTAINER_OF(_ser_hdl, struct console_hdl_attr, ser_console_hdl);
	if (s_interruptTransmit == 0) {
		lib_thread__sem_post(hdl->tx_serial_sem);
	}
	s_interruptTransmit = 0;
}


static enum receive_mode lib_console__rx_finished(ser_hdl_t *_ser_hdl, enum receive_event _type , struct rx_info const * const _info)
{
	console_hdl_t hdl = (console_hdl_t)GET_CONTAINER_OF(_ser_hdl, struct console_hdl_attr, ser_console_hdl);

    if (hdl->char_del_active) {
		hdl->number_received_data += _info->number_received_data;
		hdl->number_received_data -= 2;
		hdl->correction_offset = hdl->number_received_data -1;
		hdl->char_del_active = 0;
	}
	else {
		hdl->number_received_data = hdl->correction_offset + _info->number_received_data;
	}

	
	hdl->rx_type = _type;
	lib_thread__sem_post(hdl->rx_serial_worker_sem);
	return RECEIVE_MODE_disable;
}

static void* lib_console__rx_worker(void * _arg)
{
	unsigned int found = 0;
	unsigned int delimiter_search_iterator, rx_console_last_pos;
	int ret_val;
	console_hdl_t hdl = (console_hdl_t)_arg;

	hdl->rx_serial_running=1;
	while (hdl->rx_serial_running) {
		ret_val = lib_thread__sem_wait(hdl->rx_serial_worker_sem);
		if (ret_val < EOK) {
			hdl->rx_serial_running = 0;
			break;
		}

	//	if ((_info->number_noise_error || _info->number_framing_error || _info->number_parity_error)) {
	//		/*Invalid frame received */
	//		/* todo verify if a corrupt message is received if the DMA has copied it*/
	//		return RECEIVE_MODE_enable;
	//	}
	//
		if (hdl->rx_type == RECEIVE_EVENT_overflow) {
			hdl->rx_console_last_pos = 0;
			lib_ser__set_receiver_frame(hdl->ser_console_hdl,(uint8_t*)hdl->rx_console_buffer_addr, hdl->rx_console_buffer_size);
		}
		else
		{
			rx_console_last_pos = hdl->rx_console_last_pos;

			/*Process Echo*/
			found = lib_console__process_console(hdl,&hdl->rx_console_buffer_addr[rx_console_last_pos], hdl->number_received_data, rx_console_last_pos);
			if (found == 1) {
				
				lib_ser__set_receiver_on_off(hdl->ser_console_hdl,RECEIVE_MODE_disable);
				lib_console__print_debug_message(hdl,"%s\n\r",&hdl->rx_console_buffer_addr[rx_console_last_pos]);
				lib_thread__sem_post(hdl->rx_serial_sem);
			}
			else if (found == 2) {
				lib_ser__set_receiver_frame(hdl->ser_console_hdl, &hdl->rx_console_buffer_addr[rx_console_last_pos-1], hdl->rx_console_buffer_size - rx_console_last_pos);
				lib_ser__send(hdl->ser_console_hdl,(uint8_t*)&hdl->rx_console_buffer_addr[rx_console_last_pos],1,NULL);
				hdl->rx_console_buffer_addr[rx_console_last_pos] = 0;
				hdl->rx_console_last_pos-=1;
				hdl->char_del_active = 1;
			}
			else {
				if ((hdl->number_received_data - rx_console_last_pos) > 1) {
					lib_console__send(hdl,(uint8_t*)&hdl->rx_console_buffer_addr[rx_console_last_pos],hdl->number_received_data - rx_console_last_pos);
				}
				else if ((hdl->number_received_data - rx_console_last_pos) == 1) {
					lib_ser__send(hdl->ser_console_hdl,(uint8_t*)&hdl->rx_console_buffer_addr[rx_console_last_pos],1,NULL);
				}	
				lib_ser__set_receiver_on_off(hdl->ser_console_hdl,RECEIVE_MODE_enable);
				hdl->rx_console_last_pos = hdl->number_received_data;
			}
		}
	}

	return NULL;
}

static inline unsigned int lib_console__process_console(console_hdl_t _hdl, char *_buffer, size_t _rcv_length, size_t _already_processed)
{
	unsigned int length = _hdl->number_received_data - _already_processed;
	unsigned int iterator;

	for(iterator = 0; iterator < length; iterator++) 
	{
		if(_buffer[iterator] == _hdl->rx_console_delimiter) {
			_buffer[iterator] = 0;
		   return 1;
		}
		else if (_buffer[iterator] == 127) {
	//		_buffer[iterator] = 0;
			_buffer[iterator - 1] = 0;
			return 2;
		}
	}
	return 0;
}
