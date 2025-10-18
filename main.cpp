#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <optional>
#include <ctime>

constexpr float HITBOX_SCALE = 0.85f;

struct Stats {
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

class Obj {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mapsize;
    float hitbox_r;
    float hp, maxhp, hp_regen, body_dmg;

    Obj(float x, float y, float mapsize, float size, float maxhp, float hp_regen, float body_dmg)
        : position({ x, y }), velocity({ 0, 0 }), mapsize(mapsize), hitbox_r(size), hp(maxhp), maxhp(maxhp), hp_regen(hp_regen), body_dmg(body_dmg) {
    }

    void update() {
        position += velocity;
        position.x = std::clamp(position.x, -mapsize + hitbox_r, mapsize - hitbox_r);
        position.y = std::clamp(position.y, -mapsize + hitbox_r, mapsize - hitbox_r);
        if (hp < maxhp) hp = std::min(maxhp, hp + hp_regen);
    }
    float Getx() { return position.x; }
    float Gety() { return position.y; }
    void setMaxHp(float new_maxhp) { maxhp = new_maxhp; if (hp > maxhp) hp = maxhp; }
    void setHpRegen(float new_hp_regen) { hp_regen = new_hp_regen; }
    void setBodyDmg(float new_body_dmg) { body_dmg = new_body_dmg; }

    bool handleCollisionWith(Obj& other, bool applyBounce = true, float bounceForce = 8.f) {
        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;
        float dist = std::hypot(dx, dy);
        float minDist = hitbox_r + other.hitbox_r;
        if (dist > 0.f && dist < minDist) {
            sf::Vector2f dir = { dx / dist, dy / dist };
            float overlap = (minDist - dist);
            position += dir * (overlap * 0.5f);
            other.position -= dir * (overlap * 0.5f);

            if (applyBounce) {
                velocity += dir * bounceForce;
                other.velocity -= dir * bounceForce;
            }

            float damage = (body_dmg + other.body_dmg) * 0.5f + overlap * 0.3f;
            hp = std::max(0.f, hp - damage);
            other.hp = std::max(0.f, other.hp - damage);
            return true;
        }
        return false;
    }

    bool isCollidingWith(const Obj& other) const {
        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;
        float dist = std::hypot(dx, dy);
        return dist < (hitbox_r + other.hitbox_r);
    }

    float getDistanceTo(const Obj& other) const {
        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;
        return std::hypot(dx, dy);
    }
};

class Cannon {
public:
    int delay, reload;
    float angle;
    sf::Vector2f position;
    sf::RectangleShape gun;
    sf::Vector2f size;

    Cannon(float x, float y, float size_, float reload_)
        : position({ x, y }), angle(0), delay(0), reload(reload_), size(size_ * 2.f, size_ * 2.f / 3.f) {
        gun.setSize({ size_ * 2.f, size_ * 2.f / 3.f });
        gun.setFillColor(sf::Color(153, 153, 153));
        gun.setOutlineThickness(5.f);
        gun.setOutlineColor(sf::Color(102, 102, 102));
        gun.setOrigin({ 0, size_ / 3.f });
        gun.setPosition({ x, y });
    }
    void update(sf::Vector2f pos, int angleDeg) {
        if (delay > 0) delay -= 1;
        position = pos;
        gun.setPosition(pos);
        gun.setRotation(sf::degrees((float)angleDeg));
    }
    void DrawGun(sf::RenderWindow& window) { window.draw(gun); }
    void setReload(int new_reload) { reload = std::max(1, new_reload); }
    void setSize(float size_) {
        gun.setSize({ size_ * 2.f, size_ * 2.f / 3.f });
        size = { size_ * 2.f, size_ * 2.f / 3.f };
    }
};

class Bullet {
public:
    Obj body;
    sf::CircleShape shape;
    int timetodie;
    bool alive;
    bool fromPlayer;
    int blinkTimer;
    bool isBlinking;

    Bullet(sf::Vector2f pos, float angleDeg, float size, float speed, int timetodie_, float dmg, bool isPlayer = true)
        : body(pos.x, pos.y, 2000.f, size, dmg, 0.f, dmg), timetodie(timetodie_), alive(true), fromPlayer(isPlayer), blinkTimer(0), isBlinking(false) {
        shape.setRadius(size);
        shape.setFillColor(sf::Color(0, 178, 225));
        shape.setOutlineThickness(3.f);
        shape.setOutlineColor(sf::Color(14, 144, 178));
        shape.setOrigin({ size, size });
        shape.setPosition(pos);
        float rad = angleDeg * 3.14159265f / 180.f;
        body.velocity.x = speed * std::cos(rad);
        body.velocity.y = speed * std::sin(rad);
        float gunLength = size * 6.f + 10.f;
        body.position.x = pos.x + gunLength * std::cos(rad);
        body.position.y = pos.y + gunLength * std::sin(rad);
        shape.setPosition(body.position);
    }

