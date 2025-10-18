#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <ctime>


const int fps =30;
int frame=0;
constexpr float HITBOX_SCALE = 0.85f;

float hypot2(const sf::Vector2f &x){
    return hypot(x.x,x.y);
}

struct Stats{
    int hp_regen = 0;
    int maxhp = 0;
    int body_dmg = 0;
    int bullet_speed = 0;
    int bullet_penetration = 0;
    int bullet_dmg = 0;
    int reload = 0;
    int move_speed = 0;

    int maxLevel = 7;
};

class Obj{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mapsize;
    float hitbox_r;
    float hp,maxhp,hp_regen,body_dmg;

    Obj(float x, float y, float mapsize, float size, float maxhp, float hp_regen, float body_dmg)
        : position({x, y}), velocity({0, 0}), mapsize(mapsize), hitbox_r(size), maxhp(maxhp), hp(maxhp), hp_regen(hp_regen), body_dmg(body_dmg) {}
    void update(){
        position+=velocity;
        position.x=std::clamp(position.x,-mapsize+hitbox_r,mapsize-hitbox_r);
        position.y=std::clamp(position.y,-mapsize+hitbox_r,mapsize-hitbox_r);
        hp=std::min(maxhp,hp+hp_regen);
    }
    float Getx(){
        return position.x;
    }
    float Gety(){
        return position.y;
    }
    void setMaxHp(float new_maxhp){
        maxhp = new_maxhp;
    }
    void setHpRegen(float new_hp_regen){
        hp_regen = new_hp_regen;
    }
    void setBodyDmg(float new_body_dmg){
        body_dmg = new_body_dmg;
    }
    void handleCollisionWith(Obj& other,bool instant=true,bool bounce = true,float bounceScale =1) {
        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;
        float dist = std::hypot(dx, dy);
        float minDist = hitbox_r + other.hitbox_r;
        sf::Vector2f dir = { dx / dist, dy / dist };
        float overlap = (minDist - dist);

        if(instant == false){
            position += dir * (overlap * 0.5f);
            other.position -= dir * (overlap * 0.5f);
            hp = std::max(0.f, hp - other.body_dmg);
            other.hp = std::max(0.f, other.hp - body_dmg);
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
    bool isCollidingWith(const Obj& other) const {
        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;
        float dist = std::hypot(dx, dy);
        return dist < (hitbox_r + other.hitbox_r);
    }
};

class Cannon{
public:
    int delay, reload;
    float angle;
    sf::Vector2f position;
    sf::RectangleShape gun;
    sf::Vector2f size;

    Cannon(float x, float y,float size, float reload):position({x,y}),angle(0),delay(0), reload(reload), size(size*2, size*2/3){
        gun.setSize({size*2, size*2/3});
        gun.setFillColor(sf::Color(153,153,153));
        gun.setOutlineThickness(5.f);
        gun.setOutlineColor(sf::Color(102,102,102));
        gun.setOrigin({0,size/3});
        gun.setPosition({x,y});
    }
    void update(sf::Vector2f pos, int angle){
        if(delay>0)
            delay-=1;
        position=pos;
        gun.setPosition(pos);
        gun.setRotation(sf::degrees(angle));
    }
    void DrawGun(sf::RenderWindow &window){
        window.draw(gun);
    }
    void setReload(int new_reload){
        reload = new_reload;
    }  
    //void setSize(float size){
    //    gun.setSize({size*2, size*2/3});
    //}
};

class Bullet{
public:
    Obj body;
    sf::CircleShape shape;
    int timetodie;
    bool alive;
    Bullet(sf::Vector2f pos, float angle,float size,float speed,int timetodie, float dmg):body(pos.x,pos.y,2000,size, 10, 0, 10),timetodie(timetodie),alive(true){
        shape.setRadius(size);
        shape.setFillColor(sf::Color(0,178,225));
        shape.setOutlineThickness(3.f);
        shape.setOutlineColor(sf::Color(14, 144, 178));
        shape.setOrigin({size,size});
        shape.setPosition(pos);
        body.velocity.x=speed * cos(angle*3.14/180);
        body.velocity.y=speed * sin(angle*3.14/180);
        float gunLength = size*6+10;
        body.position.x = pos.x + gunLength * cos(angle * 3.14f / 180.f);
        body.position.y = pos.y + gunLength * sin(angle * 3.14f / 180.f);
        body.setBodyDmg(dmg);
    }
    //void newbullet(float x,float y,float angle,float size,float speed,int timetodie){
//
    //}
    void update(){
        
        if(timetodie==0)
            alive=false;
        if(alive){            
            timetodie-=1;
            body.update();
            shape.setPosition(body.position);
        }
    }
    void draw(sf::RenderWindow &window){
        window.draw(shape);
    }
};

void removeBullet(std::vector<Bullet> &bullets, int index){
    if(!bullets.empty()){
        std::swap(bullets[index], bullets.back());
        bullets.pop_back();
    }
}
class TankBasic{
public:
    Cannon gun;
    TankBasic(float x, float y, float size) : gun(x,y,size, 30){}
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
        gun.DrawGun(window);
        window.draw(bodyShape);
    }
    void update(sf::Vector2f pos, int angle){
        gun.update(pos, angle);
    }
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg){
        if (gun.delay == 0){
            bullets.push_back(Bullet(gun.position,angle,gun.size.y/2,bSpeed,bLife,bDmg));
            gun.delay = gun.reload;
        }
    }
};
class TankTwin{
public:
    Cannon gun1, gun2;
    int gunToggle; // 0:gun1 , 1:gun2
    float gunOffSet; // Kc giữa 2 nòng

