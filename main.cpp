#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace sf;
using namespace std;

class Body{
    private:
        Vector2f toaDo;
        Vector2f speed;
        float speedMax;
        float acc;
        float size;
        float max_hp, hp_regen, body_dmg; // stats

        CircleShape circle;
    public:
        Body(float x, float y, float size)
            : toaDo(x, y), size(size), speed(0, 0), speedMax(20), acc(0.7),
              max_hp(100), hp_regen(1), body_dmg(10){
            circle.setRadius(size);
            circle.setFillColor(Color::Blue);
            circle.setOutlineThickness(5.f);
            circle.setOutlineColor(Color::Cyan);
            circle.setOrigin({size,size});
            circle.setPosition(toaDo);
        }

        void draw(RenderWindow &window){
            window.draw(circle);
        }

        void move(){
            toaDo+=speed;
            circle.setPosition(toaDo);
        }
        friend void nhanVatDiChuyen(Body &body);
        
};
void nhanVatDiChuyen(Body &body){
    Vector2f tangToc(0, 0);
    float maSat = 0.9;
    if (Keyboard::isKeyPressed(Keyboard::Key::W)) tangToc.y -= body.acc;
    if (Keyboard::isKeyPressed(Keyboard::Key::S)) tangToc.y += body.acc;
    if (Keyboard::isKeyPressed(Keyboard::Key::A)) tangToc.x -= body.acc;
    if (Keyboard::isKeyPressed(Keyboard::Key::D)) tangToc.x += body.acc;
    // cập nhật vận tốc
    body.speed.x += tangToc.x;
    if (body.speed.x > body.speedMax) body.speed.x = body.speedMax;
    if (body.speed.x < -body.speedMax) body.speed.x = -body.speedMax;
    body.speed.y += tangToc.y;
    if (body.speed.y > body.speedMax) body.speed.y = body.speedMax;
    if (body.speed.y < -body.speedMax) body.speed.y = -body.speedMax;
    body.speed *= maSat;
    // loại bỏ vận tốc rất nhỏ tranh rung lắc
    if (abs(body.speed.x) < 0.1) body.speed.x = 0;
    if (abs(body.speed.y) < 0.1) body.speed.y = 0;
    body.move();
}
int main() {
    int WIDTH = 800; // dài 
    int HEIGHT = 600; // rộng
    ContextSettings settings;
    settings.attributeFlags=8;
    RenderWindow window(VideoMode({WIDTH, HEIGHT}), "Diep",Style::Default, State::Windowed, settings);
    window.setFramerateLimit(30);

    Body body(WIDTH/2, HEIGHT/2, 75);

     
    
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
        nhanVatDiChuyen(body);
        
        

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

        window.display();
    }

    return 0;
}