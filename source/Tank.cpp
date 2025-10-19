#include "Tank.hpp"

bool canEvolveTo(TankType currentType, TankType targetType){
    if (currentType == TankType::BASIC) {
        return (targetType == TankType::TWIN || targetType == TankType::SNIPER || targetType == TankType::MACHINE_GUN);
    }
    if (currentType == TankType::TWIN) return targetType == TankType::TRIPLE;
    if (currentType == TankType::SNIPER) return targetType == TankType::ASSASSIN;
    if (currentType == TankType::MACHINE_GUN) return targetType == TankType::DESTROYER;
    return false;
}
bool isMaxEvolution(TankType type){
    return (type == TankType::TRIPLE || type == TankType::ASSASSIN || type == TankType::DESTROYER);
}

Cannon::Cannon(float x, float y,float size, float reload):position({x,y}),angle(0),delay(0), reload(reload), size(size*2, size*2/3){
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
MachineGunCannon::MachineGunCannon(float x, float y,float size, float reload):position({x,y}),angle(0),delay(0), reload(reload), size(size*1.2, size*1.8){
    gun.setPointCount(4);
    float length = size * 1.8f;
    float backWidth = size*0.7;
    float frontWidth = size * 1.2f;
    gun.setPoint(0, sf::Vector2f(0, -backWidth/2));
    gun.setPoint(1, sf::Vector2f(length, -frontWidth));
    gun.setPoint(1, sf::Vector2f(length, frontWidth));
    gun.setPoint(3, sf::Vector2f(0, backWidth/2));

    gun.setFillColor(sf::Color(153,153,153));
    gun.setOutlineThickness(5.f);
    gun.setOutlineColor(sf::Color(102,102,102));
    gun.setOrigin({0,size/3});
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

TankBasic::TankBasic(float x, float y, float size) : gun(x,y,size, 30){}
void TankBasic::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
    gun.DrawGun(window);
    window.draw(bodyShape);
}
void TankBasic::update(sf::Vector2f pos, int angle){
    gun.update(pos, angle);
}
void TankBasic::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg){
    if (gun.delay == 0){
        bullets.push_back(Bullet(gun.position,angle,gun.size.y/2,bSpeed,bLife,bDmg));
        gun.delay = gun.reload;
    }
}

TankTwin::TankTwin(float x, float y, float size): gun1(x, y, size, 30), gun2(x, y, size, 30), gunOffSet(size/2) {}
void TankTwin::update(sf::Vector2f pos, float angle){
    float offSetX = gunOffSet * cos((angle + 90) * 3.14f / 180.f);
    float offsetY = gunOffSet * sin((angle + 90) * 3.14f / 180.f);
    sf::Vector2f posGun1(pos.x - offSetX, pos.y - offsetY);
    sf::Vector2f posGun2(pos.x + offSetX, pos.y + offsetY);
    gun1.update(posGun1, angle);
    gun2.update(posGun2, angle);
}
void TankTwin::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg){
    if (gun1.delay == 0 && gun2.delay == 0){
        bullets.push_back(Bullet(gun1.position,angle,gun1.size.y/2,bSpeed,bLife,bDmg));
        gun1.delay = gun1.reload;
        bullets.push_back(Bullet(gun2.position,angle,gun2.size.y/2,bSpeed,bLife,bDmg));
        gun2.delay = gun2.reload;
    }
}
void TankTwin::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
    gun1.DrawGun(window);
    gun2.DrawGun(window);
    window.draw(bodyShape);
}

TankTriple::TankTriple(float x, float y, float size): gun1(x, y, size, 30), gun2(x, y, size, 30), gun3(x, y, size, 30), gunOffSet(size*0.6){}
void TankTriple::update(sf::Vector2f pos, float angle){
    float offSetX = gunOffSet * cos((angle + 90) * 3.14f / 180.f);
    float offsetY = gunOffSet * sin((angle + 90) * 3.14f / 180.f);
    sf::Vector2f posGun1(pos.x - offSetX, pos.y - offsetY);
    sf::Vector2f posGun3(pos.x + offSetX, pos.y + offsetY);
    gun1.update(posGun1, angle-45);
    gun2.update(pos, angle);
    gun3.update(posGun3, angle+45);
}
void TankTriple::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg){
    if (gun1.delay == 0 && gun2.delay == 0, gun3.delay == 0){
        bullets.push_back(Bullet(gun1.position,angle,gun1.size.y/2,bSpeed,bLife,bDmg));
        gun1.delay = gun1.reload;
        bullets.push_back(Bullet(gun2.position,angle,gun2.size.y/2,bSpeed,bLife,bDmg));
        gun2.delay = gun2.reload;
        bullets.push_back(Bullet(gun3.position,angle,gun3.size.y/2,bSpeed,bLife,bDmg));
        gun2.delay = gun3.reload;
    }
}
void TankTriple::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
    window.draw(bodyShape);
    gun1.DrawGun(window);
    gun2.DrawGun(window);
    gun3.DrawGun(window);
}

TankMachineGun::TankMachineGun(float x, float y, float size) : gun(x, y, size, 25){}
void TankMachineGun::update(sf::Vector2f pos, int angle) {
    gun.update(pos, angle);
}
void TankMachineGun::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg) {
    if (gun.delay == 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y/2, bSpeed * 1.3f, bLife * 1.5f, bDmg * 1.2f));
        gun.delay = gun.reload;
    }
}
void TankMachineGun::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape) {
    gun.DrawGun(window);
    window.draw(bodyShape);
}