    void update() {
        if (timetodie <= 0) alive = false;
        if (alive) {
            timetodie -= 1;
            body.update();
            shape.setPosition(body.position);

            if (blinkTimer > 0) {
                blinkTimer--;
                isBlinking = (blinkTimer / 3) % 2 == 0;
            }
            else {
                isBlinking = false;
            }
        }
    }

    void startBlink() {
        blinkTimer = 20;
    }

    void draw(sf::RenderWindow& window) {
        if (alive && !isBlinking) window.draw(shape);
    }
};

void removeBullet(std::vector<Bullet>& bullets, int index) {
    if (!bullets.empty()) { std::swap(bullets[index], bullets.back()); bullets.pop_back(); }
}

class TankBasic {
public:
    Cannon gun;
    TankBasic(float x, float y, float size) : gun(x, y, size, 30) {}
    void drawTank(sf::RenderWindow& window, sf::CircleShape& bodyShape) { gun.DrawGun(window); window.draw(bodyShape); }
    void update(sf::Vector2f pos, int angle) { gun.update(pos, angle); }
    void shoot(std::vector<Bullet>& bullets, int angle, float bSpeed, int bLife, float bDmg) {
        if (gun.delay == 0) {
            bullets.emplace_back(gun.position, (float)angle, gun.size.y / 2.f, bSpeed, bLife, bDmg, true);
            gun.delay = gun.reload;
        }
    }
};

class TankTwin {
public:
    Cannon gun1, gun2;
    int gunToggle;
    float gunOffSet;

    TankTwin(float x, float y, float size) : gun1(x, y, size, 30), gun2(x, y, size, 30), gunToggle(0), gunOffSet(size / 2.f) {}
    void update(sf::Vector2f pos, float angleDeg) {
        float rad90 = (angleDeg + 90.f) * 3.14159265f / 180.f;
        float offX = gunOffSet * std::cos(rad90);
        float offY = gunOffSet * std::sin(rad90);
        sf::Vector2f posGun1(pos.x - offX, pos.y - offY);
        sf::Vector2f posGun2(pos.x + offX, pos.y + offY);
        gun1.update(posGun1, (int)angleDeg);
        gun2.update(posGun2, (int)angleDeg);
    }
    void shoot(std::vector<Bullet>& bullets, int angle, float bSpeed, int bLife, float bDmg) {
        if (gunToggle == 0 && gun1.delay == 0) {
            bullets.emplace_back(gun1.position, (float)angle, gun1.size.y / 2.f, bSpeed, bLife, bDmg, true);
            gun1.delay = gun1.reload;
            gunToggle = 1;
        }
        if (gunToggle == 1 && gun2.delay == 0) {
            bullets.emplace_back(gun2.position, (float)angle, gun2.size.y / 2.f, bSpeed, bLife, bDmg, true);
            gun2.delay = gun2.reload;
            gunToggle = 0;
        }
    }
    void drawTank(sf::RenderWindow& window, sf::CircleShape& bodyShape) { gun1.DrawGun(window); gun2.DrawGun(window); window.draw(bodyShape); }
};

class MyTank {
public:
    Obj body;
    float velocity_max;
    float friction;
    sf::CircleShape bodyShape;
    int xp_base, xp_to_lv_up;
    int score;
    int level;
    int statPoint;
    Stats stats;
    int tankType;
    TankBasic* tankBasic;
    TankTwin* tankTwin;

    float base_hp_regen = 0.2f;
    float base_maxhp = 100;
    float base_body_dmg = 2;
    float base_bullet_speed = 8;
    int   base_bullet_life = 150;
    float base_bullet_dmg = 10;
    int   base_reload = 30;
    float base_move_speed = 6;

    MyTank(float x, float y, float size, float mapsize)
        : body(x, y, mapsize, size, 100, 0.2f, 2), velocity_max(6), friction(0.9f), xp_base(0), xp_to_lv_up(100), score(0), level(1), statPoint(0), tankType(0), tankBasic(nullptr), tankTwin(nullptr) {
        bodyShape.setRadius(size);
        bodyShape.setFillColor(sf::Color(0, 178, 225));
        bodyShape.setOutlineThickness(5.f);
        bodyShape.setOutlineColor(sf::Color(14, 144, 178));
        bodyShape.setOrigin({ size, size });
        bodyShape.setPosition({ x, y });
        body.position = { x, y };
        body.velocity = { 0, 0 };
        tankBasic = new TankBasic(x, y, size);
    }
    ~MyTank() { if (tankBasic) delete tankBasic; if (tankTwin) delete tankTwin; }

