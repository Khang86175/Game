#pragma once
#include "Objects.hpp"
#include "Bullet.hpp"
#include "Globals.hpp"
#include <algorithm>

class Cannon{
public:
    int delay, reload;
    float angle;
    sf::Vector2f position;
    sf::RectangleShape gun;
    sf::Vector2f size;

    Cannon(float x, float y,float size, float reload);
    void update(sf::Vector2f pos, int angle);
    void DrawGun(sf::RenderWindow &window);
    void setReload(int new_reload);
};

class TankBasic{
public:
    Cannon gun;
    TankBasic(float x, float y, float size);
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape);
    void update(sf::Vector2f pos, int angle);
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type);
};
class TankTwin{
public:
    Cannon gun1, gun2;
    int gunToggle; // 0:gun1 , 1:gun2
    float gunOffSet; // Kc giữa 2 nòng

    TankTwin(float x, float y, float size);
    void update(sf::Vector2f pos, float angle);
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type);
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape);
};
class MyTank{
public:
    Obj body;
    float friction;
    float acceleration = 0.25;
    sf::CircleShape bodyShape;
    int xp_base, xp_to_lv_up;
    int score;
    int level;
    int statPoint;
    Stats stats; 

    int tankType; // 0:basic, 1:twin
    TankBasic *tankBasic;
    TankTwin *tankTwin;

    // Base stats
    float base_hp_regen = 0.1f;
    float base_maxhp = 100;
    float base_body_dmg = 2;
    float base_bullet_speed = 4;
    int base_bullet_life = fps*5;
    float base_bullet_dmg = 3;
    int base_reload = 60;
    float base_acceleration = 0.25;

    MyTank(float x, float y, float size,float mapsize);
    ~MyTank();
    void update(int angle);
    void applyStats();
    void addScore(int point);
    void levelUp();
    float getBulletSpeed();
    int getBulletLife();
    float getBulletDamage();
    void Drawtank(sf::RenderWindow &window);
    void moveX(float dx);
    void moveY(float dy);
    bool upgradeStat(int statIndex);
    void shoot(std::vector<Bullet> &bullets, int angle);
    int getTankType();
    void reset(float x, float y, float size, float mapsize);
};

class EnemyTank{
public:
    Obj body;
    float friction;
    float acceleration;
    sf::CircleShape bodyShape;
    Stats stats; 
    int tankType;

    TankBasic *tankBasic;
    TankTwin *tankTwin;

    float base_hp_regen = 0.1f;
    float base_maxhp = 100;
    float base_body_dmg = 2;
    float base_bullet_speed = 4;
    int base_bullet_life = fps*5;
    float base_bullet_dmg = 3;
    int base_reload = 60;
    float base_acceleration = 0.25;

    EnemyTank(float x, float y, float size,float mapsize,int tanktype);
    ~EnemyTank();
    void update(int angle);
    float getBulletSpeed();
    int getBulletLife();
    float getBulletDamage();
    void Drawtank(sf::RenderWindow &window);
    void shoot(std::vector<Bullet> &bullets, int angle);
    int getTankType();
};
