#include "HealthBar.hpp"

HealthBar::HealthBar(float width, float height, float offset) 
    : size(width, height), offset(offset) {
    background.setSize(size);
    background.setFillColor(sf::Color(150,150,150));
    background.setOutlineThickness(1.f);
    background.setOutlineColor(sf::Color(114,114,114));
    background.setOrigin({size.x / 2.f, size.y / 2.f});
    fill.setSize(size);
    fill.setFillColor(sf::Color(85, 255, 85));
    fill.setOrigin({size.x / 2.f, size.y / 2.f});
}
void HealthBar::update(Obj &body) {
    background.setPosition({body.position.x, body.position.y + offset});
    fill.setPosition({body.position.x, body.position.y + offset});
    float hpRatio = std::clamp(body.hp / body.maxhp, 0.f, 1.f);
    if (hpRatio > 0.6f) {
        fill.setFillColor(sf::Color(85, 255, 85));
    } else if (hpRatio > 0.3f) {
        fill.setFillColor(sf::Color(255, 200, 85));
    } else {
        fill.setFillColor(sf::Color(255, 85, 85));
    }
    fill.setSize({size.x * hpRatio, size.y});
}

void HealthBar::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(fill);
}
void HealthBar::setOffSet(float new_offset){
    offset = new_offset;
}
