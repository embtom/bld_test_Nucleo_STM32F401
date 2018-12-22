project(lib_isr)

SET (LIB_ISR_SOURCE src/lib_isr.c 
                    src/isr_hdl.c)

SET (LIB_ISR_HEADER include)
                                                                
#Generate the static library from the sources
add_library(${PROJECT_NAME} STATIC ${LIB_ISR_SOURCE})
target_link_libraries(${PROJECT_NAME} lib_convention lib_log lib_isr_config)
target_include_directories(${PROJECT_NAME} PUBLIC ${LIB_ISR_HEADER})
