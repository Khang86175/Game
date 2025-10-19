#include "World.hpp"


Line::Line(int mapsize):line(sf::PrimitiveType::Lines){
    for(int i=mapsize;i<=mapsize;i+=50){
        line.append(sf::Vertex{sf::Vector2f(i,-mapsize), sf::Color(195,195,195)});
        line.append(sf::Vertex{sf::Vector2f(i,mapsize), sf::Color(195,195,195)});
    }
    for(int i=-mapsize;i<=mapsize;i+=50){
        line.append(sf::Vertex{sf::Vector2f(-mapsize,i), sf::Color(195,195,195)});
        line.append(sf::Vertex{sf::Vector2f(mapsize,i), sf::Color(195,195,195)});
    }
}
void Line::draw(sf::RenderWindow &window){
    window.draw(line);
}
