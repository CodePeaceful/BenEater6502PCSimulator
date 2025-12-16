#pragma once

#include <vector>
#include <ranges>
#include <cstdint>
#include <chrono>
#include <mutex>
#include <array>
#include <optional>
#include <atomic>

namespace components
{
namespace logger
{
class DisplayedNumberLogger;
};
/// busy uses chrono timer asuming 270khz clock
/// four bit mode or any non screen equivalent mode throws exception
class MiniLCD final
{
    friend class logger::DisplayedNumberLogger;
    friend class MiniLCDRenderer;
private:
    struct TODO
    {
        bool ddr{false};
        bool read{false};
        bool clear{false};
        uint8_t data{0};
        uint8_t address{0};
    };
    mutable std::mutex ddrMutex;
    mutable std::mutex cgrMutex;
    std::optional<TODO> pendingCommand;

    // Pin References
    uint8_t& data;
    const bool& E;
    const bool& RW; // Read high / Write Low
    const bool& RS; // Register Select

    // Internals
    std::atomic<uint8_t> addressCounter{0};
    std::array<uint8_t, 80> displayDataRam;
    std::array<uint8_t, 96> characterGeneratorRam;

    // helpers
    std::atomic<uint8_t> shiftPos{0};
    std::atomic<uint8_t> curserCell;
    std::atomic<uint8_t> fourBitBuffer;
    bool curserLine1{true};
    bool curserOn;
    bool curserBlink;
    bool doShift;
    bool moveRight;
    bool eightBit{true};
    bool screenOn;
    bool cgRam;
    bool done;

    // Timing
    bool busy{false};
    std::chrono::time_point<std::chrono::high_resolution_clock> busyEndTime;

    void processPendingCommand();
    void cycleCommand();
    void cycleData();

public:
    MiniLCD(uint8_t& data, const bool& E, const bool& RW, const bool& RS);
    void cycle();
};
} // namespace components
