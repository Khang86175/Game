#pragma once
#include "Objects.hpp"

class HealthBar{
private:
    sf::RectangleShape background;
    sf::RectangleShape fill;
    float offset;
    sf::Vector2f size;
public:
    HealthBar(float width, float height, float offset);
    void update(Obj &body);
    void draw(sf::RenderWindow& window);
    void setOffSet(float new_offset);
};