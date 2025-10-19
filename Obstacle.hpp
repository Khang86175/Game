#pragma once
#include "Objects.hpp"
#include "Globals.hpp"

class Obstacle {
public:
    Obj body;
    sf::CircleShape shape;
    int type;
    int xp_reward;
    bool alive{ true };
    int timeToRespawn=0;
    float friction{ 0.9f };

    struct Conf {
        float drawRadius;
        float hp;
        float bodyDmg;
        int   xp;
        sf::Color fill, outline;
    };

    static Conf configFor(int t);

    Obstacle(float x, float y, float mapsize, int t);

    void update();

    void DrawObs(sf::RenderWindow& window);

    void respawn(const std::vector<Obstacle>& others, const Obj& tankBody);
};