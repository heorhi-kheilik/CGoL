#include "ImprovedText.hpp"
#include <math.h>

ImprovedText::ImprovedText()
{
    
}

ImprovedText::ImprovedText(std::string text, sf::Font& font, unsigned int characterSize, sf::Color color, sf::Rect<int> alignRectangle)
{
    setString(text);
    setFont(font);
    setCharacterSize(characterSize);
    setFillColor(color);
    setAlignRectangle(alignRectangle);
    setPosition(calculateStartPosition());
    
}

ImprovedText::ImprovedText(std::string text, sf::Font& font, unsigned int characterSize, sf::Color color, sf::Rect<float> alignRectangle)
{
    setString(text);
    setFont(font);
    setCharacterSize(characterSize);
    setFillColor(color);
    setAlignRectangle(alignRectangle);
    setPosition(calculateStartPosition());
}

sf::Vector2f ImprovedText::calculateStartPosition()
{
    sf::Vector2f position;
    position.x = (alignRectangle_.left + floor((alignRectangle_.width - getLocalBounds().width) / 2) - getLocalBounds().left);
    position.y = (alignRectangle_.top + ceil((alignRectangle_.height + getFont()->getGlyph('x', getCharacterSize(), false).bounds.height) / 2) - getCharacterSize());
    return position;
}

sf::Vector2f ImprovedText::calculateStartPosition(sf::Rect<int> alignRectangle)
{
    sf::Vector2f position;
    position.x = (alignRectangle.left + floor((alignRectangle.width - getLocalBounds().width) / 2) - getLocalBounds().left);
    position.y = (alignRectangle.top + ceil((alignRectangle.height + getFont()->getGlyph('x', getCharacterSize(), false).bounds.height) / 2) - getCharacterSize());
    return position;
}

sf::Vector2f ImprovedText::calculateStartPosition(sf::Rect<float> alignRectangle)
{
    sf::Vector2f position;
    position.x = (alignRectangle.left + floor((alignRectangle.width - getLocalBounds().width) / 2));
    position.y = (alignRectangle.top + ceil((alignRectangle.height + getFont()->getGlyph('x', getCharacterSize(), false).bounds.height) / 2)) - getCharacterSize();
    return position;
}

void ImprovedText::setText(std::string text)
{
    setString(text);
    setPosition(calculateStartPosition(alignRectangle_));
}

void ImprovedText::setAlignRectangle(sf::Rect<int> alignRectangle)
{
    alignRectangle_ = alignRectangle;
}

void ImprovedText::setAlignRectangle(sf::Rect<float> alignRectangle)
{
    alignRectangle_.top = round(alignRectangle.top);
    alignRectangle_.left = round(alignRectangle.left);
    alignRectangle_.width = round(alignRectangle.width);
    alignRectangle_.height = round(alignRectangle.height);
}
