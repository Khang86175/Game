#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>
#include "Tank.hpp"

class Minimap{
public:
    sf::RectangleShape map;
    sf::CircleShape player;
    sf::CircleShape enemy;
    int Mapsize;
    Minimap(float x, float y, float size,int Mapsize);
    void Drawmap(sf::RenderWindow &window,sf::Vector2f playerpos={120,120},sf::Vector2f enemypos={120,120});
};
class XpBar{
private:
    sf::RectangleShape xpBarBG;
    sf::RectangleShape xpBar;
    sf::Text xpText, scoreText;
    std::stringstream xpss, scoress;
    sf::Vector2f pos;
    sf::Vector2f size;
public:
    XpBar(float x, float y, sf::Vector2f size, MyTank &mytank,sf::Font &font);
    void update(MyTank &mytank);
    void draw(sf::RenderWindow &window);
};
class Bar{
public:
    sf::RectangleShape barBG;
    sf::RectangleShape fill;
    sf::RectangleShape button;
    sf::Text name, idxText;
    int  index;
    sf::Color color;
   
    Bar(sf::Vector2f size, sf::Font &f, sf::Color color);
    void updatePosition( sf::Vector2f position);
};
class StatsBar{
private:
    std::vector<Bar> bars;
    sf::Text pointsText;
    sf::Vector2f position;
    bool visible;
public:
    float barWidth = 180;
    float barHeight = 18;
    float barSpacing = 8;
    StatsBar(sf::Font &f, int w, int h);
    void update(MyTank &mytank);
    int checkButtonClick(sf::Vector2i mousePos);
    void draw(sf::RenderWindow &window, sf::Font &font);
    bool isVisible();
};