    void update(int angle) {
        body.velocity *= friction;
        body.velocity.x = std::clamp(body.velocity.x, -velocity_max, velocity_max);
        body.velocity.y = std::clamp(body.velocity.y, -velocity_max, velocity_max);
        if (std::abs(body.velocity.x) < 0.1f) body.velocity.x = 0.f;
        if (std::abs(body.velocity.y) < 0.1f) body.velocity.y = 0.f;
        body.update();
        if (tankType == 0) tankBasic->update(body.position, angle);
        else tankTwin->update(body.position, (float)angle);
        bodyShape.setPosition(body.position);
    }

    void applyStats() {
        body.setHpRegen(base_hp_regen * (1 + stats.hp_regen * 0.3f));
        body.setMaxHp(base_maxhp * (1 + stats.maxhp * 0.2f));
        body.setBodyDmg(base_body_dmg * (1 + stats.body_dmg * 0.25f));
        velocity_max = base_move_speed * (1 + stats.move_speed * 0.05f);
        int newReload = base_reload - stats.reload * 2;
        if (tankType == 0) tankBasic->gun.setReload(newReload);
        else { tankTwin->gun1.setReload(newReload); tankTwin->gun2.setReload(newReload); }
    }

    void addScore(int point) { score += point; }

    void levelUp() {
        if (score >= xp_to_lv_up) {
            level += 1;
            xp_base = xp_to_lv_up;
            xp_to_lv_up += level * 100;
            statPoint += 1;
            body.hp = body.maxhp;
        }
        if (level >= 5 && tankType == 0) {
            delete tankBasic; tankBasic = nullptr;
            bodyShape.setRadius(bodyShape.getRadius() + 5);
            bodyShape.setOrigin({ bodyShape.getRadius(), bodyShape.getRadius() });
            body.hitbox_r = bodyShape.getRadius();
            tankTwin = new TankTwin(body.position.x, body.position.y, bodyShape.getRadius());
            tankType = 1;
        }
    }

    float getBulletSpeed() { return base_bullet_speed * (1 + stats.bullet_speed * 0.15f); }
    int   getBulletLife() { return base_bullet_life + stats.bullet_penetration * 15; }
    float getBulletDamage() { return base_bullet_dmg * (1 + stats.bullet_dmg * 0.2f); }

    void Drawtank(sf::RenderWindow& window) {
        if (tankType == 0) tankBasic->drawTank(window, bodyShape);
        else tankTwin->drawTank(window, bodyShape);
    }

    void moveX(float dx) { body.velocity.x += dx; }
    void moveY(float dy) { body.velocity.y += dy; }

    bool upgradeStat(int statIndex) {
        if (statPoint <= 0) return false;
        switch (statIndex) {
        case 0: if (stats.hp_regen < stats.maxLevel) { stats.hp_regen++; statPoint--; applyStats(); return true; } break;
        case 1: if (stats.maxhp < stats.maxLevel) { stats.maxhp++; statPoint--; applyStats(); body.hp = body.maxhp; return true; } break;
        case 2: if (stats.body_dmg < stats.maxLevel) { stats.body_dmg++; statPoint--; applyStats(); return true; } break;
        case 3: if (stats.bullet_speed < stats.maxLevel) { stats.bullet_speed++; statPoint--; return true; } break;
        case 4: if (stats.bullet_penetration < stats.maxLevel) { stats.bullet_penetration++; statPoint--; return true; } break;
        case 5: if (stats.bullet_dmg < stats.maxLevel) { stats.bullet_dmg++; statPoint--; return true; } break;
        case 6: if (stats.reload < stats.maxLevel) { stats.reload++; statPoint--; applyStats(); return true; } break;
        case 7: if (stats.move_speed < stats.maxLevel) { stats.move_speed++; statPoint--; applyStats(); return true; } break;
        }
        return false;
    }

    void shoot(std::vector<Bullet>& bullets, int angle) {
        float bSpeed = getBulletSpeed();
        int   bLife = getBulletLife();
        float bDmg = getBulletDamage();
        if (tankType == 0) tankBasic->shoot(bullets, angle, bSpeed, bLife, bDmg);
        else tankTwin->shoot(bullets, angle, bSpeed, bLife, bDmg);
    }

