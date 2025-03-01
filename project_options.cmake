include(${PROJECT_SOURCE_DIR}/cmake/warnings.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/sanitizers.cmake)

# the following function was taken from:
# https://github.com/cpp-best-practices/cmake_template/blob/main/ProjectOptions.cmake
macro(check_sanitizer_support)
    if ((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
        set(supports_ubsan ON)
    else ()
        set(supports_ubsan OFF)
    endif ()

    if ((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
        set(supports_asan OFF)
    else ()
        set(supports_asan ON)
    endif ()
endmacro()

if (PROJECT_IS_TOP_LEVEL)
    option(iubs2k_warnings_as_errors "Treat warnings as errors" ON)
    option(iubs2k_enable_undefined_behavior_sanitizer "Enable undefined behavior sanitizer" ${supports_ubsan})
    option(iubs2k_enable_address_sanitizer "Enable address sanitizer" ${supports_asan})
    option(iubs2k_build_tests "Build unit tests" ON)
else ()
    option(iubs2k_warnings_as_errors "Treat warnings as errors" OFF)
    option(iubs2k_enable_undefined_behavior_sanitizer "Enable undefined behavior sanitizer" OFF)
    option(iubs2k_enable_address_sanitizer "Enable address sanitizer" OFF)
    option(iubs2k_build_tests "Build unit tests" OFF)
endif ()

add_library(iubs2k_warnings INTERFACE)
iubs2k_set_warnings(iubs2k_warnings ${iubs2k_warnings_as_errors})

add_library(iubs2k_sanitizers INTERFACE)
iubs2k_enable_sanitizers(iubs2k_sanitizers ${iubs2k_enable_address_sanitizer} ${iubs2k_enable_undefined_behavior_sanitizer})

add_library(iubs2k_project_options INTERFACE)
target_link_libraries(iubs2k_project_options
        INTERFACE iubs2k_warnings
        INTERFACE iubs2k_sanitizers
)