TankDestroyer::TankDestroyer(float x, float y, float size) : gun(x, y, size * 1.3f, 60) {}
void TankDestroyer::update(sf::Vector2f pos, int angle) {
    gun.update(pos, angle);
}
void TankDestroyer::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg) {
    if (gun.delay == 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y/2 * 1.8f, bSpeed * 0.7f, bLife, bDmg * 2.f));
        gun.delay = gun.reload;
    }
}
void TankDestroyer::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape) {
    gun.DrawGun(window);
    window.draw(bodyShape);
}
TankSniper::TankSniper(float x, float y, float size) : gun(x, y, size * 1.5f, 40){}
void TankSniper::update(sf::Vector2f pos, int angle) {
    gun.update(pos, angle);
}
void TankSniper::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg) {
    if (gun.delay == 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y/2, bSpeed * 1.3f, bLife * 1.5f, bDmg * 1.2f));
        gun.delay = gun.reload;
    }
}
void TankSniper::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape) {
    gun.DrawGun(window);
    window.draw(bodyShape);
}

TankAssassin::TankAssassin(float x, float y, float size) : gun(x, y, size * 1.8f, 40){}
void TankAssassin::update(sf::Vector2f pos, int angle) {
    gun.update(pos, angle);
}
void TankAssassin::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg) {
    if (gun.delay == 0) {
        bullets.push_back(Bullet(gun.position, angle, gun.size.y/2, bSpeed * 1.3f, bLife * 1.8f, bDmg * 1.3f));
        gun.delay = gun.reload;
    }
}
void TankAssassin::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape) {
    gun.DrawGun(window);
    window.draw(bodyShape);
}

MyTank::MyTank(float x, float y, float size,float mapsize):body(x,y,mapsize,size,100, 0.2f, 2),score(0),level(1), xp_base(0), xp_to_lv_up(30), tankType(TankType::BASIC), tankTwin(nullptr), statPoint(0){
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
    // hàm clamp giới hạn tốc độ
    // hàm hypot(x,y) = sqrt(x*x+y*y)
    body.velocity*=friction;

    //dpos=std::hypot(body.velocity.x,body.velocity.y);
    //if(dpos>velocity_max){
    //    body.velocity.x=body.velocity.x/dpos*velocity_max;
    //    body.velocity.y=body.velocity.y/dpos*velocity_max;
    //}
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
}
void MyTank::applyStats(){
    body.setHpRegen(base_hp_regen * (1+stats.hp_regen*0.15f));
    body.setMaxHp(base_maxhp * (1+stats.maxhp*0.2f));
    body.setBodyDmg(base_body_dmg * (1+stats.body_dmg*0.25f));
    acceleration = base_acceleration * (1+stats.move_speed*0.06f);
    float newReload = base_reload - stats.reload*2;
    if (tankType == 0){
        tankBasic->gun.setReload(newReload);
    }
    else if (tankType == 1){
        tankTwin->gun1.setReload(newReload);
        tankTwin->gun2.setReload(newReload);
    }
}
void MyTank::addScore(int point){
    score += point;
}
void MyTank::levelUp(){
    if (score >= xp_to_lv_up){
        level += 1;
        int temp=(xp_to_lv_up-xp_base)*(1.5-level/10);
        xp_base = xp_to_lv_up;
        xp_to_lv_up += temp;
        statPoint += 1;

        body.hp = body.maxhp;
    }
    if (level >= 5 && tankType == 0){
        delete tankBasic;
        tankBasic = nullptr;

        bodyShape.setRadius(bodyShape.getRadius() + 5);
        bodyShape.setOrigin({bodyShape.getRadius(), bodyShape.getRadius()});
        body.hitbox_r = bodyShape.getRadius();

        tankTwin = new TankTwin(body.position.x, body.position.y, bodyShape.getRadius());
        tankType = 1;
    }
}
bool MyTank::canEvolve() {
    if (isMaxEvolution(tankType)) return false;
    if (tankType == TankType::BASIC && level >= 10) return true;
    if ((tankType == TankType::TWIN || tankType == TankType::SNIPER || tankType == TankType::MACHINE_GUN) && level >= 20) {
        return true;
    }
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
    float radius = bodyShape.getRadius();
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
    return base_bullet_life + stats.bullet_penetration * 20;
}
float MyTank::getBulletDamage(){
    return base_bullet_dmg * (1 + stats.bullet_dmg * 0.2f);
}
void MyTank::Drawtank(sf::RenderWindow &window){
    if (tankType == 0){
        tankBasic->drawTank(window, bodyShape);
    }
    else if (tankType == 1){
        tankTwin->drawTank(window, bodyShape);
    }
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
    int bLife = base_bullet_life + stats.bullet_penetration * 15;
    float bDmg = base_bullet_dmg * (1 + stats.bullet_dmg * 0.2f);

    if (tankType == 0){
        tankBasic->shoot(bullets, angle, bSpeed, bLife, bDmg);
    }
    else if (tankType == 1){
        tankTwin->shoot(bullets, angle, bSpeed, bLife, bDmg);
    }
}
int MyTank::getTankType(){
    return tankType;
}