    TankTwin(float x, float y, float size): gun1(x, y, size, 30), gun2(x, y, size, 30), gunToggle(0), gunOffSet(size/2) {}
    void update(sf::Vector2f pos, float angle){
        float offSetX = gunOffSet * cos((angle + 90) * 3.14f / 180.f);
        float offsetY = gunOffSet * sin((angle + 90) * 3.14f / 180.f);
        sf::Vector2f posGun1(pos.x - offSetX, pos.y - offsetY);
        sf::Vector2f posGun2(pos.x + offSetX, pos.y + offsetY);
        gun1.update(posGun1, angle);
        gun2.update(posGun2, angle);
    }
    void shoot(std::vector<Bullet> &bullets, int angle, float bSpeed, int bLife, float bDmg){
        if (gunToggle == 0 && gun1.delay == 0){
            bullets.push_back(Bullet(gun1.position,angle,gun1.size.y/2,bSpeed,bLife,bDmg));
            gun1.delay = gun1.reload;
            gunToggle = 1;
        }
        if (gunToggle == 1 && gun2.delay == 0){
             bullets.push_back(Bullet(gun2.position,angle,gun2.size.y/2,bSpeed,bLife,bDmg));
            gun2.delay = gun2.reload;
            gunToggle = 0;
        }
    }
    void drawTank(sf::RenderWindow &window, sf::CircleShape &bodyShape){
        gun1.DrawGun(window);
        gun2.DrawGun(window);
        window.draw(bodyShape);
    }
};
class MyTank{
public:
    Obj body;
    // Cannon gun;
    float velocity_max;
    //float dpos;
    float friction;
    sf::CircleShape bodyShape;
    int xp_base, xp_to_lv_up;
    int score;
    int level;
    int statPoint;
    Stats stats; 

    int tankType; // 0:basic, 1:twin
    TankBasic *tankBasic;
    TankTwin *tankTwin;

    // Base stats
    float base_hp_regen = 0.2f;
    float base_maxhp = 100;
    float base_body_dmg = 2;
    float base_bullet_speed = 8;
    int base_bullet_life = 50;
    float base_bullet_dmg = 10;
    int base_reload = 30;
    float base_move_speed = 6;

