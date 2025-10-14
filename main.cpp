#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <vector>

class Obj {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mapsize;
    float hitbox_r;
    float hp, maxhp, hp_regen, body_dmg;

    Obj(float x, float y, float mapsize, float size)
        : position({ x, y }), velocity({ 0, 0 }), mapsize(mapsize), hitbox_r(size) {
        hp = maxhp = 100;
        hp_regen = 1;
        body_dmg = 2;
    }

    void update() {
        position += velocity;
        position.x = std::clamp(position.x, -mapsize + hitbox_r, mapsize - hitbox_r);
        position.y = std::clamp(position.y, -mapsize + hitbox_r, mapsize - hitbox_r);
        if (hp < maxhp)
            hp += hp_regen;
        if (hp > maxhp)
            hp = maxhp;
    }

    bool handleCollisionWith(Obj& other) {
        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;
        float dist = std::hypot(dx, dy);
        float minDist = hitbox_r + other.hitbox_r;

        if (dist > 0 && dist < minDist) {
            sf::Vector2f dir = { dx / dist, dy / dist };
            float overlap = (minDist - dist);

            position += dir * (overlap * 0.5f);
            other.position -= dir * (overlap * 0.5f);

            float pushForce = 8.f;
            velocity += dir * pushForce;
            other.velocity -= dir * pushForce;

            float damage = body_dmg + overlap * 0.3f;
            hp -= damage;
            other.hp -= damage;

            if (hp < 0) hp = 0;
            if (other.hp < 0) other.hp = 0;

            return true;
        }
        return false;
    }

    bool isCollidingWith(const Obj& other) const {
        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;
        float dist = std::hypot(dx, dy);
        float minDist = hitbox_r + other.hitbox_r;
        return dist < minDist;
    }
};

class Cannon {
public:
    int delay;
    sf::Vector2f position;
    sf::RectangleShape gun;

    Cannon(float x, float y, float size) : position({ x, y }), delay(0) {
        gun.setSize({ size * 2, size * 2 / 3 });
        gun.setFillColor(sf::Color(153, 153, 153));
        gun.setOutlineThickness(5.f);
        gun.setOutlineColor(sf::Color(102, 102, 102));
        gun.setOrigin({ 0, size / 3 });
        gun.setPosition({ x, y });
    }

    void update(sf::Vector2f pos, float angle) {
        if (delay > 0) delay--;
        position = pos;
        gun.setPosition(pos);
        gun.setRotation(sf::degrees(angle));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(gun);
    }
};

class MyTank {
public:
    Obj body;
    Cannon gun;
    float velocity_max;
    float friction;
    sf::CircleShape bodyShape;
    int score;
    int level;

    MyTank(float x, float y, float size, float mapsize)
        : body(x, y, mapsize, size), gun(x, y, size), score(0), level(1) {
        bodyShape.setRadius(size);
        bodyShape.setFillColor(sf::Color(0, 178, 225));
        bodyShape.setOutlineThickness(5.f);
        bodyShape.setOutlineColor(sf::Color(14, 144, 178));
        bodyShape.setOrigin({ size, size });
        bodyShape.setPosition({ x, y });
        body.velocity = { 0, 0 };
        velocity_max = 6;
        friction = 0.9f;
    }

    void update(float angle) {
        body.velocity *= friction;
        body.velocity.x = std::clamp(body.velocity.x, -velocity_max, velocity_max);
        body.velocity.y = std::clamp(body.velocity.y, -velocity_max, velocity_max);
        if (body.velocity.x < 0.1f && body.velocity.x > -0.1f)
            body.velocity.x = 0;
        if (body.velocity.y < 0.1f && body.velocity.y > -0.1f)
            body.velocity.y = 0;
        body.update();

        gun.update(body.position, angle);
        bodyShape.setPosition(body.position);
    }

    void draw(sf::RenderWindow& window) {
        gun.draw(window);
        window.draw(bodyShape);
    }

    void moveX(float dx) {
        body.velocity.x += dx;
    }

    void moveY(float dy) {
        body.velocity.y += dy;
    }
};

class Minimap {
public:
    sf::RectangleShape map;
    sf::CircleShape player;
    sf::Vector2f mapSize;

