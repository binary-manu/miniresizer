set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

include(BuildType)
include(CheckIncludeFileCXX)
include(GNUInstallDirs)
include(Options)

option_add_list(ENABLE_FILTERGRAPH "Enable filtergraph expansion" "wordexp" "none;wordexp;builtin")

set(REAL_FILTERGRAPH "${OPTION_ENABLE_FILTERGRAPH}")

if(NOT "${OPTION_ENABLE_FILTERGRAPH}" STREQUAL "none")
    # This variable gets replaced into config.h.in
    set(ENABLE_FILTERGRAPH 1)
endif()

if("${OPTION_ENABLE_FILTERGRAPH}" STREQUAL "wordexp")
    CHECK_INCLUDE_FILE_CXX("wordexp.h" HAVE_WORDEXP_H)
    if (NOT HAVE_WORDEXP_H)
        message(NOTICE "wordexp support is not available, falling back on builtin filtergraph expansion")
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

fltk_wrap_ui(miniresizer preview.fld resize.fld about.fld)

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
target_include_directories(fltk_ui PRIVATE "${FLTK_INCLUDE_DIR}")

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
    find_program(WINDRES NAMES ${windres_names})
    if(NOT WINDRES)
        message(FATAL_ERROR "Cannot find the Windows Resource Compiler")
    endif()
    set(CMAKE_RC_COMPILER ${WINDRES} CACHE PATH "Windows RC compiler" FORCE)

    target_sources(miniresizer PRIVATE res/win/winres.rc)
else()
    # Copy icons and desktop file
    foreach(size 16x16 24x24 32x32 64x64 128x128)
        install(FILES res/icons/${size}/miniresizer.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${size}/apps/)
    endforeach()
    install(FILES res/desktop/miniresizer.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

install(TARGETS miniresizer RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

message(STATUS "Filtergraph implementation selected: ${REAL_FILTERGRAPH}")
