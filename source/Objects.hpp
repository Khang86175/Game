#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

float hypot2(const sf::Vector2f &x);

// Chỉ số + thêm
struct Stats{
    int hp_regen = 0;
    int maxhp = 0;
    int body_dmg = 0;
    int bullet_speed = 0;
    int bullet_penetration = 0;
    int bullet_dmg = 0;
    int reload = 0;
    int move_speed = 0;

    int maxLevel = 7;
};

//Base class cho mọi đối tượng trong game
class Obj{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mapsize;
    float hitbox_r;
    float hp,maxhp,hp_regen,body_dmg;

    Obj(float x, float y, float mapsize, float size, float maxhp, float hp_regen, float body_dmg);

    void update();
    float Getx();
    float Gety();
    void setMaxHp(float new_maxhp);
    void setHpRegen(float new_hp_regen);
    void setBodyDmg(float new_body_dmg);
    void handleCollisionWith(Obj& other,bool instant=true,bool bounce = true,float bounceScale =1,int dmging=1);
    bool isCollidingWith(const Obj& other) const;
};