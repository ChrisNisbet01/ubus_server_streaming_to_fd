cmake_minimum_required(VERSION 3.10)

set(EXE_NAME debug_server)

project(${EXE_NAME} VERSION 1.0.0 DESCRIPTION "Streams debug to subscribed listeners")

option(DEBUG "Include debug output" OFF)

add_compile_options(
   -std=gnu11
  -O3 
  -Wall 
  -Wextra 
  -Werror
  -g 
  -D_GNU_SOURCE 
)

include(GNUInstallDirs)

find_library(UBOX ubox)
find_library(UBUS ubus)

SET(SOURCES 
  src/debug_output.c
  src/streamer.c
  src/streamerd.c
  src/ubus.c
  src/utils.c
  src/ubus_connection.c
  src/ubus_utils.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
  PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME}
  ${UBOX}
  ${UBUS}
)

set_target_properties(${PROJECT_NAME} 
  PROPERTIES 
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME ${EXE_NAME}
)

install(TARGETS ${PROJECT_NAME} 
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

