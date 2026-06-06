find_package(GTest REQUIRED)

add_executable(tests 
    LazySequenceTests.cpp
    TestUtils.hpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(tests PRIVATE 
    GTest::gtest
    GTest::gtest_main
    Sequences
)

include(GoogleTest)
gtest_discover_tests(tests)