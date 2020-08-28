#ifndef RectangleButton_hpp
#define RectangleButton_hpp

#include <SFML/Graphics.hpp>
#include <chrono>
#include "AdvancedMouse.hpp"
#include "RectangleBounds.hpp"

enum ButtonType
{
    Usual,
    DrawButton,
    ClickOnRelease,
    ClickWithHold
};

class RectangleButton : public sf::RectangleShape
{
private:
    
    std::chrono::steady_clock::time_point buttonTimePoint_;
    int firstClickTimer_ = 100;
    int betweenClickTimer_ = 50;
    bool firstClickRegistered_ = false;
    
public:
    
    RectangleBounds bounds;
    
    bool isPressed = false;
    bool click = false;
    bool interactable = false;
    
    
    
    ButtonType buttonType = ButtonType::Usual;
    
    void updateBounds();
    
    bool isPointingAtButton(sf::Vector2i mousePosition);
    
    void updateButtonCondition(AdvancedMouse ms, sf::RenderWindow &window);
    
    void setTimers(int firstClickTimer, int betweenClickTimer);
    
    RectangleButton();
    
    RectangleButton(sf::Vector2f position, sf::Vector2f size, sf::Color color);
};

#endif /* RectangleButton_hpp */
