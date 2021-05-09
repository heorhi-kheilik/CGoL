#ifndef Canvas_hpp
#define Canvas_hpp

#include <math.h>
#include <SFML/Graphics.hpp>
#include "AdvancedMouse.hpp"

class Canvas : public sf::Drawable {
private:
    sf::Rect<int> canvasRectangle_;
    int unitWidth_, unitHeight_;
    int horizontalAmount_, verticalAmount_;
    sf::Vector2i previousPosition_;
    
    std::vector< std::pair<int, int> > findAffectedUnits_(int x0, int y0, int x1, int y1);
    
public:
    bool isActive = false;
    
    std::vector< std::vector<sf::RectangleShape> > drawingCanvas;
    std::vector< std::pair<int, int> > changedUnits;
    
    Canvas(sf::Rect<int> rect, int uW, int uH, AdvancedMouse& ms, sf::RenderWindow& window);
    Canvas(sf::Rect<float> rect, int uW, int uH, AdvancedMouse& ms, sf::RenderWindow& window);
    Canvas(int left, int top, int width, int height, int uW, int uH, AdvancedMouse& ms, sf::RenderWindow& window);
    
    void updateCondition(AdvancedMouse& ms, sf::RenderWindow& window);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif /* Canvas_hpp */
