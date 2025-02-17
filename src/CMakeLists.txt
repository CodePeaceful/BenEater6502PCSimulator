add_subdirectory(assembler)
add_subdirectory(components)

add_executable(PcSimulation
    main.cpp
    Computer.cpp
)

target_link_libraries(PcSimulation Assembler Components)
