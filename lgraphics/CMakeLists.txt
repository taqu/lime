cmake_minimum_required(VERSION 3.7)

include("${CMAKE_CURRENT_SOURCE_DIR}/../CMakeCommon.txt")

set(ProjectName lgraphics)
project(${ProjectName})

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")

if(MSVC)
elseif(UNIX)
set(LGFX_USE_X ON)
elseif(APPLE)
endif()

# Check using API
if(NOT (DEFINED GFX_API))
set(GFX_API DX11)
endif()

# Set default variables
if(${GFX_API} STREQUAL "DX11")
set(LGFX_DX11 ON)
set(GFX_MAX_SKINNING_MATRICES 255)
set(GFX_MAX_RENDER_TARGETS 8)

elseif(${GFX_API} STREQUAL "GLES2")
set(LGFX_GLES2 ON)
set(GFX_MAX_SKINNING_MATRICES 36)
set(GFX_MAX_RENDER_TARGETS 1)

else()
set(LGFX_DX9 ON)
set(GFX_MAX_SKINNING_MATRICES 64)
set(GFX_MAX_RENDER_TARGETS 1)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/Config.h" NEWLINE_STYLE LF)

include_directories(AFTER ${CMAKE_CURRENT_SOURCE_DIR})

set(ROOT_INCLUDE "./")
set(ROOT_SOURCE "./src")

# Define API source directories
if(${GFX_API} STREQUAL "DX11")
set(GFX_API_DIR "dx11")
else()
set(GFX_API_DIR "dx9")
endif()

set(MODULES "/" "/io" "/${GFX_API_DIR}" "/${GFX_API_DIR}/io")

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
    source_group("${FILTER}" FILES ${HFILES})
    string(REPLACE "/" "\\" FILTER "src/${MODULE}")
    source_group("${FILTER}" FILES ${CPPFILES} ${CFILES})
endforeach()

set(FILES ${HEADERS} ${SOURCES})

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")

add_library(${ProjectName} STATIC ${FILES})
set_target_properties(${ProjectName} PROPERTIES OUTPUT_NAME_DEBUG "${ProjectName}d" OUTPUT_NAME_RELEASE "${ProjectName}")

if(MSVC)
    set_target_properties(${ProjectName} PROPERTIES
        STATIC_LIBRARY_FLAGS "/LIBPATH:\"${EXTERNAL_LIB_DIR}\" \"d3d11.lib\" \"dxguid.lib\" \"dxgi.lib\""
        STATIC_LIBRARY_FLAGS_DEBUG  "\"lz4d.lib\""
        STATIC_LIBRARY_FLAGS_RELEASE "/LTCG \"lz4.lib\"")

    if(1900 LESS MSVC_VERSION)
        get_vs_property_group(VSPROPERTIES "EnableCppCoreCheck" "true")
        write_vs_user(${ProjectName} ${VSPROPERTIES})
    endif()
elseif(UNIX)
elseif(APPLE)
endif()

add_subdirectory(test)
