cmake_minimum_required(VERSION 3.5)

include("${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeCommon.txt")

set(ProjectDebug lframework_debug)
project(${ProjectDebug})

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

set(ROOT_INCLUDE "../")
set(ROOT_SOURCE "../src")

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

set(HFILES "")
set(CPPFILES "")
expand_files(HFILES "./*h")
expand_files(CPPFILES "./*cpp")
source_group("include\\" FILES ${HFILES})
source_group("src\\" FILES ${CPPFILES})
set(HEADERS ${HEADERS} ${HFILES})
set(SOURCES ${SOURCES} ${CPPFILES})

set(FILES ${HEADERS} ${SOURCES})

add_library(INPUTLIB MODULE IMPORTED)
add_library(IMMLIB MODULE IMPORTED)
set_target_properties(INPUTLIB PROPERTIES IMPORTED_LOCATION "dinput8.lib")
set_target_properties(IMMLIB PROPERTIES IMPORTED_LOCATION "imm32.lib")

add_executable(${ProjectDebug} ${FILES})
target_link_libraries(${ProjectDebug} LCORE LMATH LGRAPHICS LSOUND INPUTLIB IMMLIB)
target_link_libraries(${ProjectDebug} ${LCORE_DEPEND_LIBS} ${LMATH_DEPEND_LIBS} ${LGRAPHICS_DEPEND_LIBS})

if(MSVC)
set_target_properties(${ProjectDebug} PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS"
        LINK_FLAGS_RELEASE "/LTCG /SUBSYSTEM:WINDOWS")
elseif(UNIX)
elseif(APPLE)
endif()

set_vs_working_directory(${ProjectDebug} ${CMAKE_CURRENT_SOURCE_DIR})
