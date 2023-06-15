cmake_minimum_required(VERSION 3.10)

set(EXE_NAME ubus_fd_client)

project(${EXE_NAME} VERSION 1.0.0 DESCRIPTION "test streaming debug from ubus server")

add_compile_options(
   -std=gnu11
  -O3 
  -Wall 
  -Werror
  -Wextra 
  -g 
  -D_GNU_SOURCE 
)

include(GNUInstallDirs)

find_library(BLOBMSG_JSON blobmsg_json CONFIG REQUIRED)
find_library(UBOX ubox)
find_library(UBUS ubus)

SET(SOURCES 
  client.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
  ${BLOBMSG_JSON}
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

