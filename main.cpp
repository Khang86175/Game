#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

class Map {
private:
    sf::RectangleShape background;
    sf::VertexArray grid;
    float width, height;
public:
    Map(float w, float h)
        : width(w), height(h), grid(sf::PrimitiveType::Lines) {
        background.setSize({ w, h });
        background.setFillColor(sf::Color(204, 204, 204));
        background.setPosition({ 0, 0 });
        background.setOrigin({ w / 2, h / 2 });

        float cellW = width / 100; // chiều rộng của 1 ô
        float cellH = height / 100;  // chiều dài của 1 ô
        for (float i = -width / 2; i <= width / 2; i += cellW) {
            sf::Vertex vertex1{ {i, -height / 2}, sf::Color(230,230,230) };
            sf::Vertex vertex2{ {i, height / 2}, sf::Color(230,230,230) };
            grid.append(vertex1);
            grid.append(vertex2);
        }
        for (float i = -height / 2; i <= height / 2; i += cellH) {
            sf::Vertex vertex1{ {-width / 2, i}, sf::Color(230,230,230) };
            sf::Vertex vertex2{ {width / 2, i}, sf::Color(230,230,230) };
            grid.append(vertex1);
            grid.append(vertex2);
        }
    }
    void DrawBackground(sf::RenderWindow& window) {
        window.draw(background);
        window.draw(grid);
    }
    float getWidth() {
        return width;
    }
    float getHeight() {
        return height;
    }
};

class Obj {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float hp, maxhp, hp_regen, body_dmg;

    Obj(float x, float y) :position({ x,y }), velocity({ 0,0 }) {
        hp = maxhp = 100;
        hp_regen = 1;
        body_dmg = 2;
    }
    void update() {
        position += velocity;
        if (hp < maxhp)
            hp += hp_regen;
        if (hp > maxhp)
            hp = maxhp;
    }
    float Getx() {
        return position.x;
    }
    float Gety() {
        return position.y;
    }
};

class Cannon {
public:
    int delay;
    float angle;
    sf::Vector2f position;
    sf::RectangleShape gun;

    Cannon(float x, float y, float size) :position({ x,y }), angle(0), delay(0) {
        gun.setSize({ size * 2, size * 2 / 3 });
        gun.setFillColor(sf::Color(153, 153, 153));
        gun.setOutlineThickness(5.f);
        gun.setOutlineColor(sf::Color(102, 102, 102));
        gun.setOrigin({ 0,size / 3 });
        gun.setPosition({ x,y });
    }
    void update(sf::Vector2f pos, int angle) {
        position = pos;
        gun.setPosition(pos);
        gun.setRotation(sf::degrees(angle));
    }
    void DrawGun(sf::RenderWindow& window) {
        window.draw(gun);
    }
};

// ====== Hitbox class (vòng tròn) =====
class Hitbox {
public:
    float x, y, r;
    Hitbox(float x = 0, float y = 0, float r = 0) : x(x), y(y), r(r) {}
    void SetPosition(float nx, float ny) { x = nx; y = ny; }
    bool CheckCollide(const Hitbox& other) {
        float dx = x - other.x;
        float dy = y - other.y;
        float dist = sqrt(dx * dx + dy * dy);
        return dist < (r + other.r);
    }
};

class MyTank {
public:
    Obj body;
    Cannon gun;
    float velocity_max;
    float dpos;
    float friction;
    sf::CircleShape bodyShape;
    Hitbox hitbox; // thêm hitbox

    MyTank(float x, float y, float size) :body(x, y), gun(x, y, size), hitbox(x, y, size) {
        bodyShape.setRadius(size);
        bodyShape.setFillColor(sf::Color(0, 178, 225));
        bodyShape.setOutlineThickness(5.f);
        bodyShape.setOutlineColor(sf::Color(14, 144, 178));
        bodyShape.setOrigin({ size,size });
        bodyShape.setPosition({ x,y });
        body.position = { x,y };
        body.velocity = { 0,0 };
        velocity_max = 6;
        friction = 0.9;
    }
    void update(int angle, Map& map) {
        body.velocity *= friction;
        body.velocity.x = std::clamp(body.velocity.x, -velocity_max, velocity_max);
        body.velocity.y = std::clamp(body.velocity.y, -velocity_max, velocity_max);
        dpos = std::hypot(body.velocity.x, body.velocity.y);
        if (dpos > velocity_max) {
            body.velocity.x = body.velocity.x / dpos * velocity_max;
            body.velocity.y = body.velocity.y / dpos * velocity_max;
        }
        if (body.velocity.x<0.1 && body.velocity.x>-0.1)
            body.velocity.x = 0;
        if (body.velocity.y<0.1 && body.velocity.y>-0.1)
            body.velocity.y = 0;
        body.update();
        float r = bodyShape.getRadius();
        body.position.x = std::clamp(body.position.x, -map.getWidth() / 2 + r, map.getWidth() / 2 - r);
        body.position.y = std::clamp(body.position.y, -map.getHeight() / 2 + r, map.getHeight() / 2 - r);

        hitbox.SetPosition(body.position.x, body.position.y);

        gun.update(body.position, angle);
        bodyShape.setPosition(body.position);
    }
    void Drawtank(sf::RenderWindow& window) {
        gun.DrawGun(window);
        window.draw(bodyShape);
    }
    void moveX(float dx) {
        body.velocity.x += dx;
    }
    void moveY(float dy) {
        body.velocity.y += dy;
    }
};