    int getTankType() { return tankType; }
};

class Minimap {
public:
    sf::RectangleShape map;
    sf::CircleShape player;
    sf::Vector2f Mapsize;

    Minimap(float x, float y, float size, sf::Vector2f mapsize) : Mapsize(mapsize) {
        map.setSize({ size, size });
        map.setFillColor(sf::Color(198, 198, 198));
        map.setOutlineThickness(4.f);
        map.setOutlineColor(sf::Color(114, 114, 114));
        map.setOrigin({ size / 2.f, size / 2.f });
        map.setPosition({ x - size / 2.f - 25.f, y - size / 2.f - 25.f });
        player.setRadius(3);
        player.setFillColor(sf::Color(60, 60, 60));
        player.setOrigin({ 3, 3 });
    }
    void Drawmap(sf::RenderWindow& window, sf::Vector2f playerpos = { 120,120 }) {
        player.setPosition({
            playerpos.x / Mapsize.x * (map.getSize().x / 2.f) + map.getPosition().x,
            playerpos.y / Mapsize.y * (map.getSize().y / 2.f) + map.getPosition().y
            });
        window.draw(map);
        window.draw(player);
    }
};

class Line {
private:
    sf::VertexArray line;
public:
    Line(int w, int h) : line(sf::PrimitiveType::Lines) {
        for (int i = -w; i <= w; i += 50) {
            line.append(sf::Vertex{ sf::Vector2f((float)i, (float)-h), sf::Color(195,195,195) });
            line.append(sf::Vertex{ sf::Vector2f((float)i, (float)h), sf::Color(195,195,195) });
        }
        for (int i = -h; i <= h; i += 50) {
            line.append(sf::Vertex{ sf::Vector2f((float)-w, (float)i), sf::Color(195,195,195) });
            line.append(sf::Vertex{ sf::Vector2f((float)w, (float)i), sf::Color(195,195,195) });
        }
    }
    void draw(sf::RenderWindow& window) { window.draw(line); }
};

class Obstacle {
public:
    Obj body;
    sf::CircleShape shape;
    int type;
    int xp_reward;
    bool alive{ true };
    sf::Clock respawnClock;
    float friction{ 0.9f };
    int blinkTimer;
    sf::Color originalFillColor;

    struct Conf {
        float drawRadius;
        float hp;
        float bodyDmg;
        int   xp;
        sf::Color fill, outline;
    };

    Conf getConfigFor(int t) {
        if (t == 4) { return Conf{ 20.f, 60.f, 5.f, 6,  sf::Color(255,231,105), sf::Color(195,178, 80) }; }
        else if (t == 3) { return Conf{ 25.f, 90.f, 8.f, 12, sf::Color(252,118,119), sf::Color(190, 90, 90) }; }
        else { return Conf{ 40.f,160.f,14.f,24, sf::Color(118,141,252), sf::Color(96,112,189) }; }
    }

    Obstacle(float x, float y, float, float mapsize, int t)
        : type(t),
        body(x, y, mapsize, getConfigFor(t).drawRadius* HITBOX_SCALE, getConfigFor(t).hp, 0.f, getConfigFor(t).bodyDmg),
        xp_reward(getConfigFor(t).xp), blinkTimer(0) {
        Conf c = getConfigFor(t);
        originalFillColor = c.fill;
        shape.setPointCount(t);
        shape.setOutlineThickness(5.f);
        shape.setRadius(c.drawRadius);
        shape.setOrigin({ c.drawRadius, c.drawRadius });
        shape.setPosition({ x, y });
        shape.setFillColor(c.fill);
        shape.setOutlineColor(c.outline);
        float randomDeg = (float)(rand() % 360);
        shape.setRotation(sf::degrees(randomDeg));
    }

    void update() {
        if (!alive) return;
        body.velocity *= friction;
        if (std::abs(body.velocity.x) < 0.1f) body.velocity.x = 0.f;
        if (std::abs(body.velocity.y) < 0.1f) body.velocity.y = 0.f;
        body.update();
        shape.setPosition(body.position);

        if (blinkTimer > 0) {
            blinkTimer--;
            if (blinkTimer % 4 == 0) {
                shape.setFillColor(sf::Color::White);
            }
            else {
                shape.setFillColor(originalFillColor);
            }
        }
        else {
            shape.setFillColor(originalFillColor);
        }
    }

    void startBlink() {
        blinkTimer = 10;
    }

    void DrawObs(sf::RenderWindow& window) { if (alive) window.draw(shape); }

