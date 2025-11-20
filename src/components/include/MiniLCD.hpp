#pragma once

#include <vector>
#include <ranges>
#include <cstdint>
#include <chrono>

#include <SFML/Graphics.hpp>

/// busy uses chrono timer asuming 270khz clock
/// four bit mode or any non screen equivalent mode throws exception
class MiniLCD final
{
private:
    // Pin References
    uint8_t& data;
    const bool& E;
    const bool& RW; // Read high / Write Low
    const bool& RS; // Register Select

    // Visuals
    sf::RectangleShape background;
    std::vector<sf::Sprite> topRow{std::views::repeat(sf::Sprite(texture)) | std::views::take(16) | std::ranges::to<std::vector<sf::Sprite>>()};
    std::vector<sf::Sprite> bottomRow{std::views::repeat(sf::Sprite(texture)) | std::views::take(16) | std::ranges::to<std::vector<sf::Sprite>>()};
    sf::RectangleShape curser;


    // Internals
    uint8_t addressCounter{0};
    std::array<uint8_t, 80> displayDataRam;
    std::array<volatile uint8_t, 96> characterGeneratorRam;

    // helpers
    sf::Texture texture;
    uint8_t shiftPos{0};
    uint8_t curserCell;
    uint8_t fourBitBuffer;
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


    void cycleCommand();
    void cycleData();

    // Magic 
        // to be run is architectural thread
    void updateTexture();
    void updateDisplay();

public:
    MiniLCD(uint8_t& data, const bool& E, const bool& RW, const bool& RS);
    void draw(sf::RenderTarget& window);
    void move(sf::Vector2f vec);
    void cycle();
};
