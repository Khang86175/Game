#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>

class Body{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float size;
    float hp,maxhp,hp_regen,body_dmg;
    sf::CircleShape body;

    Body(float x, float y, float size):position({x,y}),velocity({0,0}),size(size){
        hp=maxhp=100;
        hp_regen=1;
        body_dmg=2;
        body.setRadius(size);
        body.setFillColor(sf::Color(0,178,225));
        body.setOutlineThickness(5.f);
        body.setOutlineColor(sf::Color(14, 144, 178));
        body.setOrigin({size,size});
        body.setPosition({x,y});
    }
    void DrawBody(sf::RenderWindow &window){
        window.draw(body);
    }
    void update(sf::Vector2f pos){
        position=pos;
        body.setPosition(pos);
    }
};
    
class Cannon{
public:
    float dmg, time_to_die,speed;
    float angle;
    sf::Vector2f position;
    sf::RectangleShape gun;

    Cannon(float x, float y,float size):position({x,y}),dmg(10),time_to_die(210),speed(20),angle(0){
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
private:
    Body body;
    Cannon gun;
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float velocity_max;
    float friction;
    MyTank(float x, float y, float size):body(x,y,size),gun(x,y,size){
        position={x,y};
        velocity_max=8;
        velocity={0,0};
        friction=0.9;
    }
    void update(int angle){
        velocity*=friction;
        velocity.x=std::clamp(velocity.x,-velocity_max,velocity_max);
        velocity.y=std::clamp(velocity.y,-velocity_max,velocity_max);
        position+=velocity;
        if(velocity.x<0.1 && velocity.x>-0.1)
            velocity.x=0;
        if(velocity.y<0.1 && velocity.y>-0.1)
            velocity.y=0;
        gun.update(position,angle);
        body.update(position);
    }
    void Drawtank(sf::RenderWindow &window){
        gun.DrawGun(window);
        body.DrawBody(window);
    }
};

int main(){
    int WIDTH = 600; // dài 
    int HEIGHT = 600; // rộng
    sf::ContextSettings settings;
    settings.attributeFlags=8;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Diep",sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(30);

    float x=WIDTH/2,y=HEIGHT/2;
    float bodysize=60;

    MyTank mytank(x,y,bodysize);
    
    sf::Font font;
    if(!font.openFromFile("arial.ttf"))
        return -1;
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Red);

    int angle=1;

    while(window.isOpen()){
        while(auto event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){
                window.close();
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
            mytank.velocity.y-=1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)){
            mytank.velocity.y+=1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
            mytank.velocity.x-=1;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
            mytank.velocity.x+=1;
        }
        window.clear(sf::Color(204, 204, 204));
        
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int dx=mousePos.x-mytank.position.x, dy=mousePos.y-mytank.position.y;
        angle=atan2(dy,dx)*180/3.14;
        std::stringstream ss;
        mytank.update(angle);
        ss << mousePos.x <<" "<<mousePos.y<<" "<<angle << ' '<< mytank.velocity.x << ' ' << mytank.velocity.y;
        text.setString(ss.str());
        window.draw(text);

        mytank.Drawtank(window);
        window.display();
        
    }

    return 0;
}