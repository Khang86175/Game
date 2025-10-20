#include "Tank.hpp"
#include <iostream>

bool canEvolveTo(TankType currentType, TankType targetType){
    if (currentType == TankType::BASIC) {
        return (targetType == TankType::TWIN || targetType == TankType::SNIPER);
    }
    if (currentType == TankType::TWIN) return (targetType == TankType::TRIPLE || targetType == TankType::MACHINE_GUN);
    if (currentType == TankType::SNIPER) return targetType == TankType::ASSASSIN;
    if (currentType == TankType::MACHINE_GUN) return targetType == TankType::DESTROYER;
    return false;
}
bool isMaxEvolution(TankType type){
    return (type == TankType::TRIPLE || type == TankType::ASSASSIN || type == TankType::DESTROYER);
}

//CANNON IS HERE
Cannon::Cannon(float x, float y,float size, float reload)
    : position({x,y}),angle(0),delay(0), reload(reload), size(size*2, size*2/3){
    gun.setSize({size*2, size*2/3});
    gun.setFillColor(sf::Color(153,153,153));
    gun.setOutlineThickness(5.f);
    gun.setOutlineColor(sf::Color(102,102,102));
    gun.setOrigin({0,size/3});
    gun.setPosition({x,y});
}
void Cannon::update(sf::Vector2f pos, int angle){
    if(delay>0)
        delay-=1;
    position=pos;
    gun.setPosition(pos);
    gun.setRotation(sf::degrees(angle));
}
void Cannon::DrawGun(sf::RenderWindow &window){
    window.draw(gun);
}
void Cannon::setReload(int new_reload){
    reload = new_reload;
}  

MachineGunCannon::MachineGunCannon(float x, float y,float size, float reload):position({x,y}),angle(0),delay(0), reload(reload), size(size, size){
    gun.setPointCount(4);
    float length = size * 1.8f;
    float backWidth = size*0.5f;
    float frontWidth = size*0.7f;
    gun.setPoint(0, sf::Vector2f(0, -backWidth/2));
    gun.setPoint(1, sf::Vector2f(length, -frontWidth));
    gun.setPoint(2, sf::Vector2f(length, frontWidth));
    gun.setPoint(3, sf::Vector2f(0, backWidth/2));

    gun.setFillColor(sf::Color(153,153,153));
    gun.setOutlineThickness(5.f);
    gun.setOutlineColor(sf::Color(102,102,102));
    gun.setOrigin({0,0});
    gun.setPosition({x,y});
}
void MachineGunCannon::update(sf::Vector2f pos, int angle){
    if(delay>0)
        delay-=1;
    position=pos;
    gun.setPosition(pos);
    gun.setRotation(sf::degrees(angle));
}
void MachineGunCannon::DrawGun(sf::RenderWindow &window){
    window.draw(gun);
}
void MachineGunCannon::setReload(int new_reload){
    reload = new_reload;
}

DestroyerCannon::DestroyerCannon(float x, float y,float size, float reload):position({x,y}),angle(0),delay(0), reload(reload), size(size*2, size*2/3){
    gun.setSize({size*1.5, size});
    gun.setFillColor(sf::Color(153,153,153));
    gun.setOutlineThickness(5.f);
    gun.setOutlineColor(sf::Color(102,102,102));
    gun.setOrigin({0,size/2});
    gun.setPosition({x,y});
}
void DestroyerCannon::update(sf::Vector2f pos, int angle){
    if(delay>0)
        delay-=1;
    position=pos;
    gun.setPosition(pos);
    gun.setRotation(sf::degrees(angle));
}
void DestroyerCannon::DrawGun(sf::RenderWindow &window){
    window.draw(gun);
}
void DestroyerCannon::setReload(int new_reload){
    reload = new_reload;
}

//TANKBASIC IS HERE
TankBasic::TankBasic(float x, float y, float size)
    : gun(x,y,size, 30){}
void TankBasic::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
    gun.DrawGun(window);
    window.draw(bodyShape);
}
void TankBasic::update(sf::Vector2f pos, int angle){
    gun.update(pos, angle);
}
void TankBasic::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type){
    if (gun.delay == 0){
        bullets.push_back(Bullet(gun.position,angle,gun.size.y/2,gun.size.x,bSpeed,bLife,bDmg,type));
        gun.delay = gun.reload;
    }
}
void TankBasic::setReload(int new_reload){
    gun.setReload(new_reload);
}

