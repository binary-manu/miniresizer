set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

include(CheckIncludeFileCXX)
include(GNUInstallDirs)
include(Options)

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

find_package(FFMPEG MODULE REQUIRED COMPONENTS avformat avutil swscale avcodec)

set(FLTK_SKIP_OPENGL ON)
set(FLTK_SKIP_FORMS ON)
set(FLTK_SKIP_IMAGES ON)
include(FindFLTK)

if (NOT FLTK_FOUND)
    message(FATAL_ERROR "FLTK was not found")
endif()

fltk_wrap_ui(miniresizer preview.fld resize.fld)

# The package name must be in CamelCase, becuase it is used to name the FLTK
# preference file, and until now it was typed that way.
set(PACKAGE_NAME "MiniResizer")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PROJECT_VERSION}")

set(SOURCES
    Common.cpp
    PreviewWindow.cpp
    ResizeWindow.cpp
    RGBFrameReader.cpp
    Controller.cpp
    main.cpp
)

add_library(fltk_lib INTERFACE)
target_include_directories(fltk_lib INTERFACE "${FLTK_INCLUDE_DIR}")
target_link_libraries(fltk_lib INTERFACE "${FLTK_LIBRARY}")

add_library(fltk_ui OBJECT ${miniresizer_FLTK_UI_SRCS})

include(FFMPEGNewAVCodecAPI)
include(FFMPEGCallAVRegisterAll)

configure_file(config.h.in config.h)

add_executable(miniresizer ${SOURCES} $<TARGET_OBJECTS:fltk_ui>)
target_link_libraries(miniresizer PRIVATE
    FFMPEG_avcodec FFMPEG_avformat FFMPEG_swscale FFMPEG_avutil
    fltk_lib
)

if(WIN32)
    set(windres_names windres)
    if(ARCH)
        list(APPEND windres_names ${ARCH}-windres)
    endif()
    find_program_or_exit(WINDRES NAMES ${windres_names})
    set(CMAKE_RC_COMPILER ${WINDRES} CACHE PATH "Windows RC compiler" FORCE)

    target_sources(miniresizer PRIVATE res/win/winres.rc)
else()
    # Copy icons and desktop file
    foreach(size 16x16 24x24 32x32 64x64 128x128)
        install(DIRECTORY res/icons/${size} DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor)
    endforeach()
    install(FILES res/desktop/miniresizer.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

install(TARGETS miniresizer RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

message("
=====================================================
    Configuration summary:

    Filtegraph expansion: ${REAL_FILTERGRAPH}
=====================================================
")
