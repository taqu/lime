cmake_minimum_required(VERSION 3.7)

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

if(MSVC)
    target_link_libraries(${ProjectTest} LCORE LMATH LGRAPHICS LSOUND LFRAMEWORK)
    target_link_libraries(${ProjectTest} ${LCORE_DEPEND_LIBS} ${LMATH_DEPEND_LIBS} ${LGRAPHICS_DEPEND_LIBS} ${LFRAMEWORK_DEPEND_LIBS})
    set_target_properties(${ProjectTest} PROPERTIES
        LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
        LINK_FLAGS_RELEASE "/LTCG /SUBSYSTEM:CONSOLE")
    get_vs_working_directory(TMPVS_PROPERTIES ${CMAKE_CURRENT_SOURCE_DIR})
    write_vs_user(${ProjectTest} ${TMPVS_PROPERTIES})
elseif(UNIX)
elseif(APPLE)
endif()

add_dependencies(${ProjectTest} ${ProjectName})
