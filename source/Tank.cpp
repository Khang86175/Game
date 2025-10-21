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
void Cannon::SetOrigin(float x, float y,float size){
    gun.setPosition({x,y});
    gun.setOrigin({0,size/3});
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
    : gun(x,y,size, 60){}
void TankBasic::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
    gun.DrawGun(window);
    window.draw(bodyShape);
}
void TankBasic::update(sf::Vector2f pos, int angle){
    gun.update(pos, angle);
}
void TankBasic::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type){
    if (gun.delay <= 0){
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
    if (gun1.delay <= 0 && gun2.delay <= 0){
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
    if (gun1.delay <= 0 && gun2.delay <= 0 && gun3.delay <= 0){
        bullets.push_back(Bullet(gun1.position,angle-30,gun1.size.y/2,gun1.size.x,bSpeed,bLife,bDmg,type));
        gun1.delay = gun1.reload;
        bullets.push_back(Bullet(gun2.position,angle,gun2.size.y/2,gun2.size.x,bSpeed,bLife,bDmg,type));
        gun2.delay = gun2.reload;
        bullets.push_back(Bullet(gun3.position,angle+30,gun3.size.y/2,gun3.size.x,bSpeed,bLife,bDmg,type));
        gun3.delay = gun3.reload;
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
    if (gun.delay <= 0) {
        bullets.push_back(Bullet(gun.position, rand() % 36 + angle - 18, gun.size.y*0.25,gun.size.x, bSpeed, bLife, bDmg*0.5,type));
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
        bullets.push_back(Bullet(gun.position, angle, gun.size.y*2/3,gun.size.x, bSpeed * 0.8f, bLife*1.3f, bDmg * 4.f,type));
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
    if (gun.delay <= 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y/2,gun.size.x, bSpeed * 1.3f, bLife * 1.5f, bDmg * 1.5f,type));
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
    if (gun.delay <= 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y/2,gun.size.x, bSpeed * 1.6f, bLife * 1.3f, bDmg * 2.f,type));
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
    body.setHpRegen(base_hp_regen * (1+stats.hp_regen)*0.12f);
    body.setMaxHp(base_maxhp * pow(1.2,(1+stats.maxhp)));
    body.setBodyDmg(base_body_dmg * (1+stats.body_dmg)*0.2f);
    acceleration = base_acceleration +stats.move_speed*0.02f;
    int newReload = base_reload - stats.reload*4;
    switch (tankType) {
        case TankType::BASIC:
            if (tankBasic) tankBasic->setReload(newReload);
            break;
        case TankType::TWIN:
            if (tankTwin) tankTwin->setReload(newReload);
            break;
        case TankType::SNIPER:
            if (tankSniper) tankSniper->setReload(newReload*1.25);
            break;
        case TankType::MACHINE_GUN:
            if (tankMachineGun) tankMachineGun->setReload(newReload/4);
            break;
        case TankType::TRIPLE:
            if (tankTriple) tankTriple->setReload(newReload);
            break;
        case TankType::ASSASSIN:
            if (tankAssassin) tankAssassin->setReload(newReload*1.5);
            break;
        case TankType::DESTROYER:
            if (tankDestroyer) tankDestroyer->setReload(newReload*2);
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
    tankSniper = nullptr;
    tankMachineGun = nullptr;
    tankTriple = nullptr;
    tankAssassin = nullptr;
    tankDestroyer = nullptr;
    statPoint = 0;
    stats = {0,0,0,0,0,0,0,0};
    body.velocity={0,0};
    body.hitbox_r=size;
    bodyShape.setRadius(size);
    bodyShape.setOrigin({size,size});
    
    tankBasic = new TankBasic(x, y, size);
    tankBasic->gun.SetOrigin(x,y,size);
}

// ENEMYTANK IS HERE
EnemyTank::EnemyTank(float x, float y, float size, float mapsize,TankType tankType)
    : body(x,y,mapsize,size,100,0.2f,2),tankType(tankType),hp_bar(50, 5, size+10),alive(false),Xp_reward(100),timetorespawn(1800){
    bodyShape.setRadius(size);
    bodyShape.setFillColor(sf::Color(239,77,85));
    bodyShape.setOutlineThickness(5.f);
    bodyShape.setOutlineColor(sf::Color(205,65,70));
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
void EnemyTank::NewEnemy(float x, float y, float size, TankType tanktype, int level){
    Xp_reward=level*50;
    alive=true;
    bodyShape.setRadius(size);
    bodyShape.setFillColor(sf::Color(239,77,85));
    bodyShape.setOutlineThickness(5.f);
    bodyShape.setOutlineColor(sf::Color(205,65,70));
    bodyShape.setOrigin({size,size});
    body.position={x,y};
    body.velocity={0,0};

    tankBasic = nullptr;
    tankTwin = nullptr;
    tankSniper = nullptr;
    tankMachineGun = nullptr;
    tankTriple = nullptr;
    tankAssassin = nullptr;
    tankDestroyer = nullptr;

    tankType=tanktype;
    switch (tankType) {
        case TankType::BASIC:
            tankBasic = new TankBasic(x,y,size);
            break;
        case TankType::TWIN:
            tankTwin = new TankTwin(x,y,size);
            break;
        case TankType::SNIPER:
            tankSniper = new TankSniper(x,y,size);
            break;
        case TankType::MACHINE_GUN:
            tankMachineGun = new TankMachineGun(x,y,size);
            break;
        case TankType::TRIPLE:
            tankTriple = new TankTriple(x,y,size);
            break;
        case TankType::ASSASSIN:
            tankAssassin = new TankAssassin(x,y,size);
            break;
        case TankType::DESTROYER:
            tankDestroyer = new TankDestroyer(x,y,size);
            break;
    }
    int base=level/10;
    baselevel=(ceil)((float)level)/7;
    baselevel+=base;
    body.setHpRegen((base_hp_regen * (1+stats.hp_regen+base)*0.12f)/2);
    body.setMaxHp(base_maxhp * pow(1.2,(1+stats.maxhp+base)));
    body.hp=body.maxhp;
    body.setBodyDmg(base_body_dmg * (1+stats.body_dmg+base)*0.2f);
    acceleration = base_acceleration + (stats.move_speed+base)*0.04f;
    if(acceleration > 0.6)
        acceleration = 0.6;
    int newReload = base_reload - stats.reload*4;
    switch (tankType) {
        case TankType::BASIC:
            if (tankBasic) tankBasic->setReload(newReload);
            break;
        case TankType::TWIN:
            if (tankTwin) tankTwin->setReload(newReload);
            break;
        case TankType::SNIPER:
            if (tankSniper) tankSniper->setReload(newReload*1.25);
            break;
        case TankType::MACHINE_GUN:
            if (tankMachineGun) tankMachineGun->setReload(newReload/4);
            break;
        case TankType::TRIPLE:
            if (tankTriple) tankTriple->setReload(newReload);
            break;
        case TankType::ASSASSIN:
            if (tankAssassin) tankAssassin->setReload(newReload*1.5);
            break;
        case TankType::DESTROYER:
            if (tankDestroyer) tankDestroyer->setReload(newReload*2);
            break;
    }

}
EnemyTank::~EnemyTank(){
    if (tankBasic) delete tankBasic;
    if (tankTwin) delete tankTwin;
    if (tankSniper) delete tankSniper;
    if (tankMachineGun) delete tankMachineGun;
    if (tankTriple) delete tankTriple;
    if (tankAssassin) delete tankAssassin;
    if (tankDestroyer) delete tankDestroyer;
}
void EnemyTank::update(int angle){

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
float EnemyTank::getBulletSpeed(){
    return base_bullet_speed * (1 + stats.bullet_speed * 0.15f)*1.5;
}
int EnemyTank::getBulletLife(){
    return base_bullet_life + stats.bullet_penetration * 30;
}
float EnemyTank::getBulletDamage(){
    return base_bullet_dmg * (1 + stats.bullet_dmg * 0.4f);
}
void EnemyTank::Drawtank(sf::RenderWindow &window){
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
void EnemyTank::shoot(std::vector<Bullet> &bullets, int angle){
    float bSpeed = (base_bullet_speed * (1 + stats.bullet_speed * 0.15f))*1.5;
    if(bSpeed > 15)
        bSpeed=15;
    int bLife = base_bullet_life + (stats.bullet_penetration+baselevel) * 30;
    if(bLife>600)
        bLife=600;
    float bDmg = base_bullet_dmg * (1 + (stats.bullet_dmg+baselevel) * 0.4f);

    switch (tankType) {
        case TankType::BASIC:
            if (tankBasic) tankBasic->shoot(bullets, angle, bSpeed, bLife, bDmg,1);
            break;
        case TankType::TWIN:
            if (tankTwin) tankTwin->shoot(bullets, angle, bSpeed, bLife, bDmg,1);
            break;
        case TankType::SNIPER:
            if (tankSniper) tankSniper->shoot(bullets, angle, bSpeed, bLife, bDmg,1);
            break;
        case TankType::MACHINE_GUN:
            if (tankMachineGun) tankMachineGun->shoot(bullets, angle, bSpeed, bLife, bDmg,1);
            break;
        case TankType::TRIPLE:
            if (tankTriple) tankTriple->shoot(bullets, angle, bSpeed, bLife, bDmg,1);
            break;
        case TankType::ASSASSIN:
            if (tankAssassin) tankAssassin->shoot(bullets, angle, bSpeed, bLife, bDmg,1);
            break;
        case TankType::DESTROYER:
            if (tankDestroyer) tankDestroyer->shoot(bullets, angle, bSpeed, bLife, bDmg,1);
            break;
    }
}
TankType EnemyTank::getTankType(){
    return tankType;
}
void EnemyTank::Die(MyTank& myTank){
    timetorespawn=currentFrame+fps*(rand()%30+30);
    alive=false;
    if (tankBasic) delete tankBasic;
    if (tankTwin) delete tankTwin;
    if (tankSniper) delete tankSniper;
    if (tankMachineGun) delete tankMachineGun;
    if (tankTriple) delete tankTriple;
    if (tankAssassin) delete tankAssassin;
    if (tankDestroyer) delete tankDestroyer;
    myTank.score+=Xp_reward;
}
int EnemyTank::NextMove(MyTank &target) {
    // --- Các hằng số để điều chỉnh hành vi của bot ---
    const float PI = 3.14159265f;
    const float MIN_COMBAT_DISTANCE = 300.f; // Tăng khoảng cách để có không gian né
    const float MAX_COMBAT_DISTANCE = 450.f;
    const float COLLISION_BUFFER = 50.f;

    // ==========================================================
    // PHẦN 1: TÍNH TOÁN GÓC BẮN DỰ ĐOÁN
    // ==========================================================

    float dx = target.body.position.x - this->body.position.x;
    float dy = target.body.position.y - this->body.position.y;
    
    // Góc mặc định là bắn thẳng vào vị trí hiện tại của mục tiêu
    float aimAngleRad = std::atan2(dy, dx);

    // Lấy các thông số cần thiết
    float bulletSpeed = this->getBulletSpeed();
    float bulletLife = this->getBulletLife(); // Lấy thời gian tồn tại của đạn
    float targetVelX = target.body.velocity.x;
    float targetVelY = target.body.velocity.y;

    // Giải phương trình bậc hai a*t^2 + b*t + c = 0 để tìm thời gian va chạm (t)
    float a = targetVelX * targetVelX + targetVelY * targetVelY - bulletSpeed * bulletSpeed;
    float b = 2 * (dx * targetVelX + dy * targetVelY);
    float c = dx * dx + dy * dy;

    float discriminant = b * b - 4 * a * c;

    if (discriminant >= 0) {
        float t1 = (-b - std::sqrt(discriminant)) / (2 * a); // Tìm nghiệm nhỏ hơn trước
        float t2 = (-b + std::sqrt(discriminant)) / (2 * a);

        float timeToIntercept = -1.f;
        if (t1 > 0 && t2 > 0) {
            timeToIntercept = std::min(t1, t2);
        } else {
            timeToIntercept = std::max(t1, t2); // Lấy nghiệm dương duy nhất nếu có
        }
        
        // *** ĐIỀU KIỆN MỚI: KIỂM TRA THỜI GIAN SỐNG CỦA ĐẠN ***
        // Chỉ nhắm bắn dự đoán nếu thời gian va chạm là hợp lệ VÀ đạn còn tồn tại
        if (timeToIntercept > 0 && timeToIntercept < bulletLife) {
            float interceptX = target.body.position.x + targetVelX * timeToIntercept;
            float interceptY = target.body.position.y + targetVelY * timeToIntercept;
            aimAngleRad = std::atan2(interceptY - this->body.position.y, interceptX - this->body.position.x);
        }
        // Nếu không, bot sẽ dùng góc bắn thẳng mặc định đã tính ở trên.
    }

    // ==========================================================
    // PHẦN 2: LOGIC DI CHUYỂN
    // ==========================================================
    
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > 1.f) {
        float normDx = dx / dist;
        float normDy = dy / dist;

        // Ưu tiên 1: Tránh va chạm
        if (dist < this->body.hitbox_r + target.body.hitbox_r + COLLISION_BUFFER) {
            this->body.velocity.x -= normDx * this->acceleration;
            this->body.velocity.y -= normDy * this->acceleration;
        }
        // Ưu tiên 2: Giữ khoảng cách chiến đấu
        else if (dist > MAX_COMBAT_DISTANCE) {
            this->body.velocity.x += normDx * this->acceleration;
            this->body.velocity.y += normDy * this->acceleration;
        }
        else if (dist < MIN_COMBAT_DISTANCE) {
            this->body.velocity.x -= normDx * this->acceleration;
            this->body.velocity.y -= normDy * this->acceleration;
        }
        // Ưu tiên 3: Di chuyển ngang (strafe) để đối phó với mục tiêu di chuyển cắt mặt
        else {
            // Vector vuông góc với (normDx, normDy) là (-normDy, normDx)
            // Giúp bot vừa giữ khoảng cách, vừa né đạn của mục tiêu
            this->body.velocity.x += -normDy * this->acceleration;
            this->body.velocity.y += normDx * this->acceleration;
        }
    }

    // ==========================================================
    // PHẦN 3: TRẢ VỀ GÓC BẮN (THEO ĐỘ)
    // ==========================================================
    return static_cast<int>(aimAngleRad * 180.f / PI);
}