include(CheckSymbolExists)

if(NOT DEFINED FFMPEG_CALL_AV_REGISTER_ALL)
    message(STATUS "Checking if we should call av_register_all")
    get_target_property(CMAKE_REQUIRED_INCLUDES FFMPEG_avformat INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(CMAKE_REQUIRED_LIBRARIES FFMPEG_avformat INTERFACE_LINK_LIBRARIES)

    check_symbol_exists("av_register_all" "libavformat/avformat.h" FFMPEG_CALL_AV_REGISTER_ALL)

    if(FFMPEG_CALL_AV_REGISTER_ALL) 
        message(STATUS "Checking if we should call av_register_all - yes")
    else()
        message(STATUS "Checking if we should call av_register_all - no")
    endif()
endif()
