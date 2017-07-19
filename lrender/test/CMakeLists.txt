cmake_minimum_required(VERSION 3.7)

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

add_executable(${ProjectTest} ${FILES})

if(MSVC)
    target_link_libraries(${ProjectTest} LCORE LMATH LGRAPHICS LRENDER GTEST)
    target_link_libraries(${ProjectTest} ${LCORE_DEPEND_LIBS} ${LMATH_DEPEND_LIBS} ${LGRAPHICS_DEPEND_LIBS} ${LRENDER_DEPENT_LIBS})
    set_target_properties(${ProjectTest} PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELEASE "/LTCG /SUBSYSTEM:CONSOLE")
    get_vs_working_directory(TMPVS_PROPERTIES ${CMAKE_CURRENT_SOURCE_DIR})
    write_vs_user(${ProjectTest} ${TMPVS_PROPERTIES})
elseif(UNIX)
elseif(APPLE)
endif()

add_dependencies(${ProjectTest} ${ProjectName})
