#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <ctime>

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
        if(hp<maxhp)
            hp+=hp_regen;
        if(hp>maxhp)
            hp=maxhp;
    }
    float Getx(){
        return position.x;
    }
    float Gety(){
        return position.y;
    }
};
float CheckCollision(const Obj &a,const Obj &b){
    float dx=a.position.x-b.position.x;
    float dy=a.position.y-b.position.y;
    float distance=std::hypot(dx,dy);
    if(distance<a.hitbox_r+b.hitbox_r){
        return distance;
    }
    return 0;
}
class Cannon{
public:
    int delay, recoil;
    float angle;
    sf::Vector2f position;
    sf::RectangleShape gun;

    Cannon(float x, float y,float size, float recoil):position({x,y}),angle(0),delay(0), recoil(recoil){
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
};
class Bullet{
public:
    Obj body;
    sf::CircleShape shape;
    int timetodie;
    bool alive;
    Bullet(sf::Vector2f pos, float angle,float size,float speed,int timetodie):body(pos.x,pos.y,2000,size, 10, 0, 10),timetodie(timetodie),alive(true){
        shape.setRadius(size);
        shape.setFillColor(sf::Color(0,178,225));
        shape.setOutlineThickness(3.f);
        shape.setOutlineColor(sf::Color(14, 144, 178));
        shape.setOrigin({size,size});
        shape.setPosition(pos);
        body.velocity.x=speed * cos(angle*3.14/180);
        body.velocity.y=speed * sin(angle*3.14/180);
        float gunLength = 36*2;
        body.position.x = pos.x + gunLength * cos(angle * 3.14f / 180.f);
        body.position.y = pos.y + gunLength * sin(angle * 3.14f / 180.f);
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
    void shoot(std::vector<Bullet> &bullets, int angle){
        if (gun.delay == 0){
            bullets.push_back(Bullet(gun.position,angle,10,8,150));
            gun.delay = gun.recoil;
        }
    }
};
class TankTwin{
public:
    Cannon gun1, gun2;
    int gunToggle; // 0:gun1 , 1:gun2
    int gunOffSet; // Kc giữa 2 nòng

