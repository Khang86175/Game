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

    Obj(float x, float y,float mapsize,float size):position({x,y}),velocity({0,0}),mapsize(mapsize),hitbox_r(size){
        hp=maxhp=100;
        hp_regen=1;
        body_dmg=2;
    }
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
    int delay;
    float angle;
    sf::Vector2f position;
    sf::RectangleShape gun;

    Cannon(float x, float y,float size):position({x,y}),angle(0),delay(0){
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
class MyTank{
public:
    Obj body;
    Cannon gun;
    float velocity_max;
    //float dpos;
    float friction;
    sf::CircleShape bodyShape;
    int score;
    int level;

    MyTank(float x, float y, float size,float mapsize):body(x,y,mapsize,size),gun(x,y,size),score(0),level(1){
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

        gun.update(body.position,angle);
        bodyShape.setPosition(body.position);
    }
    void Drawtank(sf::RenderWindow &window){
        gun.DrawGun(window);
        window.draw(bodyShape);
    }
    void moveX(float dx){
        body.velocity.x+=dx;
    }
    void moveY(float dy){
        body.velocity.y+=dy;
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
    Obstacle(float x, float y,float size,float mapsize,int type):body(x,y,mapsize,size){
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
class Bullet{
public:
    Obj body;
    sf::CircleShape shape;
    int timetodie;
    bool alive;
    Bullet(sf::Vector2f pos, float angle,float size,float speed,int timetodie):body(pos.x,pos.y,2000,size),timetodie(timetodie),alive(true){
        shape.setRadius(size);
        shape.setFillColor(sf::Color(0,178,225));
        shape.setOutlineThickness(3.f);
        shape.setOutlineColor(sf::Color(14, 144, 178));
        shape.setOrigin({size,size});
        shape.setPosition(pos);
        body.velocity.x=speed * cos(angle*3.14/180);
        body.velocity.y=speed * sin(angle*3.14/180);
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

    MyTank mytank(x,y,bodysize,map_width);
    Minimap minimap(WIDTH,HEIGHT,120,{map_width,map_height});

    Line line(map_width,map_height);
    std::vector<Obstacle> obs;
    std::vector<Bullet> testing;
    for(int i=0;i<100;i++){
        int type=rand()%3+3;
        obs.push_back(Obstacle(rand()%(int)(map_width*2)-map_width,rand()%(int)(map_height*2)-map_height,type*10,map_width,type));
    }
    // tải font chữ
    sf::Font font;
    if(!font.openFromFile("arial.ttf"))
        return -1;
    sf::Text text(font);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::Red);

    int angle=1;
    while(window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
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
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && mytank.gun.delay==0){
            testing.push_back(Bullet(mytank.body.position,angle,10,8,150));
            mytank.gun.delay=30;
        }
        std::stringstream ss;

        mytank.update(angle);
        ss << "Mouse pos: " <<mousePos.x <<" "<<mousePos.y<<"\n"<<"angle: "<<angle << '\n'<< "Tank pos: " << mytank.body.position.x << ' ' << mytank.body.position.y << '\n' << "tank speed: " << mytank.body.velocity.x << ' ' << mytank.body.velocity.y<<'\n'<<"window size: "<<window.getSize().x <<' ' << window.getSize().y;
        text.setString(ss.str());

        window.clear(sf::Color(204, 204, 204));
        mainview.setCenter(mytank.body.position);
        window.setView(mainview);
        
        line.draw(window);
        
        for(int i=0;i<testing.size();i++){
            testing[i].update();
            if(testing[i].alive==true)
                testing[i].draw(window);
        }
        
        for(int i=0;i<100;i++)
            obs[i].DrawObs(window);
        mytank.Drawtank(window);

        window.setView(window.getDefaultView());
        minimap.Drawmap(window,mytank.body.position);
        window.draw(text);
        window.display();
    }

    return 0;
}