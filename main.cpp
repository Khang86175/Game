#include <SFML/Graphics.hpp>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>

class Map{
    private:
        sf::RectangleShape background;
        sf::VertexArray grid;
        float width, height;
    public:
        Map(float w, float h)
         : width(w), height(h), grid(sf::PrimitiveType::Lines){
            background.setSize({w, h});
            background.setFillColor(sf::Color(230, 230, 230));
            background.setOutlineColor(sf::Color(204, 204, 204));
            background.setOutlineThickness(5.f);
            background.setPosition({0, 0});
            background.setOrigin({w/2, h/2});

            float cellW = width / 200; // chiều rộng của 1 ô
            float cellH = height / 200;  // chiều dài của 1 ô
            for (float i = - width; i <= width; i += cellW){
                sf::Vertex vertex1{{i, - height}, sf::Color(204, 204, 204)};
                sf::Vertex vertex2{{i , height}, sf::Color(204, 204, 204)};
                grid.append(vertex1);
                grid.append(vertex2);
            }
            for (float i = - height; i <= height; i += cellH){
                sf::Vertex vertex1{{- width, i}, sf::Color(204, 204, 204)};
                sf::Vertex vertex2{{width, i}, sf::Color(204, 204, 204)};
                grid.append(vertex1);
                grid.append(vertex2);
            }
        }
        void DrawBackground(sf::RenderWindow &window){
            window.draw(background);
            window.draw(grid);
        }
        float getWidth(){
            return width;
        }
        float getHeight(){
            return height;
        }
};
class Obj{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float hp,maxhp,hp_regen,body_dmg;

    Obj(float x, float y):position({x,y}),velocity({0,0}){
        hp=maxhp=100;
        hp_regen=1;
        body_dmg=2;
    }
    void update(){
        position+=velocity;
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
    float dpos;
    float friction;
    sf::CircleShape bodyShape;

    MyTank(float x, float y, float size):body(x,y),gun(x,y,size){
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
    void update(int angle, Map &map){
        // hàm clamp giới hạn tốc độ
        // hàm hypot(x,y) = sqrt(x*x+y*y)
        body.velocity*=friction;
        body.velocity.x=std::clamp(body.velocity.x,-velocity_max,velocity_max);
        body.velocity.y=std::clamp(body.velocity.y,-velocity_max,velocity_max);
        dpos=std::hypot(body.velocity.x,body.velocity.y);
        if(dpos>velocity_max){
            body.velocity.x=body.velocity.x/dpos*velocity_max;
            body.velocity.y=body.velocity.y/dpos*velocity_max;
        }
        if(body.velocity.x<0.1 && body.velocity.x>-0.1)
            body.velocity.x=0;
        if(body.velocity.y<0.1 && body.velocity.y>-0.1)
            body.velocity.y=0;
        body.update();
        // giới hạn di chuyển trong map
        float r = bodyShape.getRadius();
        body.position.x = std::clamp(body.position.x, -map.getWidth()/2 + r, map.getWidth()/2 - r);
        body.position.y = std::clamp(body.position.y, -map.getHeight()/2 + r, map.getHeight()/2 - r);

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
class MiniMap{
    private:
        sf::View minimap;
        sf::RectangleShape minimapBG; // background
        sf::RectangleShape border;
    public:
        MiniMap(float width, float height, sf::RenderWindow &window){
            minimap.setSize({1500, 1500});
            minimap.setViewport(sf::FloatRect(sf::Vector2f(0.75, 0.75), sf::Vector2f(0.25, 0.25)));

            minimapBG.setSize({width*2, height*2});
            minimapBG.setFillColor(sf::Color(50,50,180));
            minimapBG.setPosition({0, 0});
            minimapBG.setOrigin({width, height});

            border.setSize({window.getSize().x * 0.25f, window.getSize().y * 0.25f}); // đúng tỉ lệ viewport
            border.setPosition({window.getSize().x * 0.75f, window.getSize().y * 0.75f}); // góc dưới phải
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineColor(sf::Color(55, 55, 55, 195));
            border.setOutlineThickness(7.f);
        }
        void drawMinimap(sf::RenderWindow &window, MyTank &mytank, Map &map){
            minimap.setCenter(mytank.body.position);
            window.setView(minimap);
            window.draw(minimapBG);
            map.DrawBackground(window); // vẽ lại 
            mytank.Drawtank(window); 
            window.setView(window.getDefaultView());
            window.draw(border);
            
        }
};

int main(){
    constexpr unsigned int WIDTH = 800; // dài 
    constexpr unsigned int HEIGHT = 600; // rộng
    float acceleration = 0.6; // gia tốc
    // int x=WIDTH/2,y=HEIGHT/2;
    float bodysize=36;

    sf::ContextSettings settings;
    settings.antiAliasingLevel=4;
    bool fullscreen=false;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Diep",sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(30);

    float view_width = WIDTH;
    float view_height = HEIGHT;
    sf::View mainView;
    mainView.setSize({view_width, view_height});

    float map_width = 5000;
    float map_height = 5000;
    Map map(map_width, map_height);
    MiniMap minimap(map_width, map_height, window);
    MyTank mytank(0,0,bodysize); // map keo dai tu -1000 den 1000
    
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
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), mainView);
        int dx=mousePos.x-mytank.body.Getx(), dy=mousePos.y-mytank.body.Gety();
        angle=atan2(dy,dx)*180/3.14;

        // std::stringstream ss;
        // ss << mytank.body.position.x <<" "<<mytank.body.position.y<<" "<<angle << ' '<< mytank.body.velocity.x << ' ' << mytank.body.velocity.y;
        // text.setString(ss.str());

        window.clear(sf::Color(220,220,220));
        // CẬP NHẬT
        mytank.update(angle, map);
        mainView.setCenter(mytank.body.position);
        // VẼ SCENE CHÍNH
        window.setView(mainView);
        map.DrawBackground(window);
        mytank.Drawtank(window);
        window.draw(text);
        minimap.drawMinimap(window, mytank, map);

        window.display();
    }

    return 0;
}