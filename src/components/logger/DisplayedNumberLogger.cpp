#include "logger/DisplayedNumberLogger.hpp"

namespace components
{
namespace logger
{
uint32_t DisplayedNumberLogger::getDisplayedNumber() const {
    uint32_t number{0};
    auto displayDataRam = lcd.displayDataRam;
    for (std::size_t i = 0; i < 16; ++i) {
        char c = displayDataRam[i];
        if (c >= '0' && c <= '9') {
            number = number * 10 + (c - '0');
        }
        else {
            break;
        }
    }
    return number;
}
} // namespace logger
} // namespace components