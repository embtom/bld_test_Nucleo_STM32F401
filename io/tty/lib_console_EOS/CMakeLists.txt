project(lib_console)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.c")


#Generate the static library from the sources

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} lib_convention lib_FREERTOS lib_mini_printf lib_thread lib_ser) 
target_include_directories(${PROJECT_NAME} PUBLIC ./)
