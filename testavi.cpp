#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({300, 300}), "SFML Window");
    sf::CircleShape shape(150.f);
    shape.setFillColor(sf::Color::Green);

    while(window.isOpen()){
        while(auto event = window.pollEvent()){
            if(event->is<sf::Event::Closed>()){
                window.close();
            }
        }

        window.clear(sf::Color(64, 64, 64));
        window.draw(shape);
        window.display();
    }

    return 0;
}