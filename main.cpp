#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({600, 600}), "SFML Window");

    float x=300,y=300;

    sf::CircleShape body(75.f);
    body.setFillColor(sf::Color(0,178,225));
    body.setOutlineThickness(5.f);
    body.setOutlineColor(sf::Color(14, 144, 178));
    
    sf::RectangleShape gun(sf::Vector2f(150.f, 50.f));
    gun.setFillColor(sf::Color(153,153,153));
    gun.setOutlineThickness(5.f);
    gun.setOutlineColor(sf::Color(102,102,102));
    gun.setPosition({x,y-25});

    
    body.setPosition({x-75,y-75});


    while(window.isOpen()){
        while(auto event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){
                window.close();
            }
        }
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        window.clear(sf::Color(204, 204, 204));
        window.draw(gun);
        window.draw(body);
        window.display();
    }

    return 0;
}