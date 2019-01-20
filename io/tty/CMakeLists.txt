project(tty)

SET(LIB_TRACE_ARCHITECTURE stm32f4)
SET(LIB_SER_ARCHITECTURE stm32)
SET(LIB_SERIAL_ARCHITECTURE stm32)

add_subdirectory(lib_console)
add_subdirectory(lib_serial)
add_subdirectory(lib_trace_CORTEXM)
add_subdirectory(lib_tty_portmux)
add_subdirectory(lib_log)