add_library(Components STATIC
    Cpu.cpp
    Eeprom32k.cpp
    MiniLCD.cpp
    VersatileInterfaceAdapter.cpp
    Ram32k.cpp
)

target_link_libraries(Components sfml-graphics)
target_include_directories(Components PUBLIC include ${SFML_3.0.0_SOURCE_DIR}/include) 