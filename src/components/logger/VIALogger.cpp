#include "logger/VIALogger.hpp"

namespace components
{
namespace logger
{
std::string VIALogger::logState() {
    return std::format("VIA State: PortA: {:08B} PortB: {:02X} PortADDR: {:02X} PortBDDR: {:02X}",
        via.portA,
        via.portB,
        via.portADataDirection,
        via.portBDataDirection);
}
} // namespace logger
} // namespace components