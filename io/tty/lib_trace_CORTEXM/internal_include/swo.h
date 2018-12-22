/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2012-2013  SEGGER Microcontroller GmbH & Co KG           *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : SWO.c
Purpose : Simple implementation for output via SWO for Cortex-M processors.
          It can be used with any IDE. This sample implementation ensures that
          output via SWO is enabled in order to gurantee that the application
          does not hang.

--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef VA_DEV_TRACE_CORTEXM_SWO_H_M_SWO_H_
#define VA_DEV_TRACE_CORTEXM_SWO_H_M_SWO_H_

#include <stdint.h>

size_t SWO_PrintString (const char* buf, size_t nbyte);


/*VA_DEV_TRACE_CORTEXM_SWO_H_DEV_TRACE_CORTEXM_SWO_H_*/
#endif
