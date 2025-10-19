#pragma once
#include <SFML/Graphics.hpp>

class Line{
private:
    sf::VertexArray line;
public:
    Line(int mapsize);
    void draw(sf::RenderWindow &window);
};
