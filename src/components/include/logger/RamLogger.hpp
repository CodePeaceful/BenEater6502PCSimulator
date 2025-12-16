#pragma once
#include <format>

#include "Ram32k.hpp"

namespace components
{
namespace logger
{
class RamLogger
{
private:
    const Ram32k& ram;
    const uint16_t logStart;
    const uint16_t logSize;
public:
    RamLogger(const Ram32k& _ram, uint16_t _logStart, uint16_t _logSize) : ram(_ram), logStart(_logStart), logSize(_logSize) { }
    std::string logState();
};
} // namespace logger
} // namespace components