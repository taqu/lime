
include("${CMAKE_CURRENT_SOURCE_DIR}/../CMakeCommon.txt")

set(ProjectName lsound)
project(${ProjectName})

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

set(ROOT_INCLUDE "./")
set(ROOT_SOURCE "./src")

set(MODULES
    "/"
    "/opus"
    "/dsp"
    "/Wasapi"
    "/speexdsp")

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
        STATIC_LIBRARY_FLAGS "/LIBPATH:\"${EXTERNAL_LIB_DIR}\""
        STATIC_LIBRARY_FLAGS_DEBUG   "\"celtd.lib\" \"silk_commond.lib\" \"silk_floatd.lib\" \"opusd.lib\" \"opusfiled.lib\" \"libogg_staticd.lib\""
        STATIC_LIBRARY_FLAGS_RELEASE "/LTCG  \"celt.lib\" \"silk_common.lib\" \"silk_float.lib\" \"opus.lib\" \"opusfile.lib\" \"libogg_static.lib\"")

elseif(UNIX)
elseif(APPLE)
endif()

#add_subdirectory(test)
