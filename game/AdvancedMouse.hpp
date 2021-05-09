#ifndef AdvancedMouse_hpp
#define AdvancedMouse_hpp

#include <SFML/Graphics.hpp>

enum PressStage {
    NotPressed,
    JustPressed,
    Pressed,
    Released
};

class AdvancedMouse : public sf::Mouse {
public:
    PressStage stage = PressStage::NotPressed;
    bool canPressButton = true;
    
    bool isInWindow(sf::RenderWindow &window);
    void updateCondition(sf::RenderWindow &window);
};

#endif /* AdvancedMouse_hpp */
