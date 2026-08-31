cmake_minimum_required(VERSION 3.16)
project(quant-review-demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(quant_review
  src/parse_trade_log.cpp
  src/validate_trade_log.cpp
  src/aggregate_trade_log.cpp
  src/load_trade_log.cpp
)
target_include_directories(quant_review PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(quant_review_tests
  tests/test_parse_trade_log.cpp
  tests/test_validate_trade_log.cpp
  tests/test_aggregate_trade_log.cpp
  tests/test_load_trade_log.cpp
  tests/test_gate_trade_log.cpp
)
target_link_libraries(quant_review_tests
  PRIVATE quant_review GTest::gtest_main
)
target_compile_definitions(quant_review_tests PRIVATE
  QUANT_REVIEW_DATA_DIR="${CMAKE_SOURCE_DIR}/data"
)
include(GoogleTest)
gtest_discover_tests(quant_review_tests)
