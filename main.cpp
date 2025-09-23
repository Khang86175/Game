#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
int main() {
    int WIDTH = 600; // dài 
    int HEIGHT = 600; // rộng
    sf::ContextSettings settings;
    settings.attributeFlags=8;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Diep",sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(30);

    float x=WIDTH/2,y=HEIGHT/2;
    float bodysize=75;
    sf::CircleShape body(bodysize);
    body.setFillColor(sf::Color(0,178,225));
    body.setOutlineThickness(5.f);
    body.setOutlineColor(sf::Color(14, 144, 178));
    
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

    body.setPosition({x-bodysize,y-bodysize});
    int angle=1;

    while(window.isOpen()){
        while(auto event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){
                window.close();
            }
        }
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
        window.draw(body);
        window.display();
    }

    return 0;
}