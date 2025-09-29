#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>

class Obj{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float hp,maxhp,hp_regen,body_dmg;

    Obj(float x, float y):position({x,y}),velocity({0,0}){
        hp=maxhp=100;
        hp_regen=1;
        body_dmg=2;
    }
    void update(){
        position+=velocity;
        if(hp<maxhp)
            hp+=hp_regen;
        if(hp>maxhp)
            hp=maxhp;
    }
    float Getx(){
        return position.x;
    }
    float Gety(){
        return position.y;
    }
};
    
class Cannon{
public:
    int delay;
    float angle;
    sf::Vector2f position;
    sf::RectangleShape gun;

    Cannon(float x, float y,float size):position({x,y}),angle(0),delay(0){
        gun.setSize({size*2, size*2/3});
        gun.setFillColor(sf::Color(153,153,153));
        gun.setOutlineThickness(5.f);
        gun.setOutlineColor(sf::Color(102,102,102));
        gun.setOrigin({0,size/3});
        gun.setPosition({x,y});
    }
    void update(sf::Vector2f pos, int angle){
        position=pos;
        gun.setPosition(pos);
        gun.setRotation(sf::degrees(angle));
    }
    void DrawGun(sf::RenderWindow &window){
        window.draw(gun);
    }   
};
class MyTank{
public:
    Obj body;
    Cannon gun;
    float velocity_max;
    float dpos;
    float friction;
    sf::CircleShape bodyShape;

    MyTank(float x, float y, float size):body(x,y),gun(x,y,size){
        bodyShape.setRadius(size);
        bodyShape.setFillColor(sf::Color(0,178,225));
        bodyShape.setOutlineThickness(5.f);
        bodyShape.setOutlineColor(sf::Color(14, 144, 178));
        bodyShape.setOrigin({size,size});
        bodyShape.setPosition({x,y});
        body.position={x,y};
        body.velocity={0,0};
        velocity_max=8;
        friction=0.9;
    }
    void update(int angle){
        body.velocity*=friction;
        body.velocity.x=std::clamp(body.velocity.x,-velocity_max,velocity_max);
        body.velocity.y=std::clamp(body.velocity.y,-velocity_max,velocity_max);
        dpos=std::hypot(body.velocity.x,body.velocity.y);
        if(dpos>velocity_max){
            body.velocity.x=body.velocity.x/dpos*velocity_max;
            body.velocity.y=body.velocity.y/dpos*velocity_max;
        }
        if(body.velocity.x<0.1 && body.velocity.x>-0.1)
            body.velocity.x=0;
        if(body.velocity.y<0.1 && body.velocity.y>-0.1)
            body.velocity.y=0;
        body.update();

        gun.update(body.position,angle);
        bodyShape.setPosition(body.position);
    }
    void Drawtank(sf::RenderWindow &window){
        gun.DrawGun(window);
        window.draw(bodyShape);
    }
    void moveX(float dx){
        body.velocity.x+=dx;
    }
    void moveY(float dy){
        body.velocity.y+=dy;
    }
};

int main(){
    int WIDTH = 600; // dài 
    int HEIGHT = 600; // rộng
    int acceleration = 1; // gia tốc
    float x=WIDTH/2,y=HEIGHT/2;
    float bodysize=60;

    sf::ContextSettings settings;
    settings.attributeFlags=8;
    bool fullscreen=false;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Diep",sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(30);

    MyTank mytank(x,y,bodysize);
    
    sf::Font font;
    if(!font.openFromFile("arial.ttf"))
        return -1;
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Red);

    int angle=1;

    while(window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)){
            mytank.moveY(-acceleration);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)){
            mytank.moveY(acceleration);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
            mytank.moveX(-acceleration);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
            mytank.moveX(acceleration);
        }
        window.clear(sf::Color(204, 204, 204));
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int dx=mousePos.x-mytank.body.Getx(), dy=mousePos.y-mytank.body.Gety();
        angle=atan2(dy,dx)*180/3.14;
        std::stringstream ss;
        mytank.update(angle);
        ss << mytank.body.position.x <<" "<<mytank.body.position.y<<" "<<angle << ' '<< mytank.body.velocity.x << ' ' << mytank.body.velocity.y;
        text.setString(ss.str());
        
        window.draw(text);
        mytank.Drawtank(window);
        window.display();
    }

    return 0;
}