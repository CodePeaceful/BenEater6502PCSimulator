#include "logger/CpuLogger.hpp"

namespace components
{
namespace logger
{
std::string CpuLogger::logState() {
    if (cpu.TCU == 0) {
        return std::format("PC: {:04X}  IR: {:02X}  A: {:02X}  X: {:02X}  Y: {:02X}  SP: {:02X}  P: {:02X}",
            cpu.programCounter,
            cpu.IR,
            cpu.A,
            cpu.X,
            cpu.Y,
            cpu.stackPointer,
            cpu.processorStatus);
    }
    return "";
}
} // namespace logger
} // namespace components