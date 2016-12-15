cmake_minimum_required(VERSION 3.5)

set(ProjectTest ${ProjectName}_test)
project(${ProjectTest})

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

set(HEADERS "")
set(CPPFILES "")
set(CFILES "")
expand_files(HEADERS "./*.h")
expand_files(CPPFILES "./*.cpp")
expand_files(CFILES "./*.c")

set(FILES ${CPPFILES} ${CFILES})
source_group(include FILES ${HEADERS})
source_group(src FILES ${FILES})

set(FILES ${HEADERS} ${FILES})

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

if(MSVC)
set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm-" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /GL /GR- /DNDEBUG" CACHE STRING "Flags used by the compiler during release builds." FORCE)

elseif(UNIX)
elseif(APPLE)
endif()

add_executable(${ProjectTest} ${FILES})

if(MSVC)
target_link_libraries(${ProjectTest} LCORE GTEST)
target_link_libraries(${ProjectTest} ${LCORE_DEPEND_LIBS})
set_target_properties(${ProjectTest} PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELEASE "/LTCG /SUBSYSTEM:CONSOLE")
elseif(UNIX)
elseif(APPLE)
endif()

set_vs_working_directory(${ProjectTest} ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(${ProjectTest} ${ProjectName})
