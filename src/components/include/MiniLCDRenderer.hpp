#pragma once
#include <SFML/Graphics.hpp>

#include "MiniLCD.hpp"


namespace components
{
class MiniLCDRenderer final
{
private:
    const MiniLCD& lcd;
    // Visuals
    sf::RectangleShape background;
    std::vector<sf::Sprite> topRow{std::views::repeat(sf::Sprite(texture)) | std::views::take(16) | std::ranges::to<std::vector<sf::Sprite>>()};
    std::vector<sf::Sprite> bottomRow{std::views::repeat(sf::Sprite(texture)) | std::views::take(16) | std::ranges::to<std::vector<sf::Sprite>>()};
    sf::RectangleShape curser;

    sf::Texture texture;

    // Magic 
            // to be run is architectural thread
    void updateTexture();
    void updateDisplay();

public:
    MiniLCDRenderer(const MiniLCD& _lcd);
    void draw(sf::RenderTarget& window);
    void move(sf::Vector2f vec);
};
} // namespace components