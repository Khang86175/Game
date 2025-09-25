#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <cmath>
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
            circle.setOutlineThickness(10.f);
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

int main(){
    int WIDTH = 800, HEIGHT = 600;
    sf::ContextSettings settings;
    settings.attributeFlags=8;
    sf::RenderWindow window(VideoMode(WIDTH, HEIGHT), "Diep", Style::Default, settings);
    window.setFramerateLimit(30);

    Body body(WIDTH/2, HEIGHT/2, 75);
    Clock clock;
    
    while (window.isOpen()){
        while (auto event = window.pollEvent()){
            if (event->type == Event::Closed){
                window.close();
            }
        }
        window.clear(Color::Black);

        float dt = clock.restart().asSeconds();
        body.move(dt);
        body.draw(window);

        
        window.display();
    }
    return 0;
}
