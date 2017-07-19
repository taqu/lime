cmake_minimum_required(VERSION 3.7)

include("${CMAKE_CURRENT_SOURCE_DIR}/../CMakeCommon.txt")

set(ProjectName lframework)
project(${ProjectName})

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
set(SHADER_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/shader_source/")

if(WIN32)
set(LFW_INPUT_DIRECT_INPUT ON)
endif()

if(NOT FW_CONFIG_SHADOW_NUMCASCADES)
set(FW_CONFIG_SHADOW_NUMCASCADES 2)
endif()

if(NOT FW_CONFIG_NAMESIZE)
set(FW_CONFIG_NAMESIZE 32)
endif()

if(NOT FW_CONFIG_FILENAMESIZE)
set(FW_CONFIG_FILENAMESIZE 64)
endif()

if(NOT FW_CONFIG_COMPUTESHADER_NUMTHREADS_BITSHIFT)
set(FW_CONFIG_COMPUTESHADER_NUMTHREADS_BITSHIFT 3)
endif()

if(NOT FW_CONFIG_COMPUTESHADER_NUMTHREADS)
set(FW_CONFIG_COMPUTESHADER_NUMTHREADS 8)
endif()

if(NOT FW_CONFIG_DISABLE_GUI)
set(LFW_CONFIG_ENABLE_GUI ON)
endif()

if(NOT FW_CONFIG_DISABLE_ONLYDEBUG_GUI)
set(LFW_CONFIG_ENABLE_ONLYDEBUG_GUI ON)
endif()

# Set configuration
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/Config.h" NEWLINE_STYLE LF)
configure_file("${SHADER_SOURCE_DIRECTORY}/Constants.inc.in" "${SHADER_SOURCE_DIRECTORY}/Constants.inc" NEWLINE_STYLE LF)

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

set(ROOT_INCLUDE "./")
set(ROOT_SOURCE "./src")

set(MODULES
    "/"
    "/ecs"
    "/input"
    "/input/DirectInput"
    "/render"
    "/render/graph"
    "/subsystem"
    "/resource"
    "/animation"
    "/physics"
    "/gui")

set(HEADERS "")
set(SOURCES "")

foreach(MODULE IN LISTS MODULES)
    set(HFILES "")
    set(CPPFILES "")
    set(CFILES "")
    expand_files(HFILES "${ROOT_INCLUDE}${MODULE}/*.h")
    expand_files(CPPFILES "${ROOT_SOURCE}${MODULE}/*.cpp")
    expand_files(CFILES "${ROOT_SOURCE}${MODULE}/*.c")

    set(HEADERS ${HEADERS} ${HFILES})
    set(SOURCES ${SOURCES} ${CPPFILES} ${CFILES})

    # Grouping files
    string(REPLACE "/" "\\" FILTER "include/${MODULE}")
    source_group("${FILTER}" FILES ${HFILES})
    string(REPLACE "/" "\\" FILTER "src/${MODULE}")
    source_group("${FILTER}" FILES ${CPPFILES} ${CFILES})
endforeach()

set(FILES ${HEADERS} ${SOURCES})

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

add_library(${ProjectName} STATIC ${FILES})
set_target_properties(${ProjectName} PROPERTIES OUTPUT_NAME_DEBUG "${ProjectName}d" OUTPUT_NAME_RELEASE "${ProjectName}")

if(MSVC)
    set_target_properties(${ProjectName} PROPERTIES
        STATIC_LIBRARY_FLAGS "/LIBPATH:\"${EXTERNAL_LIB_DIR}\" \"dinput8.lib\" \"imm32.lib\""
        STATIC_LIBRARY_FLAGS_RELEASE "/LTCG")

    if(1900 LESS MSVC_VERSION)
        get_vs_property_group(VSPROPERTIES "EnableCppCoreCheck" "true")
        write_vs_user(${ProjectName} ${VSPROPERTIES})
    endif()
elseif(UNIX)
elseif(APPLE)
endif()
add_subdirectory(test)
add_subdirectory(debug)
