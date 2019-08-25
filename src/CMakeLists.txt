include(CheckIncludeFileCXX)
include(GNUInstallDirs)

#[[
    Calls find_library forwarding all arguments to it.
    If the specified library is not found, stop CMake with a fatal error.
#]]
function(find_library_or_exit)
    cmake_parse_arguments(FLOE_ "" "" NAMES ${ARGV})
    find_library(${ARGV})
    if (NOT ${ARGV0})
        if(DEFINED FLOE_NAMES)
            list(GET FLOE_NAMES 0 name)
        else()
            set(name ${ARGV1})
        endif()
        message(FATAL_ERROR "Library '${name}' not found")
    endif()
endfunction()

function(find_program_or_exit)
    cmake_parse_arguments(FPOE_ "" "" NAMES ${ARGV})
    find_program(${ARGV})
    if (NOT ${ARGV0})
        if(DEFINED FPOE_NAMES)
            list(GET FPOE_NAMES 0 name)
        else()
            set(name ${ARGV1})
        endif()
        message(FATAL_ERROR "Program '${name}' not found")
    endif()
endfunction()

#[[
    Calls find_path forwarding all arguments to it.
    If the specified header is not found, stop CMake with a fatal error.
#]]
function(find_path_or_exit)
    cmake_parse_arguments(FPOE_ "" "" NAMES ${ARGV})
    find_path(${ARGV})
    if (NOT ${ARGV0})
        if(DEFINED FPOE_NAMES)
            list(GET FPOE_NAMES 0 name)
        else()
            set(name ${ARGV1})
        endif()
        message(FATAL_ERROR "Header '${name}' not found")
    endif()
endfunction()

function(option_add_list optname optdoc optdefault optlist)
    if(NOT DEFINED "CACHE{OPTION_${optname}}")
        set("OPTION_${optname}" "${optdefault}" CACHE STRING "${optdoc}")
        return()
    endif()
    if(NOT "$CACHE{OPTION_${optname}}" IN_LIST optlist)
        message(FATAL_ERROR "$CACHE{OPTION_${optname}} must have a value among ${optlist}")
    endif()
endfunction()

option_add_list(ENABLE_FILTERGRAPH "Enable filtergraph expansion" "wordexp" "none;wordexp;builtin")

if(NOT "${OPTION_ENABLE_FILTERGRAPH}" STREQUAL "none")
    set(ENABLE_FILTERGRAPH 1)
    set(REAL_FILTERGRAPH "${OPTION_ENABLE_FILTERGRAPH}")
endif()

if("${OPTION_ENABLE_FILTERGRAPH}" STREQUAL "wordexp")
    CHECK_INCLUDE_FILE_CXX("wordexp.h" HAVE_WORDEXP_H)
    if (NOT HAVE_WORDEXP_H)
        set(REAL_FILTERGRAPH "builtin")
    endif()
endif()


find_library_or_exit(LIBAVCODEC avcodec)
find_library_or_exit(LIBAVFORMAT avformat)
find_library_or_exit(LIBSWSCALE swscale)
find_library_or_exit(LIBAVUTIL avutil)

find_path_or_exit(FFMPEG_INCLUDE_DIR avcodec.h PATH_SUFFIXES libavcodec)
get_filename_component(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR} DIRECTORY CACHE)
set(FFMPEG_LIBRARY ${LIBAVCODEC} ${LIBAVFORMAT} ${LIBAVUTIL} ${LIBSWSCALE})

set(FLTK_SKIP_OPENGL ON)
set(FLTK_SKIP_FORMS ON)
set(FLTK_SKIP_IMAGES ON)
include(FindFLTK)

if (NOT FLTK_FOUND)
    message(FATAL_ERROR "FLTK was not found")
endif()

fltk_wrap_ui(miniresizer miniresizer.fld)

set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")

configure_file(config.h.in config.h)

set(SOURCES miniresizer_core.cpp)

add_library(fltk INTERFACE)
target_include_directories(fltk INTERFACE "${FLTK_INCLUDE_DIR}")
target_link_libraries(fltk INTERFACE "${FLTK_LIBRARY}")

add_library(fltk_ui OBJECT ${miniresizer_FLTK_UI_SRCS})
target_link_libraries(fltk_ui PUBLIC fltk)

add_library(ffmpeg INTERFACE)
target_include_directories(ffmpeg INTERFACE "${FFMPEG_INCLUDE_DIR}")
target_link_libraries(ffmpeg INTERFACE "${FFMPEG_LIBRARY}")
target_compile_options(ffmpeg INTERFACE -Wno-deprecated-declarations)

add_executable(miniresizer ${SOURCES})
target_link_libraries(miniresizer PRIVATE ffmpeg fltk fltk_ui)

if(WIN32)
    #find_program_or_exit(WINDRES NAMES windres $<ARCH:${ARCH}-windres>)
    set(windres_names windres)
    if(ARCH)
        list(APPEND windres_names ${ARCH}-windres)
    endif()
    find_program_or_exit(WINDRES NAMES ${windres_names})

    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/miniresizer.res
        MAIN_DEPENDENCY res/win/winres.rc
        COMMAND ${WINDRES} -o miniresizer.res -i res/win/winres.rc
        VERBATIM)

    target_link_libraries(miniresizer PRIVATE ${CMAKE_BINARY_DIR}/miniresizer.res)
else()
    # Copy icons and desktop file
    foreach(size 16x16 24x24 32x32 48x48 64x64 72x72 96x96 128x128)
        install(DIRECTORY res/icons/${size} DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor)
    endforeach()
    install(FILES res/desktop/miniresizer.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

install(TARGETS miniresizer)

message("
=====================================================
    Configuration summary:

    Filtegraph expansion: ${REAL_FILTERGRAPH}
=====================================================
")
