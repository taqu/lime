cmake_minimum_required(VERSION 3.5)

set(ProjectTest ${ProjectName}_test)
project(${ProjectTest})

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

set(HEADERS "")
set(CPPFILES "")
set(CFILES "")
expand_files(HEADERS "./*.h")
expand_files(CPPFILES "./*.cpp")

source_group(include FILES ${HEADERS})
source_group(src FILES ${CPPFILES})

set(FILES ${HEADERS} ${CPPFILES})

add_executable(${ProjectTest} ${FILES})

target_link_libraries(${ProjectTest} LCORE LMATH LGRAPHICS LFRAMEWORK GTEST)
target_link_libraries(${ProjectTest} ${LCORE_DEPEND_LIBS} ${LMATH_DEPEND_LIBS} ${LGRAPHICS_DEPEND_LIBS} ${LFRAMEWORK_DEPEND_LIBS})

if(MSVC)
set_target_properties(${ProjectTest} PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELEASE "/LTCG /SUBSYSTEM:CONSOLE")
elseif(UNIX)
elseif(APPLE)
endif()

set_vs_working_directory(${ProjectTest} ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(${ProjectTest} ${ProjectName})
