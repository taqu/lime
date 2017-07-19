cmake_minimum_required(VERSION 3.5)

include("${CMAKE_CURRENT_SOURCE_DIR}/../CMakeCommon.txt")

set(ProjectName tools)
project(${ProjectName})
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

set(EXTERNAL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../external/lib")

include_directories(AFTER "${CMAKE_CURRENT_SOURCE_DIR}/../")
include_directories(AFTER "${CMAKE_CURRENT_SOURCE_DIR}/../external/include")
link_directories("../external/lib")
link_directories("../lib")

set(FBXSDK "$ENV{FBXSDK}")
string(REPLACE "\\" "/" FBXSDK ${FBXSDK})
include_directories(AFTER "${FBXSDK}/include")

set(TOOLS_OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_library(LCORE MODULE IMPORTED)
add_library(LMATH MODULE IMPORTED)
add_library(LGRAPHICS MODULE IMPORTED)
add_library(LSOUND MODULE IMPORTED)
add_library(LFRAMEWORK MODULE IMPORTED)
add_library(LRENDER MODULE IMPORTED)
add_library(GTEST MODULE IMPORTED)
add_library(FBX MODULE IMPORTED)

set_target_properties(LCORE PROPERTIES IMPORTED_LOCATION_DEBUG "lcored.lib" IMPORTED_LOCATION_RELEASE "lcore.lib")
set_target_properties(LMATH PROPERTIES IMPORTED_LOCATION_DEBUG "lmathd.lib" IMPORTED_LOCATION_RELEASE "lmath.lib")
set_target_properties(LGRAPHICS PROPERTIES IMPORTED_LOCATION_DEBUG "lgraphicsd.lib" IMPORTED_LOCATION_RELEASE "lgraphics.lib")
set_target_properties(LSOUND PROPERTIES IMPORTED_LOCATION_DEBUG "lsoundd.lib" IMPORTED_LOCATION_RELEASE "lsound.lib")
set_target_properties(LFRAMEWORK PROPERTIES IMPORTED_LOCATION_DEBUG "lframeworkd.lib" IMPORTED_LOCATION_RELEASE "lframework.lib")
set_target_properties(LRENDER PROPERTIES IMPORTED_LOCATION_DEBUG "lrenderd.lib" IMPORTED_LOCATION_RELEASE "lrender.lib")
set_target_properties(GTEST PROPERTIES IMPORTED_LOCATION_DEBUG "gtestd.lib" IMPORTED_LOCATION_RELEASE "gtest.lib")
set_target_properties(FBX PROPERTIES
    IMPORTED_LOCATION_DEBUG "${FBXSDK}/lib/vs2015/x64/debug/libfbxsdk-mt.lib"
    IMPORTED_LOCATION_RELEASE "${FBXSDK}/lib/vs2015/x64/release/libfbxsdk-mt.lib")

if(MSVC)

set(DEFAULT_CXX_FLAGS "/DWIN32 /D_WINDOWS /D_MBCS /W4 /WX- /nologo /fp:precise /Zc:wchar_t /TP /Gd /FIforceinclude.h")
if("1800" VERSION_LESS MSVC_VERSION)
set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /EHsc")
endif()

elseif(UNIX)
elseif(APPLE)
endif()

add_subdirectory(ShaderCompiler)
add_subdirectory(FBXConverter)
add_subdirectory(SoundPacker)
