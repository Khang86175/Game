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
        float speedMax, acc;
        float size;
        float max_hp, hp_regen, body_dmg; // stats

        CircleShape body;
    public:
        Body(float x, float y, float size)
            : toaDo(x, y), size(size), speed(0, 0), speedMax(15), acc(0.5),
              max_hp(100), hp_regen(1), body_dmg(10){
            body.setRadius(size);
            body.setFillColor(Color::Blue);
            body.setOutlineThickness(5.f);
            body.setOutlineColor(Color::Cyan);
            body.setOrigin({size,size});
            body.setPosition(toaDo);
        }

        void draw(RenderWindow &window){
            window.draw(body);
        }

        void move(){
            toaDo+=speed;
            body.setPosition(toaDo);
        }
        friend void nhanVatDiChuyen(Body &body);
        
};

class Cannon{
    private:
        float dmg, range, bullet_speed; // stats
        float angle; // góc bắn
        Vector2f toaDo;

        RectangleShape gun;
    public:
        Cannon(float x, float y) 
            : dmg(10), range(200), bullet_speed(20), angle(0), toaDo(x, y){
            gun.setSize({50, 15});
            gun.setOrigin({10, 7.5});
            gun.setFillColor(Color(153,153,153));
            gun.setOutlineThickness(5.f);
            gun.setOutlineColor(Color(102,102,102));
            gun.setPosition({x,y-25});
        }

        void move(Vector2f pos){
            toaDo = pos;
            gun.setPosition(toaDo);
        }

        void draw(RenderWindow &window){
            window.draw(gun);
        }
        void setRotation(float angle){
            this->angle = angle;
            gun.setRotation(degrees(angle));
        }
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

        body.draw(window);

        window.display();
    }

    return 0;
}