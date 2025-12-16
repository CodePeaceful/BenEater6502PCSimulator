# add_subdirectory(assembler) assembler has been replaced by external tool
add_subdirectory(components)

add_executable(PcSimulation
    main.cpp
    Computer.cpp
    ComputerLogger.cpp
)

target_link_libraries(PcSimulation Components)

target_compile_definitions(PcSimulation PUBLIC logging)
