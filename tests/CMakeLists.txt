include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(cpuTest cpuTest.cpp)

target_link_libraries(cpuTest
 PRIVATE
  GTest::GTest
  Components
  )

add_test(cpu_gtest cpuTest)