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


/* *******************************************************************
 * includes
 * ******************************************************************/
#include <stdint.h>
#include <sys/types.h>

#ifdef STM32HAL_F1
	#include <stm32f1xx.h>
#elif STM32HAL_F4
	#include <stm32f4xx.h>
#endif


/* *******************************************************************
 * defines
 * ******************************************************************/
//SWI numbers and reason codes for RDI (Angel) monitors.
#define AngelSWI_ARM 			0x123456
#ifdef __thumb__
#define AngelSWI 			0xAB
#else
#define AngelSWI 			AngelSWI_ARM
#endif
// For thumb only architectures use the BKPT instruction instead of SWI.
#if defined(__ARM_ARCH_7M__)     \
    || defined(__ARM_ARCH_7EM__) \
    || defined(__ARM_ARCH_6M__)
#define AngelSWIInsn			"bkpt"
#define AngelSWIAsm			bkpt
#else
#define AngelSWIInsn			"swi"
#define AngelSWIAsm			swi
#endif


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
// Semihosting operations.
enum OperationNumber
{
  // Regular operations
  SEMIHOSTING_EnterSVC = 0x17,
  SEMIHOSTING_ReportException = 0x18,
  SEMIHOSTING_SYS_CLOSE = 0x02,
  SEMIHOSTING_SYS_CLOCK = 0x10,
  SEMIHOSTING_SYS_ELAPSED = 0x30,
  SEMIHOSTING_SYS_ERRNO = 0x13,
  SEMIHOSTING_SYS_FLEN = 0x0C,
  SEMIHOSTING_SYS_GET_CMDLINE = 0x15,
  SEMIHOSTING_SYS_HEAPINFO = 0x16,
  SEMIHOSTING_SYS_ISERROR = 0x08,
  SEMIHOSTING_SYS_ISTTY = 0x09,
  SEMIHOSTING_SYS_OPEN = 0x01,
  SEMIHOSTING_SYS_READ = 0x06,
  SEMIHOSTING_SYS_READC = 0x07,
  SEMIHOSTING_SYS_REMOVE = 0x0E,
  SEMIHOSTING_SYS_RENAME = 0x0F,
  SEMIHOSTING_SYS_SEEK = 0x0A,
  SEMIHOSTING_SYS_SYSTEM = 0x12,
  SEMIHOSTING_SYS_TICKFREQ = 0x31,
  SEMIHOSTING_SYS_TIME = 0x11,
  SEMIHOSTING_SYS_TMPNAM = 0x0D,
  SEMIHOSTING_SYS_WRITE = 0x05,
  SEMIHOSTING_SYS_WRITEC = 0x03,
  SEMIHOSTING_SYS_WRITE0 = 0x04,

  // Codes returned by SEMIHOSTING_ReportException
  ADP_Stopped_ApplicationExit = ((2 << 16) + 38),
  ADP_Stopped_RunTimeError = ((2 << 16) + 35),

};

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static inline int call_host (int reason, void* arg);


/*********************************************************************
*
*       SWO_PrintString()
*
* Function description
*   Print a string via SWO.
*
*/
size_t SWO_PrintString (const char* buf, size_t nbyte)
{
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
  // Check if the debugger is enabled. CoreDebug is available only on CM3/CM4.
  // [Contributed by SourceForge user diabolo38]
  if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) == 0)
    {
      // If not, pretend we wrote all bytes
      return (ssize_t) (nbyte);
    }
#endif // defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)

  static int handle;
  void* block[3];
  int ret;

  if (handle == 0)
    {
      // On the first call get the file handle from the host
      block[0] = ":tt"; // special filename to be used for stdin/out/err
      block[1] = (void*) 4; // mode "w"
      // length of ":tt", except null terminator
      block[2] = (void*) (sizeof(":tt") - 1);

      ret = call_host (SEMIHOSTING_SYS_OPEN, (void*) block);
      if (ret == -1)
        return -1;

      handle = ret;
    }

  block[0] = (void*) handle;
  block[1] = (void*) buf;
  block[2] = (void*) nbyte;
  // send character array to host file/device
  ret = call_host (SEMIHOSTING_SYS_WRITE, (void*) block);
  // this call returns the number of bytes NOT written (0 if all ok)

  // -1 is not a legal value, but SEGGER seems to return it
  if (ret == -1)
    return -1;

  // The compliant way of returning errors
  if (ret == (int) nbyte)
    return -1;

  // Return the number of bytes written
  return (ssize_t) (nbyte) - (ssize_t) ret;

}

/* *******************************************************************
 * static function definitions
 * ******************************************************************/
static inline int call_host (int reason, void* arg)
{
  int value;
  __asm volatile (

      " mov r0, %[rsn]  \n"
      " mov r1, %[arg]  \n"
      " " AngelSWIInsn " %[swi] \n"
      " mov %[val], r0"

      : [val] "=r" (value) /* Outputs */
      : [rsn] "r" (reason), [arg] "r" (arg), [swi] "i" (AngelSWI) /* Inputs */
      : "r0", "r1", "r2", "r3", "ip", "lr", "memory", "cc"
      // Clobbers r0 and r1, and lr if in supervisor mode
  );

  // Accordingly to page 13-77 of ARM DUI 0040D other registers
  // can also be clobbered. Some memory positions may also be
  // changed by a system call, so they should not be kept in
  // registers. Note: we are assuming the manual is right and
  // Angel is respecting the APCS.
  return value;
}
