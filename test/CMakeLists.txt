
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(test_utils OBJECT test_utils.cpp io/compression_utils.cpp)

enable_testing()

set(TESTS)

if (CSICS_BUILD_QUEUE)
    list(APPEND TESTS queue/spsc_queue_test.cpp)
endif()

if (CSICS_BUILD_IO)
    if (CSICS_USE_ZSTD)
        list(APPEND TESTS io/zstd_compression_test.cpp)
    endif()
    if (CSICS_USE_ZLIB)
        list(APPEND TESTS io/zlib_compression_test.cpp)
    endif()
endif()

add_executable(tests ${TESTS})
target_link_libraries(tests PRIVATE GTest::gtest_main GTest::gmock_main CSICS test_utils)

include(GoogleTest)
message(STATUS "Available tests: ${TESTS}")
gtest_discover_tests(tests)

