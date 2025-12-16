#pragma once
#include <string>

#include "MiniLCD.hpp"

namespace components
{
namespace logger
{
class DisplayedNumberLogger
{
private:
    const MiniLCD& lcd;
public:
    DisplayedNumberLogger(const MiniLCD& _lcd) : lcd(_lcd) { }
    uint32_t getDisplayedNumber() const;
};
} // namespace logger
} // namespace components