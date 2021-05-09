#include "RectangleBounds.hpp"

RectangleBounds::RectangleBounds(sf::RectangleShape rect) {
    topLeft = rect.getPosition();
    bottomRight = sf::Vector2f(topLeft.x + rect.getSize().x, topLeft.y + rect.getSize().y);
}

RectangleBounds::RectangleBounds(sf::Vector2f position, sf::Vector2f size) {
    topLeft = position;
    bottomRight = sf::Vector2f(position.x + size.x, position.y + size.y);
}

RectangleBounds::RectangleBounds() {
    topLeft = sf::Vector2f(0.f, 0.f);
    bottomRight = sf::Vector2f(0.f, 0.f);
}
