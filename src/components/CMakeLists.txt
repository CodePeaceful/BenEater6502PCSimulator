add_library(Components STATIC
    Cpu.cpp
    Eeprom32k.cpp
    MiniLCD.cpp
    MiniLCDRenderer.cpp
    VersatileInterfaceAdapter.cpp
    Ram32k.cpp
    logger/CpuLogger.cpp
    logger/RamLogger.cpp
    logger/VIALogger.cpp
    logger/DisplayedNumberLogger.cpp
)

target_link_libraries(Components sfml-graphics)
target_include_directories(Components PUBLIC include ${SFML_3.0.0_SOURCE_DIR}/include) 