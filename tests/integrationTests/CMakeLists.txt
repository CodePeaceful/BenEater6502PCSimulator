add_executable(miniLcdTest MiniLCDTest.cpp)

target_link_libraries(miniLcdTest
  PRIVATE
  GTest::GTest
  Components
)

add_test(lcd_gtest miniLcdTest)


add_executable(miniLcd_viaTest VIAlcdTest.cpp)

target_link_libraries(miniLcd_viaTest
  PRIVATE
  GTest::GTest
  Components
)

add_test(lcd_via_gtest miniLcd_viaTest)



add_executable(full_sytem_test FullSystemTest.cpp)

target_link_libraries(full_sytem_test
    PRIVATE
    GTest::GTest
    Components
)

add_test(full_system_gtest full_system_test)