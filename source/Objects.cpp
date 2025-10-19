#include "Objects.hpp"


float hypot2(const sf::Vector2f &x){
    return hypot(x.x,x.y);
}


Obj::Obj(float x, float y, float mapsize, float size, float maxhp, float hp_regen, float body_dmg)
    : position({x, y}), velocity({0, 0}), mapsize(mapsize), hitbox_r(size), maxhp(maxhp), hp(maxhp), hp_regen(hp_regen), body_dmg(body_dmg) {}
void Obj::update(){
    position+=velocity;
    position.x=std::clamp(position.x,-mapsize+hitbox_r,mapsize-hitbox_r);
    position.y=std::clamp(position.y,-mapsize+hitbox_r,mapsize-hitbox_r);
    hp=std::min(maxhp,hp+hp_regen);
}
float Obj::Getx(){
    return position.x;
}
float Obj::Gety(){
    return position.y;
}
void Obj::setMaxHp(float new_maxhp){
    maxhp = new_maxhp;
}
void Obj::setHpRegen(float new_hp_regen){
    hp_regen = new_hp_regen;
}
void Obj::setBodyDmg(float new_body_dmg){
    body_dmg = new_body_dmg;
}
void Obj::handleCollisionWith(Obj& other,bool instant,bool bounce ,float bounceScale,int dmging) {
    float dx = position.x - other.position.x;
    float dy = position.y - other.position.y;
    float dist = std::hypot(dx, dy);
    if(dist == 0.f)
        dist = 0.01f;
    float minDist = hitbox_r + other.hitbox_r;
    sf::Vector2f dir = { dx / dist, dy / dist };
    float overlap = (minDist - dist);

    if(instant == false){
        position += dir * (overlap * 0.5f);
        other.position -= dir * (overlap * 0.5f);
        hp = std::max(0.f, hp - other.body_dmg*dmging);
        other.hp = std::max(0.f, other.hp - body_dmg*dmging);
    }
    else {
        int time_collosions=std::min(std::ceil(hp/other.body_dmg),std::ceil(other.hp/body_dmg));
        hp=std::max(0.f,hp-time_collosions*other.body_dmg);
        other.hp=std::max(0.f,other.hp-time_collosions*body_dmg);
    }
    if(bounce){
        float pushForce = (hypot2(velocity)+hypot2(other.velocity))/1.5*bounceScale;
        velocity += dir * pushForce;
        other.velocity -= dir * pushForce;
    }
}
bool Obj::isCollidingWith(const Obj& other) const {
    float dx = position.x - other.position.x;
    float dy = position.y - other.position.y;
    float dist = std::hypot(dx, dy);
    return dist < (hitbox_r + other.hitbox_r);
}