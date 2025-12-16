#pragma once
#include <format>

#include "Cpu.hpp"

namespace components
{
namespace logger
{
class CpuLogger
{
private:
    const Cpu& cpu;
public:
    CpuLogger(const Cpu& _cpu) : cpu(_cpu) { }
    std::string logState();
};
}
}