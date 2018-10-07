project( SDDL LANGUAGES CXX)

cmake_minimum_required(VERSION 3.1)
set ( CMAKE_BUILD_TYPE Debug )
set(CMAKE_CXX_FLAGS_MYREL "-O3")
#set ( CMAKE_BUILD_TYPE Release )

set (CMAKE_CXX_STANDARD 11)

set(SDDL "${CMAKE_CURRENT_SOURCE_DIR}")
message("  Path: ${SDDL}")

SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")

message("  System: ${CMAKE_SYSTEM}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message("  Platform: Linux")
add_definitions ( -Wall) #-pedantic -Werror -ansi -c -I.  -std=c++11
#add_definitions ( -Wall -std=c++11 -Wno-int-in-bool-context)

#set_source_files_properties(filename.c PROPERTIES LANGUAGE CXX )

#SET(CMAKE_C_COMPILER g++)

add_library(libsddl "")
#set_property(TARGET libsddl PROPERTY C_STANDARD 11)

message("Install path: ${CMAKE_INSTALL_PREFIX}")

set (USE_JSON true)
message("USE_JSON: ${USE_JSON}")

IF (USE_JSON)
find_package ( LibArchive REQUIRED )
message("  LibArchive_INCLUDE_DIRS: ${LibArchive_INCLUDE_DIRS}")
message("  LibArchive_LIBRARIES: ${LibArchive_LIBRARIES}")

#find_package ( Boost REQUIRED COMPONENTS regex system program_options)
#message("  Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
#message("  Boost_LIBRARY_DIR: ${Boost_LIBRARY_DIR}")

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${SDDL}/cmake_modules)
#message("  CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}") 

#find_package ( JSONCPP REQUIRED )
#message("  JSONCPP_INCLUDE_DIR: ${JSONCPP_INCLUDE_DIR}")
#message("  JSONCPP_LIBRARY: ${JSONCPP_LIBRARY}")

include_directories (
#    ${Boost_INCLUDE_DIRS}
    ${LibArchive_INCLUDE_DIRS}
    )
ENDIF()

include_directories (
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/include
    )

include(src/CMakeLists.txt)

configure_file(src/global.h.in include/global.h)

#set_property(TARGET sddl PROPERTY C_STANDARD 11)

IF (USE_JSON)
target_link_libraries ( sddl
#    ${Boost_LIBRARIES}
    ${LibArchive_LIBRARIES})
#    ${JSONCPP_LIBRARY})    
ENDIF()

install (TARGETS libsddl DESTINATION lib
    PUBLIC_HEADER DESTINATION include/sddl)
    
install (TARGETS sddl DESTINATION bin)

# build a CPack driven installer package
#include (InstallRequiredSystemLibraries)
#set (CPACK_RESOURCE_FILE_LICENSE
#     "${CMAKE_CURRENT_SOURCE_DIR}/gpl.txt")
#include (CPack)





