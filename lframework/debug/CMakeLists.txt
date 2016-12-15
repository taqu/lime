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

if(MSVC)
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm-" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /GL /GR- /DNDEBUG" CACHE STRING "Flags used by the compiler during release builds." FORCE)

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ConfigDialog.res")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /INCREMENTAL /SUBSYSTEM:WINDOWS" CACHE STRING "Flags used by the linker during debug builds." FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO /LTCG /SUBSYSTEM:WINDOWS" CACHE STRING "Flags used by the linker during release builds." FORCE)


elseif(UNIX)
elseif(APPLE)
endif()

add_library(INPUTLIB MODULE IMPORTED)
add_library(IMMLIB MODULE IMPORTED)
set_target_properties(INPUTLIB PROPERTIES IMPORTED_LOCATION "dinput8.lib")
set_target_properties(IMMLIB PROPERTIES IMPORTED_LOCATION "imm32.lib")

add_executable(${ProjectDebug} ${FILES})
target_link_libraries(${ProjectDebug} LCORE LMATH LGRAPHICS INPUTLIB IMMLIB)
target_link_libraries(${ProjectDebug} ${LCORE_DEPEND_LIBS} ${LMATH_DEPEND_LIBS} ${LGRAPHICS_DEPEND_LIBS})

set_vs_working_directory(${ProjectDebug} ${CMAKE_CURRENT_SOURCE_DIR})