    void respawn(float map_w, float map_h, const std::vector<Obstacle>& others, const Obj& tankBody) {
        if (alive) return;
        if (respawnClock.getElapsedTime().asSeconds() < 30.f) return;
        bool ok = false;
        sf::Vector2f p;
        while (!ok) {
            p.x = (float)((rand() % (int)(map_w * 2)) - (int)map_w);
            p.y = (float)((rand() % (int)(map_h * 2)) - (int)map_h);
            ok = true;
            float dxT = p.x - tankBody.position.x;
            float dyT = p.y - tankBody.position.y;
            if (std::hypot(dxT, dyT) < (body.hitbox_r + tankBody.hitbox_r)) { ok = false; continue; }
            size_t count = others.size();
            for (size_t idx = 0; idx < count; idx++) {
                if (&others[idx] == this) continue;
                if (!others[idx].alive) continue;
                float dx = p.x - others[idx].body.position.x;
                float dy = p.y - others[idx].body.position.y;
                if (std::hypot(dx, dy) < (body.hitbox_r + others[idx].body.hitbox_r)) { ok = false; break; }
            }
        }
        body.position = p;
        body.hp = body.maxhp;
        body.velocity = { 0.f, 0.f };
        shape.setPosition(p);
        shape.setRotation(sf::degrees((float)(rand() % 360)));
        alive = true;
        respawnClock.restart();
    }
};
void checkCollision(std::vector<Bullet>& bullets, std::vector<Obstacle>& obstacles, MyTank& mytank) {
    const float EPS = 1e-3f;

    size_t bulletCount = bullets.size();
    for (size_t i = 0; i < bulletCount; i++) {
        if (!bullets[i].alive) continue;
        size_t obsCount = obstacles.size();
        for (size_t j = 0; j < obsCount; j++) {
            if (!obstacles[j].alive) continue;

            float dist = bullets[i].body.getDistanceTo(obstacles[j].body);

            if (dist < bullets[i].body.hitbox_r + obstacles[j].body.hitbox_r) {
                if (dist > 0.f) {
                    float dx = bullets[i].body.position.x - obstacles[j].body.position.x;
                    float dy = bullets[i].body.position.y - obstacles[j].body.position.y;
                    sf::Vector2f dir = { dx / dist, dy / dist };
                    float pushForce = 5.f;
                    bullets[i].body.velocity += dir * pushForce;
                    obstacles[j].body.velocity -= dir * pushForce * 0.3f;
                }
                bullets[i].startBlink();
                obstacles[j].startBlink();
                float A = std::max(0.0f, obstacles[j].body.body_dmg);
                float B = std::max(0.0f, bullets[i].body.body_dmg);
                float a = std::max(0.0f, bullets[i].body.hp);
                float b = std::max(0.0f, obstacles[j].body.hp);

                if (A <= 0.f && B <= 0.f) { bullets[i].alive = false; break; }

                float tb = (A > 0.f) ? (a / A) : 999999.f;
                float to = (B > 0.f) ? (b / B) : 999999.f;

                if (tb + 1e-6f < to) {
                    bullets[i].body.hp = 0.f;
                    obstacles[j].body.hp = std::max(EPS, b - B * tb);
                }
                else if (to + 1e-6f < tb) {
                    obstacles[j].body.hp = 0.f;
                    bullets[i].body.hp = std::max(EPS, a - A * to);
                }
                else {
                    if (a >= b) {
                        obstacles[j].body.hp = 0.f;
                        bullets[i].body.hp = std::max(EPS, a - A * to);
                    }
                    else {
                        bullets[i].body.hp = 0.f;
                        obstacles[j].body.hp = std::max(EPS, b - B * tb);
                    }
                }

                if (bullets[i].body.hp <= 0.f) bullets[i].alive = false;
                if (obstacles[j].body.hp <= 0.f) {
                    obstacles[j].alive = false;
                    obstacles[j].respawnClock.restart();
                    obstacles[j].shape.setFillColor(sf::Color(100, 100, 100));
                    mytank.addScore(obstacles[j].xp_reward);
                }

                if (!bullets[i].alive) break;
            }
        }
        if (!bullets[i].alive || bullets[i].fromPlayer) continue;
        float dist = bullets[i].body.getDistanceTo(mytank.body);
        if (dist < bullets[i].body.hitbox_r + mytank.body.hitbox_r) {
            if (dist > 0.f) {
                float dx = bullets[i].body.position.x - mytank.body.position.x;
                float dy = bullets[i].body.position.y - mytank.body.position.y;
                sf::Vector2f dir = { dx / dist, dy / dist };
                float pushForce = 5.f;
                bullets[i].body.velocity += dir * pushForce;
                mytank.body.velocity -= dir * pushForce * 0.3f;
            }
            bullets[i].startBlink();
            float A = std::max(0.0f, mytank.body.body_dmg);
            float B = std::max(0.0f, bullets[i].body.body_dmg);
            float a = std::max(0.0f, bullets[i].body.hp);
            float b = std::max(0.0f, mytank.body.hp);

            if (A <= 0.f && B <= 0.f) { bullets[i].alive = false; continue; }

            float tb = (A > 0.f) ? (a / A) : 999999.f;
            float to = (B > 0.f) ? (b / B) : 999999.f;

            if (tb + 1e-6f < to) {
                bullets[i].body.hp = 0.f;
                mytank.body.hp = std::max(EPS, b - B * tb);
            }
            else if (to + 1e-6f < tb) {
                mytank.body.hp = 0.f;
                bullets[i].body.hp = std::max(EPS, a - A * to);
            }
            else {
                if (a >= b) {
                    mytank.body.hp = 0.f;
                    bullets[i].body.hp = std::max(EPS, a - A * to);
                }
                else {
                    bullets[i].body.hp = 0.f;
                    mytank.body.hp = std::max(EPS, b - B * tb);
                }
            }

            if (bullets[i].body.hp <= 0.f) bullets[i].alive = false;
        }
    }
    for (int i = (int)bullets.size() - 1; i >= 0; --i) {
        if (!bullets[i].alive) removeBullet(bullets, i);
    }
}

class XpBar {
private:
    sf::RectangleShape xpBarBG, xpBar;
    sf::Text xpText, scoreText;
    std::stringstream xpss, scoress;
    sf::Vector2f pos, size;
public:
    XpBar(float x, float y, sf::Vector2f size_, MyTank&, sf::Font& font) : pos(x, y), xpText(font), scoreText(font), size(size_) {
        xpBarBG.setSize(size);
        xpBarBG.setOrigin({ size.x / 2.f, size.y / 2.f });
        xpBarBG.setPosition({ x / 2.f - 25.f, y - 25.f });
        xpBarBG.setFillColor(sf::Color(150, 150, 150));
        xpBarBG.setOutlineThickness(4.f);
        xpBarBG.setOutlineColor(sf::Color(114, 114, 114));
        xpBar.setOrigin({ size.x / 2.f, size.y / 2.f });
        xpBar.setPosition({ x / 2.f - 25.f, y - 25.f });
        xpBar.setFillColor(sf::Color(0, 240, 0));
        xpText.setCharacterSize(14);
        xpText.setFillColor(sf::Color::White);
        xpText.setOutlineThickness(2.f);
        xpText.setOutlineColor(sf::Color::Black);
        scoreText.setCharacterSize(12);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setOutlineThickness(2.f);
        scoreText.setOutlineColor(sf::Color::Black);
    }
    void update(MyTank& mytank) {
        float ratio = (float)(mytank.score - mytank.xp_base) / (float)(mytank.xp_to_lv_up - mytank.xp_base);
        ratio = std::clamp(ratio, 0.f, 1.f);
        xpBar.setSize({ size.x * ratio, size.y });
        xpss.str(""); xpss.clear();
        xpss << "Level " << mytank.level;
        sf::FloatRect xpTextBounds = xpText.getLocalBounds();
        xpText.setOrigin({ xpTextBounds.size.x / 2.f, xpTextBounds.size.y / 2.f });
        xpText.setPosition({ pos.x / 2.f - 30.f, pos.y - 25.f });
        xpText.setString(xpss.str());
        sf::FloatRect scoreTextBounds = scoreText.getLocalBounds();
        scoreText.setOrigin({ scoreTextBounds.size.x / 2.f, scoreTextBounds.size.y / 2.f });
        scoreText.setPosition({ pos.x / 2.f - 30.f, pos.y - 50.f });
        scoress.str(""); scoress.clear();
        scoress << "Score: " << mytank.score;
        scoreText.setString(scoress.str());
    }
    void draw(sf::RenderWindow& window) { window.draw(xpBarBG); window.draw(xpBar); window.draw(xpText); window.draw(scoreText); }
};

class Bar {
public:
    sf::RectangleShape barBG, fill, button;
    sf::Text name, idxText;
    int index;
    sf::Color color;

