#include "Bullet.hpp"

Bullet::Bullet(sf::Vector2f pos, float angle,float size,float gun_length,float speed,int timetodie, float dmg,int type)
    : body(pos.x, pos.y, 2000, size, 10, 0, dmg), timetodie(timetodie), alive(true){
    shape.setRadius(size);
    shape.setFillColor(type==0? sf::Color(0,178,225): sf::Color(239,77,85));
    shape.setOutlineThickness(3.f);
    shape.setOutlineColor(type == 0 ? sf::Color(14, 144, 178): sf::Color(205,65,70));
    shape.setOrigin({size,size});
    shape.setPosition(pos);
    body.velocity.x=speed * cos(angle*3.14/180);
    body.velocity.y=speed * sin(angle*3.14/180);
    float gunLength = gun_length+10;
    body.position.x = pos.x + gunLength * cos(angle * 3.14f / 180.f);
    body.position.y = pos.y + gunLength * sin(angle * 3.14f / 180.f);
    body.setBodyDmg(dmg);
}

void Bullet::update(){
    
    if(timetodie==0)
        alive=false;
    if(alive){            
        timetodie-=1;
        body.update();
        shape.setPosition(body.position);
    }
}
void Bullet::draw(sf::RenderWindow &window){
    window.draw(shape);
}


void removeBullet(std::vector<Bullet> &bullets, int index){
    if(!bullets.empty() && index < bullets.size()){
        std::swap(bullets[index], bullets.back());
        bullets.pop_back();
    }
}
