

################################################################
#Target configuration
SET(MCU "cortex-m4")

#Compiler flags -flto
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -mcpu=${MCU} -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -ffunction-sections -fdata-sections -fdata-sections -Wstrict-aliasing -fno-rtti -fno-exceptions")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=${MCU} -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -ffunction-sections -fdata-sections -Wstrict-aliasing" CACHE STRINGS "c compiler flags")
set(CMAKE_ASM_FLAGS "-mthumb -mcpu=${MCU} -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

SET(LINKER_SCRIPT "_bld_device_cmake_Nucleo_STM32F401/linkerscript/STM32F401VEHx_FLASH.ld")
SET(CMAKE_EXE_LINKER_FLAGS "-Xlinker --gc-sections -gdwarf-2 -T${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT}" CACHE INTERNAL "executable linker flags")
#SET(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> -r -c -s -o <TARGET> <LINK_FLAGS> <OBJECTS>")


################################################################
#Project
cmake_minimum_required(VERSION 2.8)
project (bld_device_cmake_Nucleo_STM32F401)
set(CMAKE_DEBUG_POSTFIX _g)

SET(STM32_TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/_bld_device_cmake_Nucleo_STM32F401/bin")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	SET(STM32_TARGET_NAME "${PROJECT_NAME}${CMAKE_DEBUG_POSTFIX}")
else()
	SET(STM32_TARGET_NAME "${PROJECT_NAME}")
endif()

# Minimal benötigte CMake-Version wenn z.B. bestimmte CMake-Kommandos benutzt werden

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -specs=nano.specs -specs=nosys.specs ")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${STM32_TARGET_DIR}/${STM32_TARGET_NAME}.map")
 
################################################################
#List of Subdirectories

#add_subdirectory(int_io)
add_subdirectory(os_base)
add_subdirectory(io)
add_subdirectory(test_modules)
add_subdirectory(entry_point)






