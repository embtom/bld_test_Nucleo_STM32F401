project(entry_point)

add_subdirectory(isr/lib_isr)
add_subdirectory(isr/lib_isr_config)

SET(ENTRY_POINT_SRC main.c 
                    stm32f4xx_hal_msp.c 
                    stm32f4xx_it.c)

#lib_startup_STM32F401XE
SET(LINK_LIBRARIES_LIST lib_clock lib_isr
                        lib_stm32hal lib_convention  
                        lib_log  lib_tty_portmux
                        lib_FREERTOS lib_thread 
                        app_test )


set(EXECUTABLE_OUTPUT_PATH "${STM32_TARGET_DIR}")

add_executable(${STM32_TARGET_NAME}.elf ${ENTRY_POINT_SRC})
target_link_libraries(${STM32_TARGET_NAME}.elf ${LINK_LIBRARIES_LIST})

STM32_ADD_HEX_BIN_TARGETS(${STM32_TARGET_NAME})
STM32_PRINT_SIZE_OF_TARGETS(${STM32_TARGET_NAME})

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${STM32_TARGET_DIR})
add_custom_target(build-time-make-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${STM32_TARGET_DIR})

