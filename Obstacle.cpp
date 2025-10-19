#include "Obstacle.hpp"

Obstacle::Conf Obstacle::configFor(int t) {
    if (t == 4) { return Conf{ 20.f, 30.f, 5.f, 10,  sf::Color(255,231,105), sf::Color(195,178, 80) }; }
    else if (t == 3) { return Conf{ 25.f, 80.f, 8.f, 25, sf::Color(252,118,119), sf::Color(190, 90, 90) }; }
    else { return Conf{ 40.f,160.f,12.f,150, sf::Color(118,141,252), sf::Color(96,112,189) }; }
}

Obstacle::Obstacle(float x, float y, float mapsize, int t)
    : type(t),
    body(x, y, mapsize, configFor(t).drawRadius* HITBOX_SCALE, configFor(t).hp, 0.f, configFor(t).bodyDmg),
    xp_reward(configFor(t).xp) {
    Conf c = configFor(t);
    shape.setPointCount(static_cast<std::size_t>(t));
    shape.setOutlineThickness(5.f);
    shape.setRadius(c.drawRadius);
    shape.setOrigin({ c.drawRadius, c.drawRadius });
    shape.setPosition({ x, y });
    shape.setFillColor(c.fill);
    shape.setOutlineColor(c.outline);
    float randomDeg = static_cast<float>(rand() % 360);
    shape.setRotation(sf::degrees(randomDeg));
}

void Obstacle::update() {
    if (!alive) return;
    body.update();
    body.velocity *= friction;
    if (std::abs(body.velocity.x) < 0.1f) body.velocity.x = 0.f;
    if (std::abs(body.velocity.y) < 0.1f) body.velocity.y = 0.f;
    shape.setPosition(body.position);
}

void Obstacle::DrawObs(sf::RenderWindow& window) { if (alive) window.draw(shape); }

void Obstacle::respawn(const std::vector<Obstacle>& others, const Obj& tankBody) {
    if(timeToRespawn<currentFrame)
        return;
    bool ok = false;
    sf::Vector2f p;
    while (!ok) {
        p.x = (float)((rand() % MapSize*2) - MapSize);
        p.y = (float)((rand() % MapSize*2) - MapSize);
        ok = true;
        float dxT = p.x - tankBody.position.x;
        float dyT = p.y - tankBody.position.y;
        if (std::hypot(dxT, dyT) < (body.hitbox_r + tankBody.hitbox_r)) {
            ok = false; 
            continue; 
        }
        for (const auto& o : others) {
            if (&o == this) continue;
            if (!o.alive) continue;
            float dx = p.x - o.body.position.x;
            float dy = p.y - o.body.position.y;
            if (std::hypot(dx, dy) < (body.hitbox_r + o.body.hitbox_r)) { 
                ok = false; 
                continue;
            }
        }
    }
    body.position = p;
    body.hp = body.maxhp;
    body.velocity = { 0.f, 0.f };
    shape.setPosition(p);
    shape.setRotation(sf::degrees((float)(rand() % 360)));
    alive = true;
}