//TANKTWIN IS HERE
TankTwin::TankTwin(float x, float y, float size)
    : gun1(x, y, size, 30), gun2(x, y, size, 30), gunOffSet(size/2) {}
void TankTwin::update(sf::Vector2f pos, float angle){
    float offSetX = gunOffSet * cos((angle + 90) * 3.14f / 180.f);
    float offsetY = gunOffSet * sin((angle + 90) * 3.14f / 180.f);
    sf::Vector2f posGun1(pos.x - offSetX, pos.y - offsetY);
    sf::Vector2f posGun2(pos.x + offSetX, pos.y + offsetY);
    gun1.update(posGun1, angle);
    gun2.update(posGun2, angle);
}
void TankTwin::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type){
    if (gun1.delay == 0 && gun2.delay == 0){
        bullets.push_back(Bullet(gun1.position,angle,gun1.size.y/2,gun1.size.x,bSpeed,bLife,bDmg,type));
        gun1.delay = gun1.reload;
        bullets.push_back(Bullet(gun2.position,angle,gun2.size.y/2,gun2.size.x,bSpeed,bLife,bDmg,type));
        gun2.delay = gun2.reload;
    }
}
void TankTwin::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
    gun1.DrawGun(window);
    gun2.DrawGun(window);
    window.draw(bodyShape);
}
void TankTwin::setReload(int new_reload){
    gun1.setReload(new_reload);
    gun2.setReload(new_reload);
}

//TANKTRIPLE IS HERE
TankTriple::TankTriple(float x, float y, float size)
    : gun1(x, y, size-5, 30), gun2(x, y, size, 30), gun3(x, y, size-5, 30), gunOffSet(size/2){}
void TankTriple::update(sf::Vector2f pos, float angle){
    float offSetX = gunOffSet * cos((angle + 90) * 3.14f / 180.f);
    float offsetY = gunOffSet * sin((angle + 90) * 3.14f / 180.f);
    sf::Vector2f posGun1(pos.x - offSetX, pos.y - offsetY);
    sf::Vector2f posGun3(pos.x + offSetX, pos.y + offsetY);
    gun1.update(posGun1, angle-30);
    gun2.update(pos, angle);
    gun3.update(posGun3, angle+30);
}
void TankTriple::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type){
    if (gun1.delay == 0 && gun2.delay == 0 && gun3.delay == 0){
        bullets.push_back(Bullet(gun1.position,angle-30,gun1.size.y/2,gun1.size.x,bSpeed,bLife,bDmg,type));
        gun1.delay = gun1.reload;
        bullets.push_back(Bullet(gun2.position,angle,gun2.size.y/2,gun2.size.x,bSpeed,bLife,bDmg,type));
        gun2.delay = gun2.reload;
        bullets.push_back(Bullet(gun3.position,angle+30,gun3.size.y/2,gun3.size.x,bSpeed,bLife,bDmg,type));
        gun2.delay = gun3.reload;
    }
}
void TankTriple::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
    gun1.DrawGun(window);
    gun2.DrawGun(window);
    gun3.DrawGun(window);
    window.draw(bodyShape);
}
 void TankTriple::setReload(int new_reload){
    gun1.setReload(new_reload);
    gun2.setReload(new_reload);
    gun3.setReload(new_reload);
 }

TankMachineGun::TankMachineGun(float x, float y, float size) 
    : gun(x, y, size, 5){}
void TankMachineGun::update(sf::Vector2f pos, int angle) {
    gun.update(pos, angle);
}
void TankMachineGun::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type) {
    if (gun.delay == 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y*0.4,gun.size.x, bSpeed, bLife, bDmg,type));
        gun.delay = gun.reload;
    }
}
void TankMachineGun::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape) {
    gun.DrawGun(window);
    window.draw(bodyShape);
}
void TankMachineGun::setReload(int new_reload){
    gun.setReload(new_reload);
}

TankDestroyer::TankDestroyer(float x, float y, float size) 
    : gun(x, y, size * 1.3f, 45) {}