    Bar(sf::Vector2f size, sf::Font& f, sf::Color color_) : name(f), color(color_), idxText(f) {
        barBG.setSize(size);
        barBG.setFillColor(sf::Color(150, 150, 150));
        barBG.setOutlineThickness(2);
        barBG.setOutlineColor(sf::Color(114, 114, 114));
        fill.setSize({ 0, size.y });
        fill.setFillColor(color);
        button.setSize({ size.y, size.y });
        button.setFillColor(sf::Color(100, 200, 100));
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(60, 150, 60));
        name.setCharacterSize(8);
        name.setFillColor(sf::Color::White);
        name.setOutlineThickness(1);
        name.setOutlineColor(sf::Color::Black);
        idxText.setCharacterSize(10);
        idxText.setFillColor(sf::Color::White);
        idxText.setOutlineThickness(2);
        idxText.setOutlineColor(sf::Color::Black);
    }
    void updatePosition(sf::Vector2f position) {
        barBG.setPosition(position);
        fill.setPosition(position);
        button.setPosition({ position.x + barBG.getSize().x + 5.f, position.y });
        name.setPosition({ position.x + 30.f, position.y + barBG.getSize().y / 2.f - 4.f });
        idxText.setPosition({ button.getPosition().x + button.getSize().x + 10.f, position.y + barBG.getSize().y / 2.f - 6.f });
    }
};