    Minimap(float x, float y, float size, sf::Vector2f mapsize) : mapSize(mapsize) {
        map.setSize({ size, size });
        map.setFillColor(sf::Color(198, 198, 198));
        map.setOutlineThickness(4.f);
        map.setOutlineColor(sf::Color(114, 114, 114));
        map.setOrigin({ size / 2, size / 2 });
        map.setPosition({ x - size / 2 - 25, y - size / 2 - 25 });
        player.setRadius(3);
        player.setFillColor(sf::Color(60, 60, 60));
        player.setOrigin({ 5, 5 });
    }

    void draw(sf::RenderWindow& window, sf::Vector2f playerpos) {
        player.setPosition({
            playerpos.x / mapSize.x * map.getSize().x / 2 + map.getPosition().x,
            playerpos.y / mapSize.y * map.getSize().y / 2 + map.getPosition().y
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
            line.append(sf::Vertex{ sf::Vector2f(i, -h), sf::Color(195, 195, 195) });
            line.append(sf::Vertex{ sf::Vector2f(i, h), sf::Color(195, 195, 195) });
        }
        for (int i = -h; i <= h; i += 50) {
            line.append(sf::Vertex{ sf::Vector2f(-w, i), sf::Color(195, 195, 195) });
            line.append(sf::Vertex{ sf::Vector2f(w, i), sf::Color(195, 195, 195) });
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(line);
    }
};

class Obstacle {
public:
    Obj body;
    sf::CircleShape shape;
    bool alive;
    sf::Clock respawnClock;

    Obstacle(float x, float y, float size, float mapsize, int type)
        : body(x, y, mapsize, size), alive(true) {
        shape.setPointCount(type);
        shape.setOutlineThickness(5.f);

        if (type == 3) {
            shape.setRadius(25);
            shape.setFillColor(sf::Color(252, 118, 119));
            shape.setOutlineColor(sf::Color(190, 90, 90));
        }
        else if (type == 4) {
            shape.setRadius(20);
            shape.setFillColor(sf::Color(255, 231, 105));
            shape.setOutlineColor(sf::Color(195, 178, 80));
        }
        else {
            shape.setRadius(40);
            shape.setFillColor(sf::Color(118, 141, 252));
            shape.setOutlineColor(sf::Color(96, 112, 189));
        }

        shape.setOrigin({ size, size });
        shape.setPosition({ x, y });
    }

    void draw(sf::RenderWindow& window) {
        if (alive)
            window.draw(shape);
    }

    void respawn(float map_width, float map_height, const std::vector<Obstacle>& others, MyTank& tank) {
        if (alive) return;

        if (respawnClock.getElapsedTime().asSeconds() >= 30.f) {
            bool validPos = false;
            sf::Vector2f newPos;

            while (!validPos) {
                newPos.x = (rand() % (int)(map_width * 2)) - map_width;
                newPos.y = (rand() % (int)(map_height * 2)) - map_height;
                validPos = true;

                if (body.handleCollisionWith(tank.body)) {
                    validPos = false;
                    continue;
                }

                for (const auto& o : others) {
                    if (&o == this) continue;
                    if (!o.alive) continue;
                    float dist = std::hypot(newPos.x - o.body.position.x, newPos.y - o.body.position.y);
                    if (dist < body.hitbox_r + o.body.hitbox_r) {
                        validPos = false;
                        break;
                    }
                }
            }

            body.position = newPos;
            shape.setPosition(newPos);
            body.hp = body.maxhp;
            body.velocity = { 0, 0 };
            alive = true;
            respawnClock.restart();
        }
    }
};

class Bullet {
public:
    Obj body;
    sf::CircleShape shape;
    int timetodie;
    bool alive;
    bool fromPlayer;

    Bullet(sf::Vector2f pos, float angle, float size, float speed, int ttd, bool isPlayer = true)
        : body(pos.x, pos.y, 2000, size), timetodie(ttd), alive(true), fromPlayer(isPlayer) {
        shape.setRadius(size);
        shape.setFillColor(sf::Color(0, 178, 225));
        shape.setOutlineThickness(3.f);
        shape.setOutlineColor(sf::Color(14, 144, 178));
        shape.setOrigin({ size, size });
        shape.setPosition(pos);

        float rad = angle * 3.14159f / 180.f;
        body.velocity.x = speed * cos(rad);
        body.velocity.y = speed * sin(rad);
    }

    void update() {
        if (timetodie <= 0)
            alive = false;

        if (alive) {
            timetodie--;
            body.update();
            shape.setPosition(body.position);
        }
    }

    void draw(sf::RenderWindow& window) {
        if (alive)
            window.draw(shape);
    }
};

void handleBulletObstacleCollision(std::vector<Bullet>& bullets, std::vector<Obstacle>& obstacles, MyTank& tank) {
    for (auto& bullet : bullets) {
        if (!bullet.alive) continue;

        for (auto& obs : obstacles) {
            if (!obs.alive) continue;

            if (bullet.body.isCollidingWith(obs.body)) {
                bullet.alive = false;
                obs.body.hp -= 10.f;

                if (obs.body.hp <= 0) {
                    obs.alive = false;
                    obs.respawnClock.restart();
                    obs.shape.setFillColor(sf::Color(100, 100, 100));
                    tank.score += 10;
                }

                break;
            }
        }
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.alive; }),
        bullets.end()
    );
}

