#include "logger/RamLogger.hpp"

#include <ranges>
#include <vector>

namespace components
{
namespace logger
{
std::string RamLogger::logState() {
    std::vector<std::string> ramSegment;
    for (auto i : std::views::iota(logStart) | std::views::take(logSize)) {
        ramSegment.push_back(std::format("{:04X}: {:02X}", i, ram.dataStore[i]));
    }
    return std::format("RAM State from {:04X} to {:04X}:\n {}", logStart, logStart + logSize - 1, ramSegment);
}
} // namespace logger
} // namespace components