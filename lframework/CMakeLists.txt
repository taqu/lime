cmake_minimum_required(VERSION 3.5)

include("${CMAKE_CURRENT_SOURCE_DIR}/../CMakeCommon.txt")

set(ProjectName lframework)
project(${ProjectName})

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

if(WIN32)
set(LFW_INPUT_DIRECT_INPUT ON)
endif()

if(NOT FW_CONFIG_SHADOW_MAXCASCADES)
set(FW_CONFIG_SHADOW_MAXCASCADES 4)
endif()

if(NOT FW_CONFIG_NAMESIZE)
set(FW_CONFIG_NAMESIZE 32)
endif()

if(NOT FW_CONFIG_FILENAMESIZE)
set(FW_CONFIG_FILENAMESIZE 64)
endif()

# Set configuration
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/Config.h" NEWLINE_STYLE LF)

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

set(ROOT_INCLUDE "./")
set(ROOT_SOURCE "./src")

set(MODULES
    "/"
    "/ecs"
    "/input"
    "/input/DirectInput"
    "/render"
    "/subsystem"
    "/resource"
    "/animation")

set(HEADERS "")
set(SOURCES "")

foreach(MODULE IN LISTS MODULES)
    set(HFILES "")
    set(CPPFILES "")
    set(CFILES "")
    expand_files(HFILES "${ROOT_INCLUDE}${MODULE}/*h")
    expand_files(CPPFILES "${ROOT_SOURCE}${MODULE}/*cpp")
    expand_files(CFILES "${ROOT_SOURCE}${MODULE}/*c")

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

if(MSVC)
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm-" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /GL /GR- /DNDEBUG" CACHE STRING "Flags used by the compiler during release builds." FORCE)

    set(CMAKE_STATIC_LINKER_FLAGS "/INCREMENTAL:NO /LTCG" CACHE STRING "Flags used by the linker during release builds." FORCE)

elseif(UNIX)
elseif(APPLE)
endif()

add_library(${ProjectName} STATIC ${FILES})
set_target_properties(${ProjectName} PROPERTIES OUTPUT_NAME_DEBUG "${ProjectName}d" OUTPUT_NAME_RELEASE "${ProjectName}")

if(MSVC)
    set_target_properties(${ProjectName} PROPERTIES
        STATIC_LIBRARY_FLAGS "/LIBPATH:\"${EXTERNAL_LIB_DIR}\" \"dinput8.lib\" \"imm32.lib\"")

elseif(UNIX)
elseif(APPLE)
endif()

add_subdirectory(test)
add_subdirectory(debug)