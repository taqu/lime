cmake_minimum_required(VERSION 3.7)

set(ProjectName lime)
project(${ProjectName})
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

set(EXTERNAL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/lib")

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(AFTER "${CMAKE_CURRENT_SOURCE_DIR}/external/include")
link_directories("external/lib")
link_directories("lib")

add_library(LCORE MODULE IMPORTED)
add_library(LMATH MODULE IMPORTED)
add_library(LGRAPHICS MODULE IMPORTED)
add_library(LSOUND MODULE IMPORTED)
add_library(LFRAMEWORK MODULE IMPORTED)
add_library(LRENDER MODULE IMPORTED)
add_library(GTEST MODULE IMPORTED)

set_target_properties(LCORE PROPERTIES IMPORTED_LOCATION_DEBUG "lcored.lib" IMPORTED_LOCATION_RELEASE "lcore.lib")
set_target_properties(LMATH PROPERTIES IMPORTED_LOCATION_DEBUG "lmathd.lib" IMPORTED_LOCATION_RELEASE "lmath.lib")
set_target_properties(LGRAPHICS PROPERTIES IMPORTED_LOCATION_DEBUG "lgraphicsd.lib" IMPORTED_LOCATION_RELEASE "lgraphics.lib")
set_target_properties(LSOUND PROPERTIES IMPORTED_LOCATION_DEBUG "lsoundd.lib" IMPORTED_LOCATION_RELEASE "lsound.lib")
set_target_properties(LFRAMEWORK PROPERTIES IMPORTED_LOCATION_DEBUG "lframeworkd.lib" IMPORTED_LOCATION_RELEASE "lframework.lib")
set_target_properties(LRENDER PROPERTIES IMPORTED_LOCATION_DEBUG "lrenderd.lib" IMPORTED_LOCATION_RELEASE "lrender.lib")
set_target_properties(GTEST PROPERTIES IMPORTED_LOCATION_DEBUG "gtestd.lib" IMPORTED_LOCATION_RELEASE "gtest.lib")

set(LCORE_DEPEND_LIBS "")
set(LMATH_DEPEND_LIBS "")
set(LGRAPHICS_DEPEND_LIBS "")
set(LFRAMEWORK_DEPEND_LIBS "")
set(LRENDER_DEPEND_LIBS "")

if(MSVC)

set(DEFAULT_CXX_FLAGS "/DWIN32 /D_WINDOWS /D_MBCS /W4 /WX- /nologo /fp:precise /Zc:wchar_t /TP /Gd /FIforceinclude.h")
if("1800" VERSION_LESS MSVC_VERSION)
set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /EHsc")
endif()

set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1 /Gy /GR- /GS /Gm-")
set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /GL /GR- /DNDEBUG")

elseif(UNIX)
elseif(APPLE)
endif()

add_subdirectory(lcore)
add_subdirectory(lmath)
add_subdirectory(lgraphics)
add_subdirectory(lsound)
add_subdirectory(lframework)
add_subdirectory(lrender)

#target_link_libraries(${ProjectName} debug "${FBX_SDK}lib/vs2013/x64/debug/libfbxsdk-mt.lib" optimized "${FBX_SDK}lib/vs2013/x64/release/libfbxsdk-mt.lib")
