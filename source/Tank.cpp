#include "Tank.hpp"

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

TankBasic::TankBasic(float x, float y, float size) : gun(x,y,size, 30){}
void TankBasic::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
    gun.DrawGun(window);
    window.draw(bodyShape);
}
void TankBasic::update(sf::Vector2f pos, int angle){
    gun.update(pos, angle);
}
void TankBasic::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type){
    if (gun.delay == 0){
        bullets.push_back(Bullet(gun.position,angle,gun.size.y/2,bSpeed,bLife,bDmg,type));
        gun.delay = gun.reload;
    }
}

TankTwin::TankTwin(float x, float y, float size): gun1(x, y, size, 30), gun2(x, y, size, 30), gunToggle(0), gunOffSet(size/2) {}
void TankTwin::update(sf::Vector2f pos, float angle){
    float offSetX = gunOffSet * cos((angle + 90) * 3.14f / 180.f);
    float offsetY = gunOffSet * sin((angle + 90) * 3.14f / 180.f);
    sf::Vector2f posGun1(pos.x - offSetX, pos.y - offsetY);
    sf::Vector2f posGun2(pos.x + offSetX, pos.y + offsetY);
    gun1.update(posGun1, angle);
    gun2.update(posGun2, angle);
}
void TankTwin::shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg,int type){
    if (gunToggle == 0 && gun1.delay == 0){
        bullets.push_back(Bullet(gun1.position,angle,gun1.size.y/2,bSpeed,bLife,bDmg,type));
        gun1.delay = gun1.reload;
        gunToggle = 1;
    }
    if (gunToggle == 1 && gun2.delay == 0){
            bullets.push_back(Bullet(gun2.position,angle,gun2.size.y/2,bSpeed,bLife,bDmg,type));
        gun2.delay = gun2.reload;
        gunToggle = 0;
    }
}
void TankTwin::drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
    gun1.DrawGun(window);
    gun2.DrawGun(window);
    window.draw(bodyShape);
}

MyTank::MyTank(float x, float y, float size,float mapsize)
    : body(x,y,mapsize,size,100, 0.2f, 2),score(0),level(1), xp_base(0), xp_to_lv_up(30), tankType(0), tankTwin(nullptr), statPoint(0){
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

    if (tankType == 0){
        tankBasic->update(body.position, angle);
    }
    else if (tankType == 1){
        tankTwin->update(body.position, angle);
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
    while (score >= xp_to_lv_up){
        level += 1;
        int temp=(xp_to_lv_up-xp_base)*(std::max(1.35-level/10,1.05));
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
float MyTank::getBulletSpeed(){
    return base_bullet_speed * (1 + stats.bullet_speed * 0.15f);
}
int MyTank::getBulletLife(){
    return base_bullet_life + stats.bullet_penetration * 30;
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
    int bLife = base_bullet_life + stats.bullet_penetration * 30;
    float bDmg = base_bullet_dmg * (1 + stats.bullet_dmg * 0.4f);

    if (tankType == 0){
        tankBasic->shoot(bullets, angle, bSpeed, bLife, bDmg,0);
    }
    else if (tankType == 1){
        tankTwin->shoot(bullets, angle, bSpeed, bLife, bDmg,0);
    }
}
int MyTank::getTankType(){
    return tankType;
}


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





