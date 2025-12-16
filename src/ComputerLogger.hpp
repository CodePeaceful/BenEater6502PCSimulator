#pragma once
#include <format>

#include "logger/RamLogger.hpp"
#include "logger/CpuLogger.hpp"
#include "logger/VIALogger.hpp"
#include "logger/DisplayedNumberLogger.hpp"

using namespace components;

class ComputerLogger
{
private:
    logger::RamLogger ramLogger;
    logger::CpuLogger cpuLogger;
    logger::VIALogger viaLogger;
    logger::DisplayedNumberLogger displayedNumberLogger;
    std::array<std::string, 256> logBuffer;
    std::size_t logIndex{0};
    uint32_t DisplayedNumber{0};
public:
    ComputerLogger(const Ram32k& ram, const Cpu& cpu, const VersatileInterfaceAdapter& via, const MiniLCD& lcd) noexcept
        : ramLogger(ram, 0x0200, 0x000C), cpuLogger(cpu), viaLogger(via), displayedNumberLogger(lcd) { }

    void logCycle() noexcept;
};

