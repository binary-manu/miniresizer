include(CheckCXXSourceRuns)

if(NOT DEFINED USE_NEW_AVCODEC_API)
    message(STATUS "Checking if we should use the new packet decoding API")
    get_target_property(CMAKE_REQUIRED_INCLUDES FFMPEG_avcodec INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(CMAKE_REQUIRED_LIBRARIES FFMPEG_avcodec INTERFACE_LINK_LIBRARIES)
    check_cxx_source_runs(
        [[
            extern "C" {
                #include <libavcodec/version.h>
            }
            int main() {
                return !(LIBAVCODEC_VERSION_MAJOR >= 57 && LIBAVCODEC_VERSION_MINOR >= 48);
            }
        ]]

        USE_NEW_AVCODEC_API
    )
    if(USE_NEW_AVCODEC_API) 
        message(STATUS "Checking if we should use the new packet decoding API - yes")
    else()
        message(STATUS "Checking if we should use the new packet decoding API - no")
    endif()
endif()