class StatsBar {
private:
    std::vector<Bar> bars;
    sf::Text pointsText;
    sf::Vector2f position;
    bool visible;
public:
    float barWidth = 180;
    float barHeight = 18;
    float barSpacing = 8;

    StatsBar(sf::Font& f, int w, int h) : pointsText(f), visible(false) {
        position.x = 15;
        position.y = h - 8 * (barHeight + barSpacing) - 50;
        std::vector<std::string> statNames = { "Health Regen", "Max Health", "Body Damage", "Bullet Speed", "Bullet Penetration", "Bullet Damage", "Reload", "Movement Speed" };
        std::vector<sf::Color> colors = { sf::Color(255,150,120), sf::Color(200,120,255), sf::Color(180,100,100), sf::Color(120,180,255), sf::Color(255,220,120), sf::Color(255,120,120), sf::Color(120,255,180), sf::Color(150,255,150) };
        for (int i = 0; i < 8; i++) {
            Bar bar({ barWidth, barHeight }, f, colors[i]);
            bar.name.setString(statNames[i]);
            bar.index = i;
            bars.push_back(bar);
        }
        pointsText.setCharacterSize(12);
        pointsText.setFillColor(sf::Color::Yellow);
        pointsText.setOutlineThickness(2);
        pointsText.setOutlineColor(sf::Color::Black);
    }

    void update(MyTank& mytank) {
        visible = (mytank.statPoint > 0);
        std::stringstream ss;
        ss << "Stat Points: " << mytank.statPoint;
        pointsText.setString(ss.str());
        pointsText.setPosition({ position.x + 30.f, position.y - 25.f });
        int statValues[10] = { mytank.stats.hp_regen, mytank.stats.maxhp, mytank.stats.body_dmg, mytank.stats.bullet_speed, mytank.stats.bullet_penetration, mytank.stats.bullet_dmg, mytank.stats.reload, mytank.stats.move_speed };
        for (int i = 0; i < 8; i++) {
            float ypos = position.y + i * (barHeight + barSpacing);
            bars[i].updatePosition({ position.x, ypos });
            float ratio = (float)statValues[i] / mytank.stats.maxLevel;
            bars[i].fill.setSize({ bars[i].barBG.getSize().x * ratio, bars[i].barBG.getSize().y });
            std::stringstream idx_ss; idx_ss << "[" << bars[i].index + 1 << "]";
            bars[i].idxText.setString(idx_ss.str());
        }
    }

    int checkButtonClick(sf::Vector2i mousePos) {
        for (int i = 0; i < 8; i++) if (bars[i].button.getGlobalBounds().contains(sf::Vector2f(mousePos))) return i;
        return -1;
    }

    void draw(sf::RenderWindow& window, sf::Font& font) {
        if (!visible) return;
        window.draw(pointsText);
        for (int i = 0; i < 8; i++) {
            window.draw(bars[i].barBG);
            window.draw(bars[i].fill);
            window.draw(bars[i].button);
            window.draw(bars[i].name);
            window.draw(bars[i].idxText);
            sf::Text plus(font);
            plus.setString("+");
            plus.setCharacterSize(15);
            plus.setFillColor(sf::Color::White);
            plus.setOutlineThickness(1);
            plus.setOutlineColor(sf::Color::Black);
            sf::FloatRect bounds = bars[i].button.getGlobalBounds();
            sf::FloatRect plusBounds = plus.getLocalBounds();
            plus.setPosition({ bounds.position.x + bounds.size.x / 2.f - plusBounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f - plusBounds.size.y + 2.f });
            window.draw(plus);
        }
    }

    bool isVisible() { return visible; }
};

