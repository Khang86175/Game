#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace sf;
using namespace std;

class Body{
    private:
        float x, y; // toạ độ
        float size;
        float max_hp, hp_regen, body_dmg, speed; // stats

        CircleShape circle;
    public:
        Body(float x, float y, float size)
            : x(x), y(y), size(size){
            circle.setRadius(size);
            circle.setFillColor(Color::Blue);
            circle.setOutlineThickness(5.f);
            circle.setOutlineColor(Color::Cyan);
            circle.setOrigin({size,size});
            circle.setPosition({x,y});
            speed = 200;
        }

        void draw(RenderWindow &window){
            window.draw(circle);
        }

        void move(float dt){
            Vector2f vec(0,0);
            if (Keyboard::isKeyPressed(Keyboard::Key::W)) vec.y -= 1;
            if (Keyboard::isKeyPressed(Keyboard::Key::S)) vec.y += 1;
            if (Keyboard::isKeyPressed(Keyboard::Key::A)) vec.x -= 1;
            if (Keyboard::isKeyPressed(Keyboard::Key::D)) vec.x += 1;

            if (vec.x != 0 || vec.y != 0){
                float len = sqrt(vec.x * vec.x + vec.y * vec.y);
                vec /= len;
                vec *= speed * dt;
            }

            x += vec.x;
            y += vec.y;
            circle.setPosition({x,y});
        }
        
};
int main() {
    int WIDTH = 600; // dài 
    int HEIGHT = 600; // rộng
    ContextSettings settings;
    settings.attributeFlags=8;
    RenderWindow window(VideoMode({WIDTH, HEIGHT}), "Diep",Style::Default, State::Windowed, settings);
    window.setFramerateLimit(30);

    Body body(WIDTH/2, HEIGHT/2, 75);
    Clock clock;
    
    int x=WIDTH/2, y=HEIGHT/2;
    sf::RectangleShape gun(sf::Vector2f(150.f, 50.f));
    gun.setFillColor(sf::Color(153,153,153));
    gun.setOutlineThickness(5.f);
    gun.setOutlineColor(sf::Color(102,102,102));
    gun.setPosition({x,y-25});

    sf::Font font;
    if(!font.openFromFile("arial.ttf"))
        return -1;
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Red);


    while(window.isOpen()){
        while(auto event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){
                window.close();
            }
        }

        float dt = clock.restart().asSeconds();

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int dx=mousePos.x-x, dy=mousePos.y-y;
        int angle=atan2(dy,dx)*180/3.14;
        std::stringstream ss;
        ss << mousePos.x <<" "<<mousePos.y<<" "<<angle;
        gun.setPosition({x+cos((angle-90)*3.14/180)*25,y+sin((angle-90)*3.14/180)*25});
        text.setString(ss.str());
        gun.setRotation(sf::degrees(angle));
        window.clear(sf::Color(204, 204, 204));
        window.draw(text);
        window.draw(gun);

        body.draw(window);
        body.move(dt);

        window.display();
    }

    return 0;
}