cmake_minimum_required(VERSION 3.5)

set(ProjectName FBXConverter)
project(${ProjectName})

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

set(ROOT_INCLUDE "./")
set(ROOT_SOURCE "./")

set(MODULES "/")

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
    source_group("${FILTER}" FILES ${HEADERS})
    string(REPLACE "/" "\\" FILTER "src/${MODULE}")
    source_group("${FILTER}" FILES ${CPPFILES} ${CFILES})
endforeach()

set(FILES ${HEADERS} ${SOURCES})

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm-" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /GL /GR- /DNDEBUG" CACHE STRING "Flags used by the compiler during release builds." FORCE)

    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO /LTCG" CACHE STRING "Flags used by the linker during release builds." FORCE)

elseif(UNIX)
elseif(APPLE)
endif()

add_executable(${ProjectName} ${FILES})
set_target_properties(${ProjectName} PROPERTIES OUTPUT_NAME_DEBUG "${ProjectName}d" OUTPUT_NAME_RELEASE "${ProjectName}")

if(MSVC)
    target_link_libraries(${ProjectName} LCORE LMATH LGRAPHICS FBX)
elseif(UNIX)
elseif(APPLE)
endif()

set_vs_working_directory(${ProjectName} ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(test)
