#include "RectangleButton.hpp"



void RectangleButton::updateBounds()
{
    bounds = RectangleBounds(getPosition(), getSize());
}

bool RectangleButton::isPointingAtButton(sf::Vector2i mousePosition)
{
    if (interactable)
    {
        if (mousePosition.x >= bounds.topLeft.x && mousePosition.y >= bounds.topLeft.y)
        {
            if (mousePosition.x <= bounds.bottomRight.x && mousePosition.y <= bounds.bottomRight.y)
            {
                return true;
            }
        }
    }
    return false;
}

void RectangleButton::updateButtonCondition(AdvancedMouse ms, sf::RenderWindow &window)
{
    if (interactable)
    {
        switch (buttonType)
        {
            case ButtonType::Usual: // usual
                if (click)
                {
                    click = false;
                }
                if (isPressed)
                {
                    if (isPointingAtButton(ms.getPosition(window)))
                    {
                        if (ms.stage != PressStage::Pressed)
                        {
                            isPressed = false;
                        }
                    }
                    else
                    {
                        isPressed = false;
                    }
                }
                else
                {
                    if (ms.canPressButton)
                    {
                        if (isPointingAtButton(ms.getPosition(window)))
                        {
                            if (ms.stage == PressStage::JustPressed)
                            {
                                isPressed = true;
                                click = true;
                            }
                        }
                    }
                }
                break;

            case ButtonType::DrawButton: // rolling
                if (isPressed)
                {
                    if (isPointingAtButton(ms.getPosition(window)))
                    {
                        if (ms.stage == PressStage::NotPressed || ms.stage == PressStage::Released)
                        {
                            isPressed = false;
                        }
                    }
                    else
                    {
                        isPressed = false;
                    }
                }
                else
                {
                    if (ms.canPressButton)
                    {
                        if (isPointingAtButton(ms.getPosition(window)))
                        {
                            if (ms.stage == PressStage::JustPressed || ms.stage == PressStage::Pressed)
                            {
                                isPressed = true;
                            }
                        }
                    }
                }
                break;
            case ButtonType::ClickOnRelease: // clickOnRelease
                if (click)
                {
                    click = false;
                }
                if (isPressed)
                {
                    if (isPointingAtButton(ms.getPosition(window)))
                    {
                        if (ms.stage == PressStage::Released)
                        {
                            click = true;
                            isPressed = false;
                        }
                        else if (ms.stage == PressStage::NotPressed)
                        {
                            isPressed = false;
                        }
                    }
                    else
                    {
                        isPressed = false;
                    }
                }
                else
                {
                    if (ms.canPressButton)
                    {
                        if (isPointingAtButton(ms.getPosition(window)))
                        {
                            if (ms.stage == PressStage::JustPressed)
                            {
                                isPressed = true;
                            }
                        }
                    }
                }
                break;
                
            case ButtonType::ClickWithHold:
                if (click)
                {
                    click = false;
                }
                
                if (isPressed)
                {
                    if (isPointingAtButton(ms.getPosition(window)))
                    {
                        if (ms.stage == PressStage::Released)
                        {
                            if (!firstClickRegistered_)
                            {
                                click = true;
                            }
                            isPressed = false;
                        }
                        else if (ms.stage == PressStage::NotPressed)
                        {
                            isPressed = false;
                        }
                    }
                    else
                    {
                        isPressed = false;
                    }
                }
                else
                {
                    if (ms.canPressButton)
                    {
                        if (isPointingAtButton(ms.getPosition(window)))
                        {
                            if (ms.stage == PressStage::JustPressed)
                            {
                                isPressed = true;
                            }
                        }
                    }
                }
                break;
        }
    }
}

void RectangleButton::setTimers(int firstClickTimer, int betweenClickTimer)
{
    firstClickTimer_ = firstClickTimer;
    betweenClickTimer_ = betweenClickTimer;
}

RectangleButton::RectangleButton()
{
    setPosition(sf::Vector2f(0.f, 0.f));
    setSize(sf::Vector2f(0.f, 0.f));
    setFillColor(sf::Color::White);
}

RectangleButton::RectangleButton(sf::Vector2f position, sf::Vector2f size, sf::Color color)
{
    setPosition(position);
    setSize(size);
    setFillColor(color);
    updateBounds();
}
