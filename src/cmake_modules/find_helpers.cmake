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
    else()
        message(STATUS "Found ${ARGV0}: ${${ARGV0}}")
    endif()
endfunction()

#[[
    Calls find_program forwarding all arguments to it.
    If the specified program is not found, stop CMake with a fatal error.
#]]
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
    else()
        message(STATUS "Found ${ARGV0}: ${${ARGV0}}")
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
    else()
        message(STATUS "Found ${ARGV0}: ${${ARGV0}}")
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
