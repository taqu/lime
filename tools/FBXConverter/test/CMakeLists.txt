cmake_minimum_required(VERSION 3.5)

set(ProjectTest ${ProjectName}_test)
project(${ProjectTest})

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

set(ROOT_INCLUDE "./")
set(ROOT_SOURCE "./")
set(PROJROOT_INCLUDE "../")
set(PROJROOT_SOURCE "../")

set(MODULES "/")

set(HEADERS "")
set(SOURCES "")

foreach(MODULE IN LISTS MODULES)
    set(HFILES "")
    set(CPPFILES "")
    set(PROJHFILES "")
    set(PROJCPPFILES "")

    expand_files(HFILES "${ROOT_INCLUDE}${MODULE}/*h")
    expand_files(CPPFILES "${ROOT_SOURCE}${MODULE}/*cpp")

    expand_files(PROJHFILES "${PROJROOT_INCLUDE}${MODULE}/*h")
    expand_files(PROJCPPFILES "${PROJROOT_SOURCE}${MODULE}/*cpp")

    set(HEADERS ${HEADERS} ${HFILES} ${PROJHFILES})
    set(SOURCES ${SOURCES} ${CPPFILES} ${PROJCPPFILES})

    # Grouping files
    string(REPLACE "/" "\\" FILTER "include/${MODULE}")
    source_group("${FILTER}" FILES ${HFILES} ${PROJHFILES})
    string(REPLACE "/" "\\" FILTER "src/${MODULE}")
    source_group("${FILTER}" FILES ${CPPFILES} ${PROJCPPFILES})
endforeach()

set(FILES ${HEADERS} ${SOURCES})

if(MSVC)
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm-" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /GL /GR- /DNDEBUG" CACHE STRING "Flags used by the compiler during release builds." FORCE)

    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO /LTCG" CACHE STRING "Flags used by the linker during release builds." FORCE)

elseif(UNIX)
elseif(APPLE)
endif()

add_executable(${ProjectTest} ${FILES})
set_target_properties(${ProjectTest} PROPERTIES OUTPUT_NAME_DEBUG "${ProjectTest}d" OUTPUT_NAME_RELEASE "${ProjectTest}" COMPILE_OPTIONS "/DFBXCONVERTER_TEST")

if(MSVC)
    target_link_libraries(${ProjectTest} LCORE LMATH LGRAPHICS FBX GTEST)
elseif(UNIX)
elseif(APPLE)
endif()

set_vs_working_directory(${ProjectTest} ${CMAKE_CURRENT_SOURCE_DIR})
