#pragma once
#include <format>

#include "VersatileInterfaceAdapter.hpp"

namespace components
{
namespace logger
{
class VIALogger
{
private:
    const VersatileInterfaceAdapter& via;
public:
    VIALogger(const VersatileInterfaceAdapter& _via) : via(_via) { }
    std::string logState();
};
} // namespace logger
} // namespace components