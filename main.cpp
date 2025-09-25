#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace sf;
using namespace std;

class Body{
    public:
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
        friend void nhanVatDiChuyen(Body &body, RenderWindow &window);
        
};

class Cannon{
    private:
        float dmg, range, bullet_speed; // stats
        float angle; // góc bắn
        Vector2f toaDo;

        RectangleShape gun;
    public:
        Cannon(Body &body) 
            : dmg(10), range(200), bullet_speed(20), angle(0), toaDo(body.toaDo){
            float rong = body.size*0.5, dai = body.size*2;
            gun.setSize({dai, rong});
            gun.setOrigin({0, rong/2});
            gun.setFillColor(Color(153,153,153));
            gun.setOutlineThickness(5.f);
            gun.setOutlineColor(Color(102,102,102));
            gun.setPosition(toaDo);
        }

        void move(Vector2f pos){
            toaDo = pos;
            gun.setPosition(toaDo);
        }

        void draw(RenderWindow &window){
            window.draw(gun);
        }

        void setRotation(float newAngle){
            angle = newAngle;
            gun.setRotation(degrees(angle));
        }
};

void nhanVatDiChuyen(Body &body, RenderWindow &window){
    Vector2f tangToc(0, 0);
    float maSat = 0.9;
    if (Keyboard::isKeyPressed(Keyboard::Key::W)) tangToc.y -= body.acc;
    if (Keyboard::isKeyPressed(Keyboard::Key::S)) tangToc.y += body.acc;
    if (Keyboard::isKeyPressed(Keyboard::Key::A)) tangToc.x -= body.acc;
    if (Keyboard::isKeyPressed(Keyboard::Key::D)) tangToc.x += body.acc;
    // cập nhật vận tốc
    body.speed += tangToc;
    float speedLen = sqrt(body.speed.x*body.speed.x + body.speed.y*body.speed.y);
    body.speed *= maSat;
    // loại bỏ vận tốc rất nhỏ tranh rung lắc
    if (abs(body.speed.x) < 0.1) body.speed.x = 0;
    if (abs(body.speed.y) < 0.1) body.speed.y = 0;
    body.move();
    body.draw(window);
}
void updateCannon(Cannon &gun, Body &body, RenderWindow &window){
    // cập nhật góc quay súng
    Vector2i mousePos = Mouse::getPosition(window);
    int dx=mousePos.x - body.toaDo.x;
    int dy=mousePos.y - body.toaDo.y;
    int newAngle = atan2(dy,dx)*180/3.14;
    if(newAngle<0) newAngle+=360;

    gun.setRotation(newAngle);
    gun.move(body.toaDo);
    gun.draw(window);
}
int main() {
    unsigned int WIDTH = 800, HEIGHT = 600;
    ContextSettings settings;
    settings.attributeFlags=8;
    RenderWindow window(VideoMode({WIDTH, HEIGHT}), "Diep",Style::Default, State::Windowed, settings);
    window.setFramerateLimit(60);
    
    sf::Font font;
    if(!font.openFromFile("arial.ttf"))
        return -1;
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(Color::Red);

    Body body(WIDTH/2, HEIGHT/2, 75);
    Cannon gun(body);


    while(window.isOpen()){
        while(auto event = window.pollEvent()){
            if(event->is<Event::Closed>()){
                window.close();
            }
        }
        window.clear(Color(204, 204, 204));

        updateCannon(gun, body, window);
        nhanVatDiChuyen(body, window);
                   
        window.display();
    }

    return 0;
}