cmake_minimum_required(VERSION 3.7)

set(ProjectName Viewer)
project(${ProjectName})

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

include_directories(AFTER "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(AFTER "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/FBXConverter/")

set(ROOT_INCLUDE "./include")
set(ROOT_SOURCE "./src")

set(MODULES "/")

set(HEADERS "")
set(SOURCES "")

foreach(MODULE IN LISTS MODULES)
    set(HFILES "")
    set(CPPFILES "")
    expand_files(HFILES "${ROOT_INCLUDE}${MODULE}/*h")
    expand_files(CPPFILES "${ROOT_SOURCE}${MODULE}/*cpp")

    set(HEADERS ${HEADERS} ${HFILES})
    set(SOURCES ${SOURCES} ${CPPFILES})

    # Grouping files
    string(REPLACE "/" "\\" FILTER "include/${MODULE}")
    source_group("${FILTER}" FILES ${HFILES})
    string(REPLACE "/" "\\" FILTER "src/${MODULE}")
    source_group("${FILTER}" FILES ${CPPFILES})
endforeach()

set(FBXCONVDIR "../../tools/FBXConverter/")
set(HFILES "${FBXCONVDIR}converter.h;${FBXCONVDIR}load_geometry.h;${FBXCONVDIR}ObjConverter.h")
set(CPPFILES "${FBXCONVDIR}converter.cpp;${FBXCONVDIR}load_geometry.cpp;${FBXCONVDIR}ObjConverter.cpp")
set(HEADERS ${HEADERS} ${HFILES})
set(SOURCES ${SOURCES} ${CPPFILES})
source_group("include" FILES ${HFILES})
source_group("src" FILES ${CPPFILES})

set(FILES ${HEADERS} ${SOURCES})

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

add_executable(${ProjectName} ${FILES})

set_target_properties(${ProjectName} PROPERTIES OUTPUT_NAME_DEBUG "${ProjectName}d" OUTPUT_NAME_RELEASE "${ProjectName}")
target_link_libraries(${ProjectName} LCORE LMATH LGRAPHICS LSOUND LFRAMEWORK INPUTLIB IMMLIB FBX)

if(MSVC)
set_target_properties(${ProjectName} PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS"
    LINK_FLAGS_RELEASE "/LTCG /SUBSYSTEM:WINDOWS")

    get_vs_working_directory(TMPVS_PROPERTIES ${CMAKE_CURRENT_SOURCE_DIR})
    write_vs_user(${ProjectName} ${TMPVS_PROPERTIES})

elseif(UNIX)
elseif(APPLE)
endif()