    TankTwin(float x, float y, float size): gun1(x, y, size, 30), gun2(x, y, size, 30), gunToggle(0), gunOffSet(20) {}
    void update(sf::Vector2f pos, float angle){
        float offSetX = gunOffSet * cos((angle + 90) * 3.14f / 180.f);
        float offsetY = gunOffSet * sin((angle + 90) * 3.14f / 180.f);
        sf::Vector2f posGun1(pos.x - offSetX, pos.y - offsetY);
        sf::Vector2f posGun2(pos.x + offSetX, pos.y + offsetY);
        gun1.update(posGun1, angle);
        gun2.update(posGun2, angle);
    }
    void shoot(std::vector<Bullet> &bullets, int angle){
        if (gunToggle == 0 && gun1.delay == 0){
            bullets.push_back(Bullet(gun1.position,angle,10,8,150));
            gun1.delay = gun1.recoil/2;
            gunToggle = 1;
        }
        if (gunToggle == 1 && gun2.delay == 0){
            bullets.push_back(Bullet(gun2.position,angle,10,8,150));
            gun2.delay = gun2.recoil/2;
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

    int tankType; // 0:basic, 1:twin
    TankBasic *tankBasic;
    TankTwin *tankTwin;
    float size;

    MyTank(float x, float y, float size,float mapsize):body(x,y,mapsize,size,100, 0.2f, 2),score(0),level(1), xp_base(0), xp_to_lv_up(100), tankType(0), size(size), tankTwin(nullptr){
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
    void addScore(int point){
        score += point;
    }
    void levelUp(){
        if (score >= xp_to_lv_up){
            level += 1;
            xp_base = xp_to_lv_up;
            xp_to_lv_up += level*100;

            body.maxhp += 20;
            body.hp = body.maxhp;
            body.hp_regen += 0.05;
            body.body_dmg += 1;
            velocity_max += 0.2;

            bodyShape.setRadius(bodyShape.getRadius() + 1);
            bodyShape.setOrigin({bodyShape.getRadius(), bodyShape.getRadius()});
            body.hitbox_r = bodyShape.getRadius();
        }
        if (level >= 5 && tankType == 0){
            delete tankBasic;
            tankBasic = nullptr;
            tankTwin = new TankTwin(body.position.x, body.position.y, size);
            tankType = 1;
        }
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
    void shoot(std::vector<Bullet> &bullets, int angle){
        if (tankType == 0){
            tankBasic->shoot(bullets, angle);
        }
        else if (tankType == 1){
            tankTwin->shoot(bullets, angle);
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
        player.setOrigin({5,5});
    }
    void Drawmap(sf::RenderWindow &window,sf::Vector2f playerpos={120,120}){
        player.setPosition({playerpos.x/Mapsize.x*map.getSize().x/2+map.getPosition().x, playerpos.y/Mapsize.y*map.getSize().y/2+map.getPosition().y});
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
class Obstacle{
public:
    Obj body;
    sf::CircleShape shape;
    Obstacle(float x, float y,float size,float mapsize,int type):body(x,y,mapsize,size, 50, 0, 5){
        shape.setPointCount(type);
        shape.setOutlineThickness(5.f);
        shape.setRadius((type==3? 25: type==4? 20: 40));
        if(type==3){
            shape.setFillColor(sf::Color(252, 118, 119));
            shape.setOutlineColor(sf::Color(190, 90, 90));
        }
        else if(type==4){
            shape.setFillColor(sf::Color(255, 231, 105));
            shape.setOutlineColor(sf::Color(195, 178, 80));
        }
        else{
            shape.setFillColor(sf::Color(118, 141, 252));
            shape.setOutlineColor(sf::Color(96, 112, 189));
        }
        shape.setOrigin({size,size});
        shape.setPosition({x,y});
    }

    void DrawObs(sf::RenderWindow &window){
        //shape.setPosition(body.position);
        window.draw(shape);
    }
};
class XpBar{
private:
    sf::RectangleShape xpBarBG;
    sf::RectangleShape xpBar;
    sf::Text xpText, scoreText;
    std::stringstream xpss, scoress;
    sf::Vector2f pos;
    sf::Vector2f size;
public:
    XpBar(float x, float y, sf::Vector2f size, MyTank &mytank,sf::Font font) : pos(x,y), xpText(font), scoreText(font), size(size){
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
int main(){
    srand(time(0));
    int WIDTH = 900; // dài 
    int HEIGHT = 600; // rộng
    float acceleration = 0.7; // gia tốc
    float map_width=2000, map_height=2000;
    int x=0,y=0;
    float bodysize=36;

    sf::ContextSettings settings;
    settings.antiAliasingLevel=4;
    bool fullscreen=false;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Diep",sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(30);
    sf::View mainview;
    mainview.setSize({(float)WIDTH, (float)HEIGHT});

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


    MyTank mytank(x,y,bodysize,map_width);
    Minimap minimap(WIDTH,HEIGHT,120,{map_width,map_height});
    XpBar xpbar(WIDTH, HEIGHT, {400, 10}, mytank, SupercellMagic);
    Line line(map_width,map_height);
    std::vector<Obstacle> obs;
    std::vector<Bullet> bullets;
    for(int i=0;i<100;i++){
        int type=rand()%3+3;
        float x, y;
        // Không sinh obstacle quanh điểm spawn
        do{
            x = rand()%(int)(map_width*2)-map_width;
            y = rand()%(int)(map_height*2)-map_height;
        }while (std::hypot(x, y) <= 200);
        obs.push_back(Obstacle(x,y,type*10,map_width,type));
    }
   
    int angle=1;
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

        std::stringstream ss;

        mytank.update(angle);
        mytank.levelUp();
        xpbar.update(mytank);
        ss << "Mouse pos: " <<mousePos.x <<" "<<mousePos.y<<"\n"<<"angle: "<<angle << '\n'<< "Tank pos: " << mytank.body.position.x << ' ' << mytank.body.position.y << '\n' << "tank speed: " << mytank.body.velocity.x << ' ' << mytank.body.velocity.y<<'\n'<<"window size: "<<window.getSize().x <<' ' << window.getSize().y;
        ss << "Level: " << mytank.level << '\n'<< "Score: " << mytank.score << '\n' << "Xp_base: " << mytank.xp_base << "/" << mytank.xp_to_lv_up << '\n'<< "HP: " << (int)mytank.body.hp << "/" << (int)mytank.body.maxhp;
        text.setString(ss.str());      

        window.clear(sf::Color(204, 204, 204));
        mainview.setCenter(mytank.body.position);
        window.setView(mainview);
        
        line.draw(window);
        
        for (int i = bullets.size() - 1; i >= 0; --i) {
            bullets[i].update();
            if (bullets[i].alive)
                bullets[i].draw(window);
            else
                removeBullet(bullets, i);
        }       
        for(int i=0;i<100;i++)
            obs[i].DrawObs(window);
        mytank.Drawtank(window); 

        window.setView(window.getDefaultView());
        xpbar.draw(window);
        minimap.Drawmap(window,mytank.body.position);
        window.draw(text);
        window.display();
    }

    return 0;
}