void handleBulletTankCollision(std::vector<Bullet>& bullets, MyTank& tank) {
    for (auto& bullet : bullets) {
        if (!bullet.alive || bullet.fromPlayer) continue; // Không va chạm với đạn của player

        if (bullet.body.handleCollisionWith(tank.body)) {
            bullet.alive = false;
            tank.body.hp -= 10.f;
            if (tank.body.hp < 0) tank.body.hp = 0;
        }
    }
}

int main() {
    srand((unsigned)time(0));

    constexpr unsigned int WIDTH = 900;
    constexpr unsigned int HEIGHT = 600;
    float acceleration = 0.7f;
    float map_width = 2000.f;
    float map_height = 2000.f;
    float bodysize = 36.f;

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode({ WIDTH, HEIGHT }), "Diep", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(60);

    sf::View mainview;
    mainview.setSize({ (float)WIDTH, (float)HEIGHT });

    MyTank mytank(0, 0, bodysize, map_width);
    Minimap minimap(WIDTH, HEIGHT, 120, { map_width, map_height });
    Line line((int)map_width, (int)map_height);

    std::vector<Obstacle> obs;
    std::vector<Bullet> bullets;

    for (int i = 0; i < 100; i++) {
        int type = rand() % 3 + 3;
        float x = (float)((rand() % (int)(map_width * 2)) - (int)map_width);
        float y = (float)((rand() % (int)(map_height * 2)) - (int)map_height);
        obs.emplace_back(x, y, (float)(type * 10), map_width, type);
    }

    sf::Font font;
    sf::Text scoreText(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition({ 10.f, 10.f });

    float angle = 0.f;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            mytank.moveY(-acceleration);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            mytank.moveY(acceleration);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            mytank.moveX(-acceleration);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            mytank.moveX(acceleration);

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        float dx = (float)(mousePos.x - (int)window.getSize().x / 2);
        float dy = (float)(mousePos.y - (int)window.getSize().y / 2);
        angle = atan2(dy, dx) * 180.f / 3.14159f;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && mytank.gun.delay == 0) {
            bullets.emplace_back(mytank.body.position, angle, 10.f, 8.f, 150);
            mytank.gun.delay = 15;
        }

        mytank.update(angle);

        for (auto& o : obs) {
            if (o.alive) {
                mytank.body.handleCollisionWith(o.body);
                o.shape.setPosition(o.body.position);
                if (o.body.hp <= 0) {
                    o.alive = false;
                    o.respawnClock.restart();
                }
            }
        }

        for (auto& b : bullets) {
            b.update();
        }

        handleBulletObstacleCollision(bullets, obs, mytank);
        handleBulletTankCollision(bullets, mytank);

        for (auto& o : obs) {
            if (!o.alive) {
                o.respawn(map_width, map_height, obs, mytank);
            }
        }

        window.clear(sf::Color(204, 204, 204));
        mainview.setCenter(mytank.body.position);
        window.setView(mainview);

        line.draw(window);

        for (auto& o : obs) {
            o.draw(window);
        }

        for (auto& b : bullets) {
            b.draw(window);
        }

        mytank.draw(window);

        window.setView(window.getDefaultView());
        scoreText.setString("Score: " + std::to_string(mytank.score) + " | Bullets: " + std::to_string(bullets.size()));
        window.draw(scoreText);
        minimap.draw(window, mytank.body.position);

        window.display();
    }

    return 0;
}