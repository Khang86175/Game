#include <SFML/Graphics.hpp>


#include "Objects.hpp"
#include "Bullet.hpp"
#include "Globals.hpp"
#include "Collision.hpp"
#include "Obstacle.hpp"
#include "Tank.hpp"
#include "World.hpp"
#include "UI.hpp"

int main(){
    srand(time(0));
    int delay_click=0;
    float bodysize=36;

    sf::ContextSettings settings;
    settings.antiAliasingLevel=4;
    bool fullscreen=false;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Diep",sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(fps);
    sf::View mainview;
    mainview.setSize({(float)WIDTH*1.5, (float)HEIGHT*1.5});

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


    MyTank mytank(0,0,bodysize,MapSize);
    EnemyTank enemyTank(150,0,bodysize,MapSize,0);
    Minimap minimap(WIDTH,HEIGHT,120,MapSize);
    XpBar xpbar(WIDTH, HEIGHT, {400, 10}, mytank, SupercellMagic);
    StatsBar statsbar(SupercellMagic, WIDTH, HEIGHT);
    Line line(MapSize);

    std::vector<Obstacle> obs;
    std::vector<Bullet> my_bullets;
    std::vector<Bullet> enemy_bullets;

    for(int i=0;i<100;i++){
        int type=rand()%3+3;
        float x, y;
        // Không sinh obstacle quanh điểm spawn
        do{
            x = rand()%(int)(MapSize*2)-MapSize;
            y = rand()%(int)(MapSize*2)-MapSize;
        }while (std::hypot(x, y) <= 200.f);
        obs.push_back(Obstacle(x,y,MapSize,type));
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
            mytank.moveY(-1);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)){
            mytank.moveY(1);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)){
            mytank.moveX(-1);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)){
            mytank.moveX(1);
        }

        // tính góc nòng súng
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int dx=mousePos.x-window.getSize().x/2, dy=mousePos.y-window.getSize().y/2;
        angle=atan2(dy,dx)*180/3.14;

        // Shootings
        if((sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))){
            mytank.shoot(my_bullets, angle);
            enemyTank.shoot(enemy_bullets,angle);
        }

        // Chọn nâng cấp
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && statsbar.isVisible() && delay_click==0){
            int statIndex = statsbar.checkButtonClick(mousePos);
            if(statIndex != -1){
                mytank.upgradeStat(statIndex);
            }
            delay_click=15;
        }
        if(delay_click >0)
            delay_click--;

        std::stringstream ss;

        mytank.update(angle);
        enemyTank.update(angle);
        xpbar.update(mytank);
        statsbar.update(mytank);

        // ss << "Mouse pos: " <<mousePos.x <<" "<<mousePos.y<<"\n"<<"angle: "<<angle << '\n'<< "Tank pos: " << mytank.body.position.x << ' ' << mytank.body.position.y << '\n' << "tank speed: " << mytank.body.velocity.x << ' ' << mytank.body.velocity.y<<'\n'<<"window size: "<<window.getSize().x <<' ' << window.getSize().y;
        ss << "Level: " << mytank.level << '\n'<< "Score: " << mytank.score << '\n' << "Xp_base: " << mytank.xp_base << "/" << mytank.xp_to_lv_up << '\n'<< "HP: " << (int)mytank.body.hp << "/" << (int)mytank.body.maxhp;
        // ss << "Reload: " << mytank.tankBasic->gun.reload;
        ss << '\n' << "bSpeed: " << mytank.getBulletSpeed() << '\n' << "bLife: " << mytank.getBulletLife() << '\n' << "bDmg: " << mytank.getBulletDamage() << '\n';
        ss << my_bullets.size() <<' '<<enemy_bullets.size();
        ss << '\n'<<enemyTank.getBulletDamage();
        text.setString(ss.str()); 
        
        
        
        for (int i = my_bullets.size() - 1; i >= 0; --i) {
            my_bullets[i].update();
            if (!my_bullets[i].alive)
                removeBullet(my_bullets, i);
        }
        for (int i = enemy_bullets.size() - 1; i >= 0; --i){
            enemy_bullets[i].update();
            if (!enemy_bullets[i].alive)
                removeBullet(enemy_bullets,i);
        }
        for (auto& o : obs) {
            o.update();
            if(!o.alive)
                o.respawn(obs,mytank.body);
        }

         
        handleBulletObstacleCollision(my_bullets, obs, mytank);
        handleBulletTankCollision(enemy_bullets, mytank);
        handleBulletTankCollision(my_bullets,enemyTank);
        handleBullet_BulletCollision(enemy_bullets,my_bullets);
        handleTankObstacleCollision(mytank,obs);
        handle2TankCollision(mytank,enemyTank);
        handle2ObstacleCollision(obs);
        
        //Vẽ
        window.clear(sf::Color(204, 204, 204));

        mainview.setCenter(mytank.body.position);
        window.setView(mainview);
        line.draw(window);
        for (auto& b : my_bullets) b.draw(window);
        for (auto& b : enemy_bullets) b.draw(window);
        for (auto& o : obs) o.DrawObs(window);
        mytank.Drawtank(window);
        enemyTank.Drawtank(window);

        
        //vẽ UI
        window.setView(window.getDefaultView());
        statsbar.draw(window,SupercellMagic);
        xpbar.draw(window);
        minimap.Drawmap(window,mytank.body.position);
        window.draw(text);

        window.display();

        currentFrame++;
    }

    return 0;
}
