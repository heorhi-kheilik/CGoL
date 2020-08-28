#include "ImprovedText.hpp"



void ImprovedText::calculateStartPosition()
{
    int textWidth = (text_.size() - 1);
    int textHeight = font_.getGlyph('x', characterSize_, false).bounds.height;
    
    for (int i = 0; i < text_.size(); i++)
    {
        if (text_[i] == ' ')
        {
            textWidth += round(characterSize_ / 4);
        }
        else
        {
            textWidth += glyphMap_[text_[i]].bounds.width;
        }
    }
    
    currentPosition_.x = alignRectangle_.left + ceil((alignRectangle_.width - textWidth) / 2);
    currentPosition_.y = alignRectangle_.top + ceil((alignRectangle_.height + textHeight) / 2);
}

void ImprovedText::resetText()
{
    glyphMap_.clear();
    spriteVector_.clear();
    
    std::unordered_set<char> tempSet;
    std::unordered_set<char>::iterator it;
    for (int i = 0; i < text_.size(); i++)
    {
        tempSet.insert(text_[i]);
    }
    for (it = tempSet.begin(); it != tempSet.end(); it++)
    {
        glyphMap_[*it] = font_.getGlyph(*it, characterSize_, false);
    }
    
    bitmap_ = font_.getTexture(characterSize_);
    
    calculateStartPosition();
    
    for (int i = 0; i < text_.size(); i++)
    {
        if (text_[i] == ' ')
        {
            currentPosition_.x += round(characterSize_ / 4);
            spriteVector_.push_back(sf::Sprite());
        }
        else
        {
            sf::Sprite tempSprite = sf::Sprite(bitmap_, glyphMap_[text_[i]].textureRect);
            tempSprite.setPosition(sf::Vector2f(currentPosition_.x, currentPosition_.y + glyphMap_[text_[i]].bounds.top));
            tempSprite.setColor(textColor_);
            spriteVector_.push_back(tempSprite);
            
            currentPosition_.x += tempSprite.getGlobalBounds().width + 1;
            if (i != text_.size() - 1)
            {
                currentPosition_.x += font_.getKerning(text_[i], text_[i + 1], characterSize_);
            }
        }
    }
}

ImprovedText::ImprovedText(std::string str, sf::Font fnt, unsigned int sz, sf::Color clr, sf::Rect<float> alignRect)
{
    text_ = str;
    font_ = fnt;
    characterSize_ = sz;
    textColor_ = clr;
    
    alignRectangle_.left = alignRect.left;
    alignRectangle_.top = alignRect.top;
    alignRectangle_.width = alignRect.width;
    alignRectangle_.height = alignRect.height;
    
    resetText();
}

ImprovedText::ImprovedText(std::string str, sf::Font fnt, unsigned int sz, sf::Color clr, sf::Rect<int> alignRect)
{
    text_ = str;
    font_ = fnt;
    characterSize_ = sz;
    textColor_ = clr;
    
    alignRectangle_ = alignRect;
    
    resetText();
}

void ImprovedText::setSize(unsigned int sz)
{
    characterSize_ = sz;
    resetText();
}

void ImprovedText::setFont(sf::Font f)
{
    font_ = f;
    resetText();
}

void ImprovedText::setText(std::string s)
{
    text_ = s;
    resetText();
}

void ImprovedText::setTextColor(sf::Color clr)
{
    textColor_ = clr;
    for (int i = 0; i < spriteVector_.size(); i++)
    {
        spriteVector_[i].setColor(textColor_);
    }
}

void ImprovedText::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    for (int i = 0; i < text_.size(); i++)
    {
        if (text_[i] != ' ')
        {
            target.draw(spriteVector_[i]);
        }
    }
}
