#pragma once
#include "Objects.hpp"
#include <vector>

class Bullet{
public:
    Obj body;
    sf::CircleShape shape;
    int timetodie;
    bool alive;

    Bullet(sf::Vector2f pos, float angle,float size,float gun_length,float speed,int timetodie, float dmg,int type,int bhp=10);

    void update();
    void draw(sf::RenderWindow &window);
};

//Loại bỏ đạn khỏi vector
void removeBullet(std::vector<Bullet> &bullets, int index);
