CPMAddPackage(
        NAME GTEST
        GITHUB_REPOSITORY google/googletest
        VERSION 1.15.2
        OPTIONS
        "INSTALL_GTEST OFF"
        "BUILD_SHARED_LIBS OFF"
)

add_executable(
        tests
        test.cpp
)
target_link_system_libraries(
        tests
        PRIVATE
        gtest_main
        gmock_main
)

include(GoogleTest)
gtest_discover_tests(tests)
