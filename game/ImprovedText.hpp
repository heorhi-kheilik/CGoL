#ifndef ImprovedText_hpp
#define ImprovedText_hpp

#include <SFML/Graphics.hpp>

class ImprovedText : public sf::Text {
private:
    sf::Rect<int> alignRectangle_;
    
    sf::Vector2f calculateStartPosition();
    sf::Vector2f calculateStartPosition(sf::Rect<int> alignRectangle);
    sf::Vector2f calculateStartPosition(sf::Rect<float> alignRectangle);
    
public:
    void setText(std::string text);
    void setAlignRectangle(sf::Rect<int> alignRectangle);
    void setAlignRectangle(sf::Rect<float> alignRectangle);
    
    ImprovedText();
    ImprovedText(std::string text, sf::Font& font, unsigned int characterSize, sf::Color color, sf::Rect<int> alignRectangle);
    ImprovedText(std::string text, sf::Font& font, unsigned int characterSize, sf::Color color, sf::Rect<float> alignRectangle);
};

#endif /* ImprovedText_hpp */