int main() {
    srand((unsigned)time(0));
    constexpr unsigned int WIDTH = 900;
    constexpr unsigned int HEIGHT = 600;
    float acceleration = 0.7f;
    float map_width = 2000.f, map_height = 2000.f;
    float bodysize = 36.f;

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode({ WIDTH, HEIGHT }), "Diep", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(60);
    sf::View mainview;
    mainview.setSize({ (float)WIDTH, (float)HEIGHT });

    sf::Font font;
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Red);
    sf::Font SupercellMagic;

    MyTank mytank(0.f, 0.f, bodysize, map_width);
    Minimap minimap((float)WIDTH, (float)HEIGHT, 120.f, { map_width, map_height });
    XpBar xpbar((float)WIDTH, (float)HEIGHT, { 400.f, 10.f }, mytank, SupercellMagic);
    StatsBar statsbar(SupercellMagic, WIDTH, HEIGHT);
    Line line((int)map_width, (int)map_height);

    std::vector<Obstacle> obs;
    std::vector<Bullet> bullets;

    for (int i = 0; i < 100; i++) {
        int type = rand() % 3 + 3;
        float ox, oy;
        do {
            ox = (float)(rand() % (int)(map_width * 2) - (int)map_width);
            oy = (float)(rand() % (int)(map_height * 2) - (int)map_height);
        } while (std::hypot(ox, oy) <= 200.f);
        obs.emplace_back(ox, oy, 0.f, map_width, type);
    }

    int angle = 0;

    while (window.isOpen()) {
        while (const std::optional ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();
            else if (const auto* keyPressed = ev->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) window.close();
                if (keyPressed->scancode == sf::Keyboard::Scancode::R) { mytank.addScore(1000); mytank.levelUp(); }
                if (keyPressed->scancode == sf::Keyboard::Scancode::T) { mytank.score = 0; mytank.levelUp(); }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) mytank.moveY(-acceleration);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) mytank.moveY(acceleration);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) mytank.moveX(-acceleration);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) mytank.moveX(acceleration);

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int dx = mousePos.x - (int)window.getSize().x / 2;
        int dy = mousePos.y - (int)window.getSize().y / 2;
        angle = (int)(std::atan2((float)dy, (float)dx) * 180.f / 3.14159265f);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) mytank.shoot(bullets, angle);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && statsbar.isVisible()) {
            int statIndex = statsbar.checkButtonClick(mousePos);
            if (statIndex != -1) mytank.upgradeStat(statIndex);
        }

        mytank.update(angle);
        mytank.levelUp();
        xpbar.update(mytank);
        statsbar.update(mytank);

        size_t bulletCount = bullets.size();
        for (size_t i = 0; i < bulletCount; i++) {
            bullets[i].update();
        }

        for (int i = (int)bullets.size() - 1; i >= 0; --i) {
            if (!bullets[i].alive) removeBullet(bullets, i);
        }

        size_t obsCount = obs.size();
        for (size_t i = 0; i < obsCount; i++) {
            if (!obs[i].alive) continue;
            obs[i].update();
            if (mytank.body.handleCollisionWith(obs[i].body)) {
                obs[i].startBlink();
                obs[i].shape.setPosition(obs[i].body.position);
                mytank.bodyShape.setPosition(mytank.body.position);
                if (mytank.getTankType() == 0) mytank.tankBasic->gun.update(mytank.body.position, angle);
                else mytank.tankTwin->update(mytank.body.position, (float)angle);
            }
            if (obs[i].body.hp <= 0.f) { obs[i].alive = false; obs[i].respawnClock.restart(); obs[i].shape.setFillColor(sf::Color(100, 100, 100)); }
        }
        checkCollision(bullets, obs, mytank);

        obsCount = obs.size();
        for (size_t i = 0; i < obsCount; i++) {
            if (!obs[i].alive) obs[i].respawn(map_width, map_height, obs, mytank.body);
        }

        window.clear(sf::Color(204, 204, 204));
        sf::View mainviewCpy = mainview;
        mainviewCpy.setCenter(mytank.body.position);
        window.setView(mainviewCpy);
        line.draw(window);

        bulletCount = bullets.size();
        for (size_t i = 0; i < bulletCount; i++) bullets[i].draw(window);

        obsCount = obs.size();
        for (size_t i = 0; i < obsCount; i++) obs[i].DrawObs(window);

        mytank.Drawtank(window);
        window.setView(window.getDefaultView());
        statsbar.draw(window, SupercellMagic);
        xpbar.draw(window);
        minimap.Drawmap(window, mytank.body.position);
        window.draw(text);
        window.display();
    }

    return 0;
}