// ======= ObstacleMap class =========
class ObstacleMap {
public:
    struct Obstacle {
        sf::ConvexShape shape;
        Hitbox hitbox;
    };
    std::vector<Obstacle> obstacles;

    ObstacleMap(int count, float mapWidth, float mapHeight) {
        for (int i = 0; i < count; i++) {
            int sides = 3 + rand() % 3; // 3,4 hoặc 5 cạnh
            float size = 30 + (rand() % 3) * 10; // 30,40,50

            sf::ConvexShape poly;
            poly.setPointCount(sides);
            for (int j = 0; j < sides; j++) {
                float angle = j * 2 * 3.14159f / sides - 3.14159f / 2;
                poly.setPoint(j, sf::Vector2f(cos(angle) * size, sin(angle) * size));
            }

            float x = -mapWidth / 2 + rand() % (int)(mapWidth);
            float y = -mapHeight / 2 + rand() % (int)(mapHeight);
            poly.setPosition(sf::Vector2f(x, y));


            if (sides == 3) {
                poly.setFillColor(sf::Color(252, 118, 119));
                poly.setOutlineColor(sf::Color(190, 90, 90));
            }
            else if (sides == 4) {
                poly.setFillColor(sf::Color(255, 232, 105));
                poly.setOutlineColor(sf::Color(195, 178, 80));
            }
            else {
                poly.setFillColor(sf::Color(118, 141, 252));
                poly.setOutlineColor(sf::Color(96, 112, 189));
            }
            poly.setOutlineThickness(3);

            Obstacle obs;
            obs.shape = poly;
            obs.hitbox = Hitbox(x, y, size);
            obstacles.push_back(obs);
        }
    }
    void DrawObstacles(sf::RenderWindow& window) {
        for (auto& obs : obstacles)
            window.draw(obs.shape);
    }
};



int main() {
    srand(time(0));
    constexpr unsigned int WIDTH = 800; // dài 
    constexpr unsigned int HEIGHT = 600; // rộng
    float acceleration = 0.6; // gia tốc
    float bodysize = 36;
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 4;
    bool fullscreen = false;
    sf::RenderWindow window(sf::VideoMode({ WIDTH, HEIGHT }), "Diep", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(30);

    float view_width = WIDTH;
    float view_height = HEIGHT;
    sf::View view;
    view.setSize({ view_width, view_height });

    MyTank mytank(0, 0, bodysize);
    Map map(2000, 2000);
    ObstacleMap obstacleMap(30, map.getWidth(), map.getHeight());
    // tải font chữ
    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
        return -1;
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Red);


    int angle = 1;

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            mytank.moveY(-acceleration);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            mytank.moveY(acceleration);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            mytank.moveX(-acceleration);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            mytank.moveX(acceleration);
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
        int dx = mousePos.x - mytank.body.Getx(), dy = mousePos.y - mytank.body.Gety();
        angle = atan2(dy, dx) * 180 / 3.14;

        mytank.update(angle, map);

        // Kiểm tra va chạm với vật cản
        for (int i = 0; i < (int)obstacleMap.obstacles.size(); i++) {
            if (mytank.hitbox.CheckCollide(obstacleMap.obstacles[i].hitbox)) {
                // Đẩy tank về vị trí cũ
                mytank.body.position -= mytank.body.velocity;
                mytank.bodyShape.setPosition(mytank.body.position);
                mytank.gun.update(mytank.body.position, angle);
                mytank.hitbox.SetPosition(mytank.body.position.x, mytank.body.position.y);
                break;
            }
        }

        view.setCenter(mytank.body.position);
        window.clear(sf::Color(204, 204, 204));
        map.DrawBackground(window);
        obstacleMap.DrawObstacles(window);
        window.draw(text);
        mytank.Drawtank(window);
        window.setView(view);
        window.display();
    }

    return 0;
}