    MyTank(float x, float y, float size,float mapsize):body(x,y,mapsize,size,100, 0.2f, 2),score(0),level(1), xp_base(0), xp_to_lv_up(30), tankType(0), tankTwin(nullptr), statPoint(0){
        bodyShape.setRadius(size);
        bodyShape.setFillColor(sf::Color(0,178,225));
        bodyShape.setOutlineThickness(5.f);
        bodyShape.setOutlineColor(sf::Color(14, 144, 178));
        bodyShape.setOrigin({size,size});
        bodyShape.setPosition({x,y});
        body.position={x,y};
        body.velocity={0,0};
        velocity_max=6;
        friction=0.9;
        tankBasic = new TankBasic(x, y, size);
    }
    ~MyTank(){
        if (tankBasic) delete tankBasic;
        if (tankTwin) delete tankTwin;
    }
    void update(int angle){
        // hàm clamp giới hạn tốc độ
        // hàm hypot(x,y) = sqrt(x*x+y*y)
        body.velocity*=friction;
        body.velocity.x=std::clamp(body.velocity.x,-velocity_max,velocity_max);
        body.velocity.y=std::clamp(body.velocity.y,-velocity_max,velocity_max);
        //dpos=std::hypot(body.velocity.x,body.velocity.y);
        //if(dpos>velocity_max){
        //    body.velocity.x=body.velocity.x/dpos*velocity_max;
        //    body.velocity.y=body.velocity.y/dpos*velocity_max;
        //}
        if(body.velocity.x<0.1 && body.velocity.x>-0.1)
            body.velocity.x=0;
        if(body.velocity.y<0.1 && body.velocity.y>-0.1)
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
    void applyStats(){
        body.setHpRegen(base_hp_regen * (1+stats.hp_regen*0.3f));
        body.setMaxHp(base_maxhp * (1+stats.maxhp*0.2f));
        body.setBodyDmg(base_body_dmg * (1+stats.body_dmg*0.25f));
        velocity_max = base_move_speed * (1+stats.move_speed*0.05f);
        float newReload = base_reload - stats.reload*2;
        if (tankType == 0){
            tankBasic->gun.setReload(newReload);
        }
        else if (tankType == 1){
            tankTwin->gun1.setReload(newReload);
            tankTwin->gun2.setReload(newReload);
        }
    }
    void addScore(int point){
        score += point;
    }
    void levelUp(){
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
    float getBulletSpeed(){
        return base_bullet_speed * (1 + stats.bullet_speed * 0.15f);
    }
    int getBulletLife(){
        return base_bullet_life + stats.bullet_penetration * 20;
    }
    float getBulletDamage(){
        return base_bullet_dmg * (1 + stats.bullet_dmg * 0.2f);
    }
    void Drawtank(sf::RenderWindow &window){
        if (tankType == 0){
            tankBasic->drawTank(window, bodyShape);
        }
        else if (tankType == 1){
            tankTwin->drawTank(window, bodyShape);
        }
    }
    void moveX(float dx){
        body.velocity.x+=dx;
    }
    void moveY(float dy){
        body.velocity.y+=dy;
    }
    bool upgradeStat(int statIndex){
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
    void shoot(std::vector<Bullet> &bullets, int angle){
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
    int getTankType(){
        return tankType;
    }
};
class Minimap{
public:
    sf::RectangleShape map;
    sf::CircleShape player;
    sf::CircleShape enemy;
    sf::Vector2f Mapsize;
    Minimap(float x, float y, float size,sf::Vector2f mapsize):Mapsize(mapsize){
        map.setSize({size,size});
        map.setFillColor(sf::Color(198,198,198));
        map.setOutlineThickness(4.f);
        map.setOutlineColor(sf::Color(114,114,114));
        map.setOrigin({size/2,size/2});
        map.setPosition({x-size/2-25,y-size/2-25});
        player.setRadius(3);
        player.setFillColor(sf::Color(60,60,60));
        player.setOrigin({3,3});
        enemy.setRadius(3);
        enemy.setFillColor(sf::Color(239,77,85));
        enemy.setOrigin({3,3});
    }
    void Drawmap(sf::RenderWindow &window,sf::Vector2f playerpos={120,120},sf::Vector2f enemypos={120,120}){
        player.setPosition({playerpos.x/Mapsize.x*map.getSize().x/2+map.getPosition().x, playerpos.y/Mapsize.y*map.getSize().y/2+map.getPosition().y});
        enemy.setPosition({enemypos.x/Mapsize.x*map.getSize().x/2+map.getPosition().x,enemypos.y/Mapsize.y*map.getSize().y/2+map.getPosition().y});
        window.draw(map);
        window.draw(player);
    }
};
class Line{
private:
    sf::VertexArray line;
public:
    Line(int w, int h):line(sf::PrimitiveType::Lines){
        for(int i=-w;i<=w;i+=50){
            line.append(sf::Vertex{sf::Vector2f(i,-h), sf::Color(195,195,195)});
            line.append(sf::Vertex{sf::Vector2f(i,h), sf::Color(195,195,195)});
        }
        for(int i=-h;i<=h;i+=50){
            line.append(sf::Vertex{sf::Vector2f(-w,i), sf::Color(195,195,195)});
            line.append(sf::Vertex{sf::Vector2f(w,i), sf::Color(195,195,195)});
        }
    }
    void draw(sf::RenderWindow &window){
        window.draw(line);
    }
};
class Obstacle {
public:
    Obj body;
    sf::CircleShape shape;
    int type;
    int xp_reward;
    bool alive{ true };
    int timeToRespawn=0;
    float friction{ 0.9f };
    struct Conf {
        float drawRadius;
        float hp;
        float bodyDmg;
        int   xp;
        sf::Color fill, outline;
    };

    static Conf configFor(int t) {
        if (t == 4) { return Conf{ 20.f, 30.f, 5.f, 10,  sf::Color(255,231,105), sf::Color(195,178, 80) }; }
        else if (t == 3) { return Conf{ 25.f, 80.f, 8.f, 25, sf::Color(252,118,119), sf::Color(190, 90, 90) }; }
        else { return Conf{ 40.f,160.f,12.f,100, sf::Color(118,141,252), sf::Color(96,112,189) }; }
    }

    Obstacle(float x, float y, float, float mapsize, int t)
        : type(t),
        body(x, y, mapsize, configFor(t).drawRadius* HITBOX_SCALE, configFor(t).hp, 0.f, configFor(t).bodyDmg),
        xp_reward(configFor(t).xp) {
        Conf c = configFor(t);
        shape.setPointCount(static_cast<std::size_t>(t));
        shape.setOutlineThickness(5.f);
        shape.setRadius(c.drawRadius);
        shape.setOrigin({ c.drawRadius, c.drawRadius });
        shape.setPosition({ x, y });
        shape.setFillColor(c.fill);
        shape.setOutlineColor(c.outline);
        float randomDeg = static_cast<float>(rand() % 360);
        shape.setRotation(sf::degrees(randomDeg));
    }

    void update() {
        if (!alive) return;
        body.update();
        body.velocity *= friction;
        if (std::abs(body.velocity.x) < 0.1f) body.velocity.x = 0.f;
        if (std::abs(body.velocity.y) < 0.1f) body.velocity.y = 0.f;
        shape.setPosition(body.position);
    }

    void DrawObs(sf::RenderWindow& window) { if (alive) window.draw(shape); }

    void respawn(float map_w, float map_h, const std::vector<Obstacle>& others, const Obj& tankBody) {
        if(timeToRespawn<frame)
            return;
        bool ok = false;
        sf::Vector2f p;
        while (!ok) {
            p.x = (float)((rand() % (int)(map_w * 2)) - (int)map_w);
            p.y = (float)((rand() % (int)(map_h * 2)) - (int)map_h);
            ok = true;
            float dxT = p.x - tankBody.position.x;
            float dyT = p.y - tankBody.position.y;
            if (std::hypot(dxT, dyT) < (body.hitbox_r + tankBody.hitbox_r)) {
                ok = false; 
                continue; 
            }
            for (const auto& o : others) {
                if (&o == this) continue;
                if (!o.alive) continue;
                float dx = p.x - o.body.position.x;
                float dy = p.y - o.body.position.y;
                if (std::hypot(dx, dy) < (body.hitbox_r + o.body.hitbox_r)) { 
                    ok = false; 
                    continue;
                }
            }
        }
        body.position = p;
        body.hp = body.maxhp;
        body.velocity = { 0.f, 0.f };
        shape.setPosition(p);
        shape.setRotation(sf::degrees((float)(rand() % 360)));
        alive = true;
    }
};
void handleBulletObstacleCollision(std::vector<Bullet>& bullets, std::vector<Obstacle>& obstacles, MyTank& mytank) {
    for (int i=0;i<bullets.size();i++) {
        if (!bullets[i].alive) 
            continue;
        for (int j=0;j<obstacles.size();j++) {
            if (!obstacles[j].alive) 
                continue;
            if (bullets[i].body.isCollidingWith(obstacles[j].body)) {
                bullets[i].body.handleCollisionWith(obstacles[j].body,true,true,0.5);
                if (bullets[i].body.hp <= 0.f){
                    bullets[i].alive = false;
                }
                if (obstacles[j].body.hp <= 0.f) {
                    obstacles[j].alive = false;
                    obstacles[j].timeToRespawn=frame+fps*30;
                    mytank.addScore(obstacles[j].xp_reward);
                    mytank.levelUp();
                }
                break;
            }
        }
    }
    for (int i = (int)bullets.size() - 1; i >= 0; --i) 
        if (!bullets[i].alive) 
            removeBullet(bullets, i);
}
void handleBulletTankCollision(std::vector<Bullet>& enemy_bullets, MyTank& tank) {
    const float EPS = 1e-3f;
    for (int i=0;i<enemy_bullets.size();i++) {
        if (!enemy_bullets[i].alive) 
            continue;
        if (enemy_bullets[i].body.isCollidingWith(tank.body) == false) 
            continue;
        enemy_bullets[i].body.handleCollisionWith(tank.body,true,true,0.5);
        if (enemy_bullets[i].body.hp <= 0.f){
            enemy_bullets[i].alive = false;
            removeBullet(enemy_bullets,i);
        }
    }
}
void handleBullet_BulletCollision(std::vector<Bullet>& my_bullets,std::vector<Bullet>& enemy_bullets){
    for(int i=0;i<my_bullets.size();i++){
        if(my_bullets[i].alive==false)
            continue;
        for(int j=0;j<enemy_bullets.size();j++){
            if(enemy_bullets[j].alive == false)
                continue;
            my_bullets[i].body.handleCollisionWith(enemy_bullets[j].body,true,false);
            if(my_bullets[i].body.hp == 0){
                my_bullets[i].alive=false;
            }
            if(enemy_bullets[j].body.hp==0){
                enemy_bullets[j].alive=false;
            }
        }
    }
    for(int i=0;i<my_bullets.size();i++)
        if(my_bullets[i].alive == false)
            removeBullet(my_bullets,i);
    for(int i=0;i<enemy_bullets.size();i++)
        if(enemy_bullets[i].alive == false)
            removeBullet(enemy_bullets,i);

}
void handleTankObstacleCollision(MyTank &tank,std::vector<Obstacle> & obs){
    for(int i=0;i<obs.size();i++){
        if(obs[i].alive==false)
            continue;
        if(obs[i].body.isCollidingWith(tank.body)){
            tank.body.handleCollisionWith(obs[i].body,false,true,0.5);
            if(obs[i].body.hp<=0){
                obs[i].alive = false;
                tank.addScore(obs[i].xp_reward);
                tank.levelUp();
                obs[i].timeToRespawn=frame+fps*30;
            }
            return;
        }
    }
}
class XpBar{
private:
    sf::RectangleShape xpBarBG;
    sf::RectangleShape xpBar;
    sf::Text xpText, scoreText;
    std::stringstream xpss, scoress;
    sf::Vector2f pos;
    sf::Vector2f size;
public:
    XpBar(float x, float y, sf::Vector2f size, MyTank &mytank,sf::Font &font) : pos(x,y), xpText(font), scoreText(font), size(size){
        xpBarBG.setSize(size);
        xpBarBG.setOrigin({size.x/2, size.y/2});
        xpBarBG.setPosition({x/2-25, y-25});
        xpBarBG.setFillColor(sf::Color(150,150,150));
        xpBarBG.setOutlineThickness(4.f);
        xpBarBG.setOutlineColor(sf::Color(114,114,114));
        xpBar.setOrigin({size.x/2, size.y/2});
        xpBar.setPosition({x/2 -25, y -25});
        xpBar.setFillColor(sf::Color(0, 240, 0));
        xpText.setCharacterSize(14);
        xpText.setFillColor(sf::Color::White);
        xpText.setOutlineThickness(2.f);
        xpText.setOutlineColor(sf::Color::Black);
        scoreText.setCharacterSize(12);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setOutlineThickness(2.f);
        scoreText.setOutlineColor(sf::Color::Black);
    }
    void update(MyTank &mytank){
        float ratio = (float)(mytank.score - mytank.xp_base) / (mytank.xp_to_lv_up - mytank.xp_base);
        ratio = std::clamp(ratio, 0.f, 1.f);
        xpBar.setSize({size.x * ratio, size.y});
        xpss.str("");
        xpss.clear();
        xpss << "Level " << mytank.level;
        sf::FloatRect xpTextBounds = xpText.getLocalBounds();
        xpText.setOrigin({xpTextBounds.size.x / 2, xpTextBounds.size.y / 2});
        xpText.setPosition({pos.x / 2-30, pos.y-25});
        xpText.setString(xpss.str());
        sf::FloatRect scoreTextBounds =  scoreText.getLocalBounds();
        scoreText.setOrigin({scoreTextBounds.size.x / 2, scoreTextBounds.size.y / 2});
        scoreText.setPosition({pos.x / 2 - 30, pos.y - 50});
        scoress.str("");
        scoress.clear();
        scoress << "Score: " << mytank.score;
        scoreText.setString(scoress.str());
    }
    void draw(sf::RenderWindow &window){
        window.draw(xpBarBG);
        window.draw(xpBar);
        window.draw(xpText);
        window.draw(scoreText);
    }
};
class Bar{
public:
    sf::RectangleShape barBG;
    sf::RectangleShape fill;
    sf::RectangleShape button;
    sf::Text name, idxText;
    int  index;
    sf::Color color;
   
    Bar(sf::Vector2f size, sf::Font &f, sf::Color color):name(f), color(color), idxText(f){
        barBG.setSize(size);
        barBG.setFillColor(sf::Color(150,150,150));
        barBG.setOutlineThickness(2);
        barBG.setOutlineColor(sf::Color(114,114,114));
        fill.setSize({0, size.y});
        fill.setFillColor(color);
        button.setSize({size.y, size.y});
        button.setFillColor(sf::Color(100, 200, 100));
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(60, 150, 60));
        name.setCharacterSize(8);
        name.setFillColor(sf::Color::White);
        name.setOutlineThickness(1);
        name.setOutlineColor(sf::Color::Black);
        idxText.setCharacterSize(10);
        idxText.setFillColor(sf::Color::White);
        idxText.setOutlineThickness(2);
        idxText.setOutlineColor(sf::Color::Black);
    }
    void updatePosition( sf::Vector2f position){
        barBG.setPosition(position);
        fill.setPosition(position);
        button.setPosition({position.x + barBG.getSize().x + 5, position.y});
        name.setPosition({position.x +30, position.y + barBG.getSize().y/2-4});
        idxText.setPosition({button.getPosition().x + button.getSize().x + 10, position.y + barBG.getSize().y/2 - 6});
    }
};
class StatsBar{
private:
    std::vector<Bar> bars;
    sf::Text pointsText;
    sf::Vector2f position;
    bool visible;
public:
    float barWidth = 180;
    float barHeight = 18;
    float barSpacing = 8;
    StatsBar(sf::Font &f, int w, int h):pointsText(f), visible(false){
        position.x = 15;
        position.y = h - 8*(barHeight+barSpacing) - 50;
        std::vector<std::string> statNames = {"Health Regen", "Max Health", "Body Damage", "Bullet Speed","Bullet Penetration", "Bullet Damage", "Reload", "Movement Speed"};
        std::vector<sf::Color> colors = {
            sf::Color(255, 150, 120),  // Health Regen
            sf::Color(200, 120, 255),  // Max Health
            sf::Color(180, 100, 100),  // Body Damage
            sf::Color(120, 180, 255),  // Bullet Speed 
            sf::Color(255, 220, 120),  // Bullet Penetration 
            sf::Color(255, 120, 120),  // Bullet Damage 
            sf::Color(120, 255, 180),  // Reload 
            sf::Color(150, 255, 150)   // Movement Speed
        };
        for (int i = 0; i < 8; i++){
            Bar bar({barWidth, barHeight}, f, colors[i]);
            bar.name.setString(statNames[i]);
            bar.index = i;
            bars.push_back(bar);
        }
        pointsText.setCharacterSize(12);
        pointsText.setFillColor(sf::Color::Yellow);
        pointsText.setOutlineThickness(2);
        pointsText.setOutlineColor(sf::Color::Black);
    }
    void update(MyTank &mytank){
        visible = (mytank.statPoint > 0);
        std::stringstream ss;
        ss << "Stat Points: " << mytank.statPoint;
        pointsText.setString(ss.str());
        pointsText.setPosition({position.x + 30, position.y-25});
        int statValues[10] = {
            mytank.stats.hp_regen, mytank.stats.maxhp, mytank.stats.body_dmg,
            mytank.stats.bullet_speed, mytank.stats.bullet_penetration, mytank.stats.bullet_dmg,
            mytank.stats.reload, mytank.stats.move_speed
        };
        for (int i = 0; i < 8; i++){
            float ypos = position.y + i*(barHeight+barSpacing);
            bars[i].updatePosition({position.x, ypos});
            float ratio = (float)statValues[i] / mytank.stats.maxLevel;
            bars[i].fill.setSize({bars[i].barBG.getSize().x * ratio, bars[i].barBG.getSize().y});
            std::stringstream idx_ss;
            idx_ss << "[" << bars[i].index + 1 << "]";
            bars[i].idxText.setString(idx_ss.str());
        }
    }
    int checkButtonClick(sf::Vector2i mousePos){
        for (int i = 0; i < 8; i++){
            if (bars[i].button.getGlobalBounds().contains(sf::Vector2f(mousePos))){
                return i;
            }
        }
        return -1;
    }
    void draw(sf::RenderWindow &window, sf::Font &font){
        if (!visible) return;
        window.draw(pointsText);
        for(int i = 0; i < 8; i++){
            window.draw(bars[i].barBG);
            window.draw(bars[i].fill);
            window.draw(bars[i].button);
            window.draw(bars[i].name);
            window.draw(bars[i].idxText);

            sf::Text plus(font);
            plus.setString("+");
            plus.setCharacterSize(15);
            plus.setFillColor(sf::Color::White);
            plus.setOutlineThickness(1);
            plus.setOutlineColor(sf::Color::Black);
            sf::FloatRect bounds = bars[i].button.getGlobalBounds();
            sf::FloatRect plusBounds = plus.getLocalBounds();
            plus.setPosition({bounds.position.x + bounds.size.x/2 - plusBounds.size.x/2,bounds.position.y + bounds.size.y/2 - plusBounds.size.y+2});
            window.draw(plus);
        }
    }
    bool isVisible(){return  visible;}
};

int main(){
    srand(time(0));
    int delay_click=0;
    int WIDTH = 900; // dài 
    int HEIGHT = 600; // rộng
    float acceleration = 0.7; // gia tốc
    float map_width=2000, map_height=2000;
    float bodysize=36;

    sf::ContextSettings settings;
    settings.antiAliasingLevel=4;
    bool fullscreen=false;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Diep",sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(fps);
    sf::View mainview;
    mainview.setSize({(float)WIDTH, (float)HEIGHT});
    mainview.setSize({1350,900});

     // tải font chữ
    sf::Font font;
    if(!font.openFromFile("arial.ttf"))
        return -1;
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Red);
    sf::Font SupercellMagic;
    if(!SupercellMagic.openFromFile("Supercell-Magic Regular.ttf"))
        return -1;


    MyTank mytank(0,0,bodysize,map_width);
    Minimap minimap(WIDTH,HEIGHT,120,{map_width,map_height});
    XpBar xpbar(WIDTH, HEIGHT, {400, 10}, mytank, SupercellMagic);
    StatsBar statsbar(SupercellMagic, WIDTH, HEIGHT);
    Line line(map_width,map_height);

    std::vector<Obstacle> obs;
    std::vector<Bullet> bullets;
    std::vector<Bullet> enemy_bullets;

    for(int i=0;i<100;i++){
        int type=rand()%3+3;
        float x, y;
        // Không sinh obstacle quanh điểm spawn
        do{
            x = rand()%(int)(map_width*2)-map_width;
            y = rand()%(int)(map_height*2)-map_height;
        }while (std::hypot(x, y) <= 200.f);
        obs.push_back(Obstacle(x,y,0.f,map_width,type));
    }
   
    int angle=0;

    while(window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
                if (keyPressed->scancode == sf::Keyboard::Scancode::R){
                    mytank.addScore(100);
                    mytank.levelUp();
                }
                if (keyPressed->scancode == sf::Keyboard::Scancode::T){
                    mytank.score = 0;
                    mytank.stats={0,0,0,0,0,0,0,7};
                    mytank.levelUp();
                }
            }
        }
        // di chuyển 
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)){
            mytank.moveY(-acceleration);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)){
            mytank.moveY(acceleration);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
            mytank.moveX(-acceleration);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
            mytank.moveX(acceleration);
        }

        // tính góc nòng súng
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int dx=mousePos.x-window.getSize().x/2, dy=mousePos.y-window.getSize().y/2;
        angle=atan2(dy,dx)*180/3.14;

        // Shooting
        if((sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))){
            mytank.shoot(bullets, angle);
        }

        // Chọn nâng cấp
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && statsbar.isVisible() && delay_click==0){
            int statIndex = statsbar.checkButtonClick(mousePos);
            if(statIndex != -1){
                mytank.upgradeStat(statIndex);
            }
            delay_click=10;
        }
        delay_click=std::max(0,delay_click-1);

        std::stringstream ss;

        mytank.update(angle);
        xpbar.update(mytank);
        statsbar.update(mytank);

        // ss << "Mouse pos: " <<mousePos.x <<" "<<mousePos.y<<"\n"<<"angle: "<<angle << '\n'<< "Tank pos: " << mytank.body.position.x << ' ' << mytank.body.position.y << '\n' << "tank speed: " << mytank.body.velocity.x << ' ' << mytank.body.velocity.y<<'\n'<<"window size: "<<window.getSize().x <<' ' << window.getSize().y;
        ss << "Level: " << mytank.level << '\n'<< "Score: " << mytank.score << '\n' << "Xp_base: " << mytank.xp_base << "/" << mytank.xp_to_lv_up << '\n'<< "HP: " << (int)mytank.body.hp << "/" << (int)mytank.body.maxhp;
        ss << "HP_regen: " << mytank.body.hp_regen << '\n' << "Body_dmg: " << mytank.body.body_dmg << '\n' << "Move Speed: " << mytank.velocity_max << '\n';
        // ss << "Reload: " << mytank.tankBasic->gun.reload;
        ss << '\n' << "bSpeed: " << mytank.getBulletSpeed() << '\n' << "bLife: " << mytank.getBulletLife() << '\n' << "bDmg: " << mytank.getBulletDamage() << '\n';
        text.setString(ss.str());      
        
        
        
        for (int i = bullets.size() - 1; i >= 0; --i) {
            bullets[i].update();
            if (!bullets[i].alive)
                removeBullet(bullets, i);
        }
        for (auto& o : obs) {
            o.update();
        }
        handleTankObstacleCollision(mytank,obs);
        handleBulletObstacleCollision(bullets, obs, mytank);
        handleBulletTankCollision(enemy_bullets, mytank);
        handleBullet_BulletCollision(enemy_bullets,bullets);
        //for (auto& o : obs) 
        //    if (!o.alive) 
        //        o.respawn(map_width, map_height, obs, mytank.body);

        window.clear(sf::Color(204, 204, 204));

        mainview.setCenter(mytank.body.position);
        window.setView(mainview);
        line.draw(window);
        for (auto& b : bullets) b.draw(window);
        for (auto& o : obs) o.DrawObs(window);
        mytank.Drawtank(window); 
        
        window.setView(window.getDefaultView());
        statsbar.draw(window,SupercellMagic);
        xpbar.draw(window);
        minimap.Drawmap(window,mytank.body.position);
        window.draw(text);

        window.display();

        frame++;
    }

    return 0;
}