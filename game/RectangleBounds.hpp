#ifndef RectangleBounds_hpp
#define RectangleBounds_hpp

#include <SFML/Graphics.hpp>

class RectangleBounds
{
public:
    sf::Vector2f topLeft, bottomRight;
    
    
    RectangleBounds(sf::RectangleShape rect);
    
    RectangleBounds(sf::Vector2f position, sf::Vector2f size);
    
    RectangleBounds();
};

#endif /* RectangleBounds_hpp */
