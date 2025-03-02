include("${CMAKE_SOURCE_DIR}/cmake/CPM.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/system_link.cmake")

function(obpf_simulator_setup_dependencies)
    CPMAddPackage(
            NAME SPDLOG
            GITHUB_REPOSITORY gabime/spdlog
            VERSION 1.14.1
            OPTIONS
            "SPDLOG_BUILD_EXAMPLE OFF"
            "SPDLOG_BUILD_TESTS OFF"
            "BUILD_SHARED_LIBS OFF"
    )
    CPMAddPackage(
            NAME MAGIC_ENUM
            GITHUB_REPOSITORY Neargye/magic_enum
            VERSION 0.9.6
    )
    CPMAddPackage(
            NAME LIB2K
            GITHUB_REPOSITORY mgerhold/lib2k
            VERSION 0.1.4
    )
    CPMAddPackage(
            NAME TL_OPTIONAL
            GITHUB_REPOSITORY TartanLlama/optional
            VERSION 1.1.0
            OPTIONS
            "OPTIONAL_BUILD_PACKAGE OFF"
            "OPTIONAL_BUILD_TESTS OFF"
            "OPTIONAL_BUILD_PACKAGE_DEB OFF"
            "BUILD_SHARED_LIBS OFF"
    )
    CPMAddPackage(
            NAME TL_EXPECTED
            GITHUB_REPOSITORY TartanLlama/expected
            VERSION 1.1.0
            OPTIONS
            "OPTIONAL_BUILD_PACKAGE OFF"
            "OPTIONAL_BUILD_TESTS OFF"
            "OPTIONAL_BUILD_PACKAGE_DEB OFF"
            "BUILD_SHARED_LIBS OFF"
    )
    CPMAddPackage(
            NAME FMT
            GITHUB_REPOSITORY fmtlib/fmt
            GIT_TAG 11.1.4
            OPTIONS
            "FMT_INSTALL OFF"
            "FMT_OS OFF"
    )
    CPMAddPackage(
            NAME SFML
            GITHUB_REPOSITORY SFML/SFML
            GIT_TAG 3.0.0
    )
    CPMAddPackage(
            NAME CTRE
            GITHUB_REPOSITORY hanickadot/compile-time-regular-expressions
            VERSION 3.9.0
    )
endfunction()
