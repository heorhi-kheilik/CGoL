#include "AdvancedMouse.hpp"
#include <SFML/Graphics.hpp>



bool AdvancedMouse::isInWindow(sf::RenderWindow &window)
{
    sf::Vector2i mousePosition = getPosition(window);
    if (mousePosition.x >= 0 && mousePosition.y >= 0)
    {
        if (mousePosition.x <= window.getSize().x && mousePosition.y <= window.getSize().y)
        {
            return true;
        }
    }
    return false;
}

void AdvancedMouse::updateCondition(sf::RenderWindow &window)
{
    switch (stage)
    {
        case PressStage::NotPressed:
            if (isButtonPressed(sf::Mouse::Button::Left))
            {
                stage = PressStage::JustPressed;
            }
            break;

        case PressStage::JustPressed:
            if (isButtonPressed(sf::Mouse::Button::Left))
            {
                stage = PressStage::Pressed;
            }
            else
            {
                stage = PressStage::Released;
            }
            break;

        case PressStage::Pressed:
            if (!isButtonPressed(sf::Mouse::Button::Left))
            {
                stage = PressStage::Released;
            }
            break;

        case PressStage::Released:
            if (isButtonPressed(sf::Mouse::Button::Left))
            {
                stage = PressStage::JustPressed;
            }
            else
            {
                stage = PressStage::NotPressed;
            }
            break;
    }

    if (canPressButton)
    {
        if (stage == PressStage::JustPressed && !isInWindow(window))
        {
            canPressButton = false;
        }
    }
    else
    {
        if (stage == PressStage::Released || stage == PressStage::NotPressed)
        {
            canPressButton = true;
        }
    }
}