void TankDestroyer::update(sf::Vector2f pos, int angle) {
    gun.update(pos, angle);
}
void TankDestroyer::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type) {
    if (gun.delay == 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y*2/3,gun.size.x, bSpeed * 0.8f, bLife*1.3f, bDmg * 2.f,type));
        gun.delay = gun.reload;
    }
}
void TankDestroyer::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape) {
    gun.DrawGun(window);
    window.draw(bodyShape);
}
void TankDestroyer::setReload(int new_reload){
    gun.setReload(new_reload);
}

TankSniper::TankSniper(float x, float y, float size) 
    : gun(x, y, size * 1.3f, 40){}
void TankSniper::update(sf::Vector2f pos, int angle) {
    gun.update(pos, angle);
}
void TankSniper::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type) {
    if (gun.delay == 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y/2,gun.size.x, bSpeed * 1.3f, bLife * 1.5f, bDmg * 1.2f,type));
        gun.delay = gun.reload;
    }
}
void TankSniper::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape) {
    gun.DrawGun(window);
    window.draw(bodyShape);
}
void TankSniper::setReload(int new_reload){
    gun.setReload(new_reload);
}

TankAssassin::TankAssassin(float x, float y, float size) : gun(x, y, size * 1.5f, 40){}
void TankAssassin::update(sf::Vector2f pos, int angle) {
    gun.update(pos, angle);
}
void TankAssassin::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type) {
    if (gun.delay == 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y/2,gun.size.x, bSpeed * 1.3f, bLife * 1.8f, bDmg * 1.3f,type));
        gun.delay = gun.reload;
    }
}
void TankAssassin::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape) {
    gun.DrawGun(window);
    window.draw(bodyShape);
}
void TankAssassin::setReload(int new_reload){
    gun.setReload(new_reload);
}
//MYTANK IS HERE
MyTank::MyTank(float x, float y, float size,float mapsize)
    : body(x,y,mapsize,size,100, 0.2f, 2),score(0),level(1), xp_base(0), xp_to_lv_up(30), tankType(TankType::BASIC), statPoint(0),hp_bar(50, 5, size+10){
    bodyShape.setRadius(size);
    bodyShape.setFillColor(sf::Color(0,178,225));
    bodyShape.setOutlineThickness(5.f);
    bodyShape.setOutlineColor(sf::Color(14, 144, 178));
    bodyShape.setOrigin({size,size});
    bodyShape.setPosition({x,y});
    body.position={x,y};
    body.velocity={0,0};
    friction=0.94;
    tankBasic = new TankBasic(x, y, size);

    tankTwin = nullptr;
    tankSniper = nullptr;
    tankMachineGun = nullptr;
    tankTriple = nullptr;
    tankAssassin = nullptr;
    tankDestroyer = nullptr;
}
MyTank::~MyTank(){
    if (tankBasic) delete tankBasic;
    if (tankTwin) delete tankTwin;
    if (tankSniper) delete tankSniper;
    if (tankMachineGun) delete tankMachineGun;
    if (tankTriple) delete tankTriple;
    if (tankAssassin) delete tankAssassin;
    if (tankDestroyer) delete tankDestroyer;
}
void MyTank::update(int angle){
    body.velocity*=friction;

    if(body.velocity.x<0.01 && body.velocity.x>-0.01)
        body.velocity.x=0;
    if(body.velocity.y<0.01 && body.velocity.y>-0.01)
        body.velocity.y=0;
    body.update();

    switch (tankType) {
        case TankType::BASIC:
            if (tankBasic) tankBasic->update(body.position, angle);
            break;
        case TankType::TWIN:
            if (tankTwin) tankTwin->update(body.position, angle);
            break;
        case TankType::SNIPER:
            if (tankSniper) tankSniper->update(body.position, angle);
            break;
        case TankType::MACHINE_GUN:
            if (tankMachineGun) tankMachineGun->update(body.position, angle);
            break;
        case TankType::TRIPLE:
            if (tankTriple) tankTriple->update(body.position, angle);
            break;
        case TankType::ASSASSIN:
            if (tankAssassin) tankAssassin->update(body.position, angle);
            break;
        case TankType::DESTROYER:
            if (tankDestroyer) tankDestroyer->update(body.position, angle);
            break;
    }
    bodyShape.setPosition(body.position);
    hp_bar.update(body);
}
void MyTank::applyStats(){
    body.setHpRegen(base_hp_regen * (1+stats.hp_regen*0.12f));
    body.setMaxHp(base_maxhp * pow(1.2,(1+stats.maxhp)));
    body.setBodyDmg(base_body_dmg * (1+stats.body_dmg*0.2f));
    acceleration = base_acceleration * (1+stats.move_speed*0.06f);
    float newReload = base_reload - stats.reload*4;
     switch (tankType) {
        case TankType::BASIC:
            if (tankBasic) tankBasic->setReload(newReload);
            break;
        case TankType::TWIN:
            if (tankTwin) tankTwin->setReload(newReload);
            break;
        case TankType::SNIPER:
            if (tankSniper) tankSniper->setReload(newReload);
            break;
        case TankType::MACHINE_GUN:
            if (tankMachineGun) tankMachineGun->setReload(newReload);
            break;
        case TankType::TRIPLE:
            if (tankTriple) tankTriple->setReload(newReload);
            break;
        case TankType::ASSASSIN:
            if (tankAssassin) tankAssassin->setReload(newReload);
            break;
        case TankType::DESTROYER:
            if (tankDestroyer) tankDestroyer->setReload(newReload);
            break;
    }
}
void MyTank::addScore(int point){
    score += point;
}
void MyTank::levelUp(){
    while (score >= xp_to_lv_up){
        level += 1;
        int temp=(xp_to_lv_up-xp_base)*(std::max(1.35-level/10,1.05));
        xp_base = xp_to_lv_up;
        xp_to_lv_up += temp;
        if(level<=50)
            statPoint += 1;

        body.hp = body.maxhp;
    }
}
bool MyTank::canEvolve() {
    // std::cout << "=== canEvolve() ===" << std::endl;
    // std::cout << "tankType: " << static_cast<int>(tankType) << std::endl;
    // std::cout << "level: " << level << std::endl;
    // std::cout << "isMaxEvolution: " << isMaxEvolution(tankType) << std::endl;
    if (isMaxEvolution(tankType)) return false;
    if (tankType == TankType::BASIC && level >= 10) return true;
    if ((tankType == TankType::TWIN || tankType == TankType::SNIPER) && level >= 20) {
        return true;
    }
    if((tankType == TankType::MACHINE_GUN) && level >=30){
        // std::cout << "MACHINE_GUN peut évoluer!" << std::endl;
        return true;
    }
    //  std::cout << "Aucune évolution possible" << std::endl;
    return false;
}
bool MyTank::evolveTank(TankType newType) {
    if (!canEvolveTo(tankType, newType)) return false;
    // Xóa tank cũ
    if (tankBasic) { delete tankBasic; tankBasic = nullptr; }
    if (tankTwin) { delete tankTwin; tankTwin = nullptr; }
    if (tankSniper) { delete tankSniper; tankSniper = nullptr; }
    if (tankMachineGun) { delete tankMachineGun; tankMachineGun = nullptr; }
    if (tankTriple) { delete tankTriple; tankTriple = nullptr; }
    if (tankAssassin) { delete tankAssassin; tankAssassin = nullptr; }
    if (tankDestroyer) { delete tankDestroyer; tankDestroyer = nullptr; }

    tankType = newType;
    float radius = bodyShape.getRadius() + 5;
    bodyShape.setRadius(radius);
    bodyShape.setOrigin({radius, radius});
    body.hitbox_r = radius;
    hp_bar.setOffSet(radius + 5);
    
    // Tạo tank mới
    switch (newType) {
        case TankType::BASIC:
            tankBasic = new TankBasic(body.position.x, body.position.y, radius);
            break;
        case TankType::TWIN:
            tankTwin = new TankTwin(body.position.x, body.position.y, radius);
            break;
        case TankType::SNIPER:
            tankSniper = new TankSniper(body.position.x, body.position.y, radius);
            break;
        case TankType::MACHINE_GUN:
            tankMachineGun = new TankMachineGun(body.position.x, body.position.y, radius);
            break;
        case TankType::TRIPLE:
            tankTriple = new TankTriple(body.position.x, body.position.y, radius);
            break;
        case TankType::ASSASSIN:
            tankAssassin = new TankAssassin(body.position.x, body.position.y, radius);
            break;
        case TankType::DESTROYER:
            tankDestroyer = new TankDestroyer(body.position.x, body.position.y, radius);
            break;
    }
    applyStats();
    return true;
}
float MyTank::getBulletSpeed(){
    return base_bullet_speed * (1 + stats.bullet_speed * 0.15f);
}
int MyTank::getBulletLife(){
    return base_bullet_life + stats.bullet_penetration * 30;
}
float MyTank::getBulletDamage(){
    return base_bullet_dmg * (1 + stats.bullet_dmg * 0.4f);
}
void MyTank::Drawtank(sf::RenderWindow &window){
    switch (tankType) {
        case TankType::BASIC:
            if (tankBasic) tankBasic->drawTank(window, bodyShape);
            break;
        case TankType::TWIN:
            if (tankTwin) tankTwin->drawTank(window, bodyShape);
            break;
        case TankType::SNIPER:
            if (tankSniper) tankSniper->drawTank(window, bodyShape);
            break;
        case TankType::MACHINE_GUN:
            if (tankMachineGun) tankMachineGun->drawTank(window, bodyShape);
            break;
        case TankType::TRIPLE:
            if (tankTriple) tankTriple->drawTank(window, bodyShape);
            break;
        case TankType::ASSASSIN:
            if (tankAssassin) tankAssassin->drawTank(window, bodyShape);
            break;
        case TankType::DESTROYER:
            if (tankDestroyer) tankDestroyer->drawTank(window, bodyShape);
            break;
    }
    if(body.hp!=body.maxhp)
        hp_bar.draw(window);
}
void MyTank::moveX(float dir){
    body.velocity.x+=acceleration*dir;
}
void MyTank::moveY(float dir){
    body.velocity.y+=acceleration*dir;
}
bool MyTank::upgradeStat(int statIndex){
    if (statPoint <= 0) return false;
    switch(statIndex){
        case 0: // Health Regen
            if(stats.hp_regen < stats.maxLevel){
                stats.hp_regen++;
                statPoint--;
                applyStats();
                return true;
            }
            break;
        case 1: // Max Health
            if(stats.maxhp < stats.maxLevel){
                stats.maxhp++;
                statPoint--;
                applyStats();
                body.hp = body.maxhp;
                return true;
            }
            break;
        case 2: // Body Damage
            if(stats.body_dmg < stats.maxLevel){
                stats.body_dmg++;
                statPoint--;
                applyStats();
                return true;
            }
            break;
        case 3: // Bullet Speed
            if(stats.bullet_speed < stats.maxLevel){
                stats.bullet_speed++;
                statPoint--;
                return true;
            }
            break;
        case 4: // Bullet Penetration
            if(stats.bullet_penetration < stats.maxLevel){
                stats.bullet_penetration++;
                statPoint--;
                return true;
            }
            break;
        case 5: // Bullet Damage
            if(stats.bullet_dmg < stats.maxLevel){
                stats.bullet_dmg++;
                statPoint--;
                return true;
            }
            break;
        case 6: // Reload
            if(stats.reload < stats.maxLevel){
                stats.reload++;
                statPoint--;
                applyStats();
                return true;
            }
            break;
        case 7: // Move Speed
            if(stats.move_speed < stats.maxLevel){
                stats.move_speed++;
                statPoint--;
                applyStats();
                return true;
            }
            break;
    }
    return false;
}
void MyTank::shoot(std::vector<Bullet> &bullets, int angle){
    float bSpeed = base_bullet_speed * (1 + stats.bullet_speed * 0.15f);
    int bLife = base_bullet_life + stats.bullet_penetration * 30;
    float bDmg = base_bullet_dmg * (1 + stats.bullet_dmg * 0.4f);

    switch (tankType) {
        case TankType::BASIC:
            if (tankBasic) tankBasic->shoot(bullets, angle, bSpeed, bLife, bDmg,0);
            break;
        case TankType::TWIN:
            if (tankTwin) tankTwin->shoot(bullets, angle, bSpeed, bLife, bDmg,0);
            break;
        case TankType::SNIPER:
            if (tankSniper) tankSniper->shoot(bullets, angle, bSpeed, bLife, bDmg,0);
            break;
        case TankType::MACHINE_GUN:
            if (tankMachineGun) tankMachineGun->shoot(bullets, angle, bSpeed, bLife, bDmg,0);
            break;
        case TankType::TRIPLE:
            if (tankTriple) tankTriple->shoot(bullets, angle, bSpeed, bLife, bDmg,0);
            break;
        case TankType::ASSASSIN:
            if (tankAssassin) tankAssassin->shoot(bullets, angle, bSpeed, bLife, bDmg,0);
            break;
        case TankType::DESTROYER:
            if (tankDestroyer) tankDestroyer->shoot(bullets, angle, bSpeed, bLife, bDmg,0);
            break;
    }
}
TankType MyTank::getTankType(){
    return tankType;
}
void MyTank::reset(float x, float y, float size, float mapsize){    
    body.position={x,y};
    body.maxhp = 100;
    body.hp = 100;
    body.hp_regen = 0.2;
    acceleration = 0.25;

    score = 0;
    level = 1;
    xp_base = 0;
    xp_to_lv_up = 30;
    tankType = TankType::BASIC;
    tankTwin = nullptr;
    statPoint = 0;
    stats = {0,0,0,0,0,0,0,0};
    body.velocity={0,0};
    body.hitbox_r=size;
    bodyShape.setRadius(size);

    tankBasic = new TankBasic(x, y, size);
}

