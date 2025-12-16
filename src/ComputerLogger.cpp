#include "ComputerLogger.hpp"
#include <fstream>

void ComputerLogger::logCycle() noexcept {
    auto cpuLog = cpuLogger.logState();
    if (!cpuLog.empty()) {
        logBuffer[logIndex % logBuffer.size()] = std::format(
            "{} | {} \n{}\nDisplayed Number: {}",
            cpuLog,
            viaLogger.logState(),
            ramLogger.logState(),
            displayedNumberLogger.getDisplayedNumber()
        );
        ++logIndex;
        if (DisplayedNumber + 1 < displayedNumberLogger.getDisplayedNumber() && displayedNumberLogger.getDisplayedNumber() != DisplayedNumber + 10 && displayedNumberLogger.getDisplayedNumber() != DisplayedNumber + 100) {
            std::ofstream logFile("computer_log.txt");
            for (std::size_t i = 0; i < std::min(logIndex, logBuffer.size()); ++i) {
                logFile << logBuffer[(logIndex + i) % logBuffer.size()] << "\n\n";
            }
            logFile.close();
            throw std::runtime_error("Displayed number increased more then expected, log saved to computer_log.txt" + std::to_string(DisplayedNumber) + " -> " + std::to_string(displayedNumberLogger.getDisplayedNumber()));
        }
        DisplayedNumber = std::max(displayedNumberLogger.getDisplayedNumber(), DisplayedNumber);
    }
}