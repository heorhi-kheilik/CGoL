#include "Canvas.hpp"

Canvas::Canvas(sf::Rect<int> rect, int uW, int uH, AdvancedMouse& ms, sf::RenderWindow& window) {
    canvasRectangle_ = rect;
    
    unitWidth_ = uW;
    unitHeight_ = uH;
    
    horizontalAmount_ = canvasRectangle_.width / unitWidth_;
    verticalAmount_ = canvasRectangle_.height / unitHeight_;
    
    int curX = canvasRectangle_.left, curY = canvasRectangle_.top;
    drawingCanvas.resize(horizontalAmount_, std::vector<sf::RectangleShape> (verticalAmount_));
    for (int i = 0; i < drawingCanvas.size(); i++) {
        for (int j = 0; j < drawingCanvas[i].size(); j++) {
            drawingCanvas[i][j].setSize(sf::Vector2f(unitWidth_, unitHeight_));
            drawingCanvas[i][j].setPosition(sf::Vector2f(curX, curY));
            drawingCanvas[i][j].setFillColor(sf::Color::Black);
            curY += unitHeight_;
        }
        curY = canvasRectangle_.top;
        curX += unitWidth_;
    }
    
    previousPosition_ = ms.getPosition(window);
    previousPosition_.x = ((previousPosition_.x - canvasRectangle_.left) / unitWidth_);
    previousPosition_.y = ((previousPosition_.y - canvasRectangle_.top) / unitHeight_);
}

Canvas::Canvas(sf::Rect<float> rect, int uW, int uH, AdvancedMouse& ms, sf::RenderWindow& window) {
    canvasRectangle_.left = round(rect.left);
    canvasRectangle_.top = round(rect.top);
    canvasRectangle_.width = round(rect.width);
    canvasRectangle_.height = round(rect.height);
    
    unitWidth_ = uW;
    unitHeight_ = uH;
    
    horizontalAmount_ = canvasRectangle_.width / unitWidth_;
    verticalAmount_ = canvasRectangle_.height / unitHeight_;
    
    int curX = canvasRectangle_.left, curY = canvasRectangle_.top;
    drawingCanvas.resize(horizontalAmount_, std::vector<sf::RectangleShape> (verticalAmount_));
    for (int i = 0; i < drawingCanvas.size(); i++) {
        for (int j = 0; j < drawingCanvas[i].size(); j++) {
            drawingCanvas[i][j].setSize(sf::Vector2f(unitWidth_, unitHeight_));
            drawingCanvas[i][j].setPosition(sf::Vector2f(curX, curY));
            drawingCanvas[i][j].setFillColor(sf::Color::Black);
            curY += unitHeight_;
        }
        curY = canvasRectangle_.top;
        curX += unitWidth_;
    }
    
    previousPosition_ = ms.getPosition(window);
    previousPosition_.x = ((previousPosition_.x - canvasRectangle_.left) / unitWidth_);
    previousPosition_.y = ((previousPosition_.y - canvasRectangle_.top) / unitHeight_);
}

Canvas::Canvas(int left, int top, int width, int height, int uW, int uH, AdvancedMouse& ms, sf::RenderWindow& window) {
    canvasRectangle_.left = left;
    canvasRectangle_.top = top;
    canvasRectangle_.width = width;
    canvasRectangle_.height = height;
    
    unitWidth_ = uW;
    unitHeight_ = uH;
    
    horizontalAmount_ = canvasRectangle_.width / unitWidth_;
    verticalAmount_ = canvasRectangle_.height / unitHeight_;
    
    int curX = canvasRectangle_.left, curY = canvasRectangle_.top;
    drawingCanvas.resize(horizontalAmount_, std::vector<sf::RectangleShape> (verticalAmount_));
    for (int i = 0; i < drawingCanvas.size(); i++) {
        for (int j = 0; j < drawingCanvas[i].size(); j++) {
            drawingCanvas[i][j].setSize(sf::Vector2f(unitWidth_, unitHeight_));
            drawingCanvas[i][j].setPosition(sf::Vector2f(curX, curY));
            drawingCanvas[i][j].setFillColor(sf::Color::Black);
            curY += unitHeight_;
        }
        curY = canvasRectangle_.top;
        curX += unitWidth_;
    }
    
    previousPosition_ = ms.getPosition(window);
    previousPosition_.x = ((previousPosition_.x - canvasRectangle_.left) / unitWidth_);
    previousPosition_.y = ((previousPosition_.y - canvasRectangle_.top) / unitHeight_);
}

std::vector< std::pair<int, int> > Canvas::findAffectedUnits_(int x0, int y0, int x1, int y1) {
    if (x0 == x1 && y0 == y1) return {};
    
    bool isXFirst = true;
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    float factor = ((y1 - y0) / (float)(x1 - x0));
    
    if (factor > 1 || factor < -1) {
        isXFirst = false;
        std::swap(x0, y0);
        std::swap(x1, y1);
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        factor = ((y1 - y0) / (float)(x1 - x0));
    }
    std::vector< std::pair<int, int> > affectedUnits;
    float curSecond = y0;
    
    if (isXFirst) {
        for (int x = x0; x <= x1; x++) {
            int tempY = (int)round(curSecond);
            if (x >= 0 && x < horizontalAmount_ && tempY >= 0 && tempY < verticalAmount_)
                affectedUnits.push_back(std::make_pair(x, tempY));
            curSecond += factor;
        }
    } else {
        for (int x = x0; x <= x1; x++) {
            int tempY = (int)round(curSecond);
            if (tempY >= 0 && tempY < horizontalAmount_ && x >= 0 && x < verticalAmount_)
                affectedUnits.push_back(std::make_pair(tempY, x));
            curSecond += factor;
        }
    }
    
    return affectedUnits;
}

void Canvas::updateCondition(AdvancedMouse& ms, sf::RenderWindow& window) {
    sf::Vector2i currentPosition = ms.getPosition(window);
    
    currentPosition.x = ((currentPosition.x - canvasRectangle_.left) / unitWidth_);
    currentPosition.y = ((currentPosition.y - canvasRectangle_.top) / unitHeight_);
    
    switch (ms.stage) {
        case PressStage::NotPressed:
            changedUnits.clear();
            break;
            
        case PressStage::JustPressed:
            changedUnits.clear();
            if (isActive && ms.canPressButton)
                if (currentPosition.x >= 0 && currentPosition.x < horizontalAmount_ &&
                    currentPosition.y >= 0 && currentPosition.y < verticalAmount_)
                    changedUnits.push_back(std::make_pair(currentPosition.x, currentPosition.y));
            break;
            
        case PressStage::Pressed:
            if (isActive && ms.canPressButton)
                changedUnits = findAffectedUnits_(previousPosition_.x, previousPosition_.y, currentPosition.x, currentPosition.y);
            break;
            
        case PressStage::Released:
            changedUnits.clear();
            break;
    }
    
    previousPosition_ = currentPosition;
}

void Canvas::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (int i = 0; i < horizontalAmount_; i++)
        for (int j = 0; j < verticalAmount_; j++)
            target.draw(drawingCanvas[i][j]);
}