// ENEMYTANK IS HERE
EnemyTank::EnemyTank(float x, float y, float size, float mapsize,int tankType)
    : body(x,y,mapsize,size,100,0.2f,2),tankType(tankType){
    bodyShape.setRadius(size);
    bodyShape.setFillColor(sf::Color(239,77,85));
    bodyShape.setOutlineThickness(5.f);
    bodyShape.setOutlineColor(sf::Color(205,65,70));
    bodyShape.setOrigin({size,size});
    bodyShape.setPosition({x,y});
    body.position={x,y};
    body.velocity={0,0};
    friction=0.94;
    if(tankType==0)
        tankBasic = new TankBasic(x, y, size);
    else if(tankType == 1)
        tankTwin = new TankTwin(x,y,size);
}
EnemyTank::~EnemyTank(){
    if (tankBasic) delete tankBasic;
    if (tankTwin) delete tankTwin;
}
void EnemyTank::update(int angle){

    body.velocity*=friction;

    if(body.velocity.x<0.01 && body.velocity.x>-0.01)
        body.velocity.x=0;
    if(body.velocity.y<0.01 && body.velocity.y>-0.01)
        body.velocity.y=0;
    body.update();

    if (tankType == 0){
        tankBasic->update(body.position, angle);
    }
    else if (tankType == 1){
        tankTwin->update(body.position, angle);
    }
    bodyShape.setPosition(body.position);
}
float EnemyTank::getBulletSpeed(){
    return base_bullet_speed * (1 + stats.bullet_speed * 0.15f);
}
int EnemyTank::getBulletLife(){
    return base_bullet_life + stats.bullet_penetration * 30;
}
float EnemyTank::getBulletDamage(){
    return base_bullet_dmg * (1 + stats.bullet_dmg * 0.2f);
}
void EnemyTank::Drawtank(sf::RenderWindow &window){
    if (tankType == 0){
        tankBasic->drawTank(window, bodyShape);
    }
    else if (tankType == 1){
        tankTwin->drawTank(window, bodyShape);
    }
}
void EnemyTank::shoot(std::vector<Bullet> &bullets, int angle){
    float bSpeed = base_bullet_speed * (1 + stats.bullet_speed * 0.15f);
    int bLife = base_bullet_life + stats.bullet_penetration * 30;
    float bDmg = base_bullet_dmg * (1 + stats.bullet_dmg * 0.4f);

    if (tankType == 0){
        tankBasic->shoot(bullets, angle, bSpeed, bLife, bDmg,1);
    }
    else if (tankType == 1){
        tankTwin->shoot(bullets, angle, bSpeed, bLife, bDmg,1);
    }
}
int EnemyTank::getTankType(){
    return tankType;
}





