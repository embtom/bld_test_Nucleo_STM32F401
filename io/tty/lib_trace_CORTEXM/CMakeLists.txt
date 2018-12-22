project(lib_trace_CORTEXM)

#######################################################################################
# Target Config
#######################################################################################
SET(LIB_TRACE_SOURCE_C   		src/swo.c 
								src/Trace.c)
SET(LIB_TRACE_INTERFACE_HEADER  "include")
SET(LIB_TRACE_INTERNAL_HEADER   "internal_include")
SET(LIB_TRACE_PRIVATE_DEFINITION OS_USE_TRACE_SEMIHOSTING_STDOUT)

#######################################################################################
#Build target settings
#######################################################################################
add_library(${PROJECT_NAME} STATIC ${LIB_TRACE_SOURCE_C})
target_link_libraries(${PROJECT_NAME} lib_convention lib_FREERTOS lib_stm32hal lib_mini_printf)
target_include_directories(${PROJECT_NAME} PUBLIC ${LIB_TRACE_INTERFACE_HEADER})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIB_TRACE_INTERNAL_HEADER})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${LIB_TRACE_PRIVATE_DEFINITION})