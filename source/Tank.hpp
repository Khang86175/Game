#pragma once
#include "Objects.hpp"
#include "Bullet.hpp"
#include "Globals.hpp"
#include "HealthBar.hpp"
#include <algorithm>

enum TankType{
    BASIC,
    TWIN,
    SNIPER,
    MACHINE_GUN,
    TRIPLE,
    ASSASSIN,
    DESTROYER
};
bool canEvolveTo(TankType currentType, TankType targetType);
bool isMaxEvolution(TankType type);

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

class MachineGunCannon{
public:
    int delay, reload;
    float angle;
    sf::Vector2f position;
    sf::ConvexShape gun;
    sf::Vector2f size;

    MachineGunCannon(float x, float y,float size, float reload);
    void update(sf::Vector2f pos, int angle);
    void DrawGun(sf::RenderWindow &window);
    void setReload(int new_reload);  
};

class DestroyerCannon{
public:
    int delay, reload;
    float angle;
    sf::Vector2f position;
    sf::RectangleShape gun;
    sf::Vector2f size;

    DestroyerCannon(float x, float y,float size, float reload);
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
    void setReload(int new_reload);
};

class TankTwin{
public:
    Cannon gun1, gun2;
    float gunOffSet; // Kc giữa 2 nòng

    TankTwin(float x, float y, float size);
    void update(sf::Vector2f pos, float angle);
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type);
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape);
    void setReload(int new_reload); 
};

class TankTriple{
public:
    Cannon gun1, gun2, gun3;
    float gunOffSet;
    TankTriple(float x, float y, float size);
    void update(sf::Vector2f pos, float angle);
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type);
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape);
    void setReload(int new_reload); 
};

class TankMachineGun{
public:
    MachineGunCannon gun;
    TankMachineGun(float x, float y, float size);
    void update(sf::Vector2f pos, int angle);
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type);
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape);
    void setReload(int new_reload); 
};

class TankDestroyer {
public:
    DestroyerCannon gun;
    TankDestroyer(float x, float y, float size);
    void update(sf::Vector2f pos, int angle);
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type);
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape);
    void setReload(int new_reload); 
};

class TankSniper {
public:
    Cannon gun;
    TankSniper(float x, float y, float size);
    void update(sf::Vector2f pos, int angle);
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type);
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape);
    void setReload(int new_reload); 
};

class TankAssassin {
public:
    Cannon gun;

    TankAssassin(float x, float y, float size);
    void update(sf::Vector2f pos, int angle);
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type);
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape);
    void setReload(int new_reload); 
};


class MyTank{
public:
    Obj body;
    HealthBar hp_bar;
    float friction;
    float acceleration = 0.25;
    sf::CircleShape bodyShape;
    int xp_base, xp_to_lv_up;
    int score;
    int level;
    int statPoint;
    Stats stats; 

    TankType tankType;

    TankBasic *tankBasic;
    TankTwin *tankTwin;
    TankSniper *tankSniper;
    TankMachineGun *tankMachineGun;
    TankTriple *tankTriple;
    TankAssassin *tankAssassin;
    TankDestroyer *tankDestroyer;

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
    bool canEvolve();
    bool evolveTank(TankType newType);
    float getBulletSpeed();
    int getBulletLife();
    float getBulletDamage();
    void Drawtank(sf::RenderWindow &window);
    void moveX(float dx);
    void moveY(float dy);
    bool upgradeStat(int statIndex);
    void shoot(std::vector<Bullet> &bullets, int angle);
    TankType getTankType();
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
