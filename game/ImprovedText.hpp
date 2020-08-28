#ifndef ImprovedText_hpp
#define ImprovedText_hpp

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <math.h>

class ImprovedText : public sf::Drawable
{
private:
    
    std::string text_;
    sf::Font font_;
    unsigned int characterSize_;
    sf::Color textColor_;
    sf::Texture bitmap_;
    
    sf::Rect<int> alignRectangle_;
    
    sf::Vector2f currentPosition_;
    
    std::unordered_map<char, sf::Glyph> glyphMap_;
    std::vector<sf::Sprite> spriteVector_;
    
    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    
public:
    void calculateStartPosition();
    
    void resetText();
    
    ImprovedText(std::string str, sf::Font fnt, unsigned int sz, sf::Color clr, sf::Rect<float> alignRect);
    
    ImprovedText(std::string str, sf::Font fnt, unsigned int sz, sf::Color clr, sf::Rect<int> alignRect);
    
    void setSize(unsigned int sz);
    
    void setFont(sf::Font f);
    
    void setText(std::string s);
    
    void setTextColor(sf::Color clr);
};



#endif /* ImprovedText_hpp */
