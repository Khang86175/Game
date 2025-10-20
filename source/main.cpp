#include <SFML/Graphics.hpp>
#include <fstream>

#include "Bullet.hpp"
#include "Collision.hpp"
#include "Globals.hpp"
#include "Objects.hpp"
#include "Obstacle.hpp"
#include "Tank.hpp"
#include "UI.hpp"
#include "World.hpp"
#include "menu.hpp"
#include "HealthBar.hpp"

enum GameState { MENU, PLAYING, HIGHSCORE, PAUSED };

int main() {
    srand((unsigned)time(nullptr));
    int delay_click = 0;
    float bodysize = 36;

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode({(unsigned)WIDTH,(unsigned)HEIGHT}), "Diep", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(fps);
    sf::View mainview;
    mainview.setSize({WIDTH * 1.5,HEIGHT * 1.5});

    // tải font chữ
    sf::Font font;
    if (!font.openFromFile("arial.ttf")) return -1;
    sf::Text debug(font);
    debug.setCharacterSize(14);
    debug.setFillColor(sf::Color::Red);

    sf::Font uiFont;
    if (!uiFont.openFromFile("Supercell-Magic Regular.ttf")) return -1;
    sf::Texture bg;
    if (!bg.loadFromFile("assets/background.png")) return -1;
    sf::Texture hsTex;
    sf::Texture* hsBG = &bg;
    if (hsTex.loadFromFile("assets/hightscore.png")) hsBG = &hsTex;

    GameState state = MENU;
    StartingScreen start(uiFont, (float)WIDTH, (float)HEIGHT, bg);
    HighScoreScreen high(uiFont, (float)WIDTH, (float)HEIGHT, *hsBG);
    PauseScreen pause(uiFont, (float)WIDTH, (float)HEIGHT);

    std::ifstream cin ("highscore.txt");
    std::string temp;
    if(!cin){
        std::ofstream cout("highscore.txt");
        cout.close();
    }
    else{
        int i=10;
        while(i-- && getline(cin,temp)){
            std::stringstream ss(temp);
            std::string name;
            int score;
            ss >> std::quoted(name) >> score;
            high.addScore(name,score);
        }
    }
    cin.close();
    
    MyTank myTank(0, 0, bodysize, MapSize);
    EnemyTank enemyTank(250,0,bodysize,MapSize,BASIC);
    Minimap minimap((float)WIDTH, (float)HEIGHT, 120.f, MapSize);
    XpBar xpbar((float)WIDTH, (float)HEIGHT, {400.f, 10.f}, myTank, uiFont);
    StatsBar statsbar(uiFont, (int)WIDTH, (int)HEIGHT);
    TankEvolutionUI evolutionUI({WIDTH, HEIGHT});
    if (!evolutionUI.loadTextures())
        return -1;
    Line grid(MapSize);

    std::vector<Obstacle> obs;
    std::vector<Bullet> my_bullets;
    std::vector<Bullet> enemy_bullets;

    auto resetWorld = [&]() {
        myTank.reset(0, 0, bodysize, MapSize);
        my_bullets.clear();
        enemy_bullets.clear();
        obs.clear();
        for (int i = 0; i < 100; ++i) {
            int t = rand() % 3 + 3;
            float x, y;
            do {
                x = rand() % (int)(MapSize * 2) - MapSize;
                y = rand() % (int)(MapSize * 2) - MapSize;
            } while (std::hypot(x, y) <= 200.f);
            obs.emplace_back(x, y, MapSize, t);
        }
    };
    resetWorld();

    int angle = 0;
    bool gameOver = false;
    std::string playerName;

    while (window.isOpen()) {
        while (const std::optional ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>())
                window.close();
            else if (const auto* k = ev->getIf<sf::Event::KeyPressed>()) {
                if (k->scancode == sf::Keyboard::Scancode::Escape) {
                    if (state == MENU)
                        window.close();
                    else if (state == PLAYING)
                        state = PAUSED;
                    else if (state == PAUSED)
                        state = PLAYING;
                }
                if (k->scancode == sf::Keyboard::Scancode::R && state == PLAYING) {
                    myTank.addScore(1000);
                    myTank.levelUp();
                }
                if (k->scancode == sf::Keyboard::Scancode::T && state == PLAYING) {
                    myTank.reset(myTank.body.Getx(),myTank.body.Gety(),bodysize,MapSize);
                    myTank.levelUp();
                }
            } 
            else if (const auto* t = ev->getIf<sf::Event::TextEntered>()) {
                if (state == MENU) start.handleTextInput(t->unicode);
            } 
            else if (const auto* m = ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (state == MENU && m->button == sf::Mouse::Button::Left){
                    start.handleNameBoxClick(sf::Mouse::getPosition(window));
                }
            }
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        int dx = mousePos.x - (int)window.getSize().x / 2;
        int dy = mousePos.y - (int)window.getSize().y / 2;
        angle = (int)(std::atan2((float)dy, (float)dx) * 180.f / 3.14f);

        if (state == MENU) {
            if (start.checkStartClick(mousePos)&& delay_click == 0 ) {
                state = PLAYING;
                playerName = start.getPlayerName();
                gameOver = false;
                delay_click=10;
                resetWorld();
            }
            if (start.checkHighscoreClick(mousePos) && delay_click == 0) {
                state = HIGHSCORE;
                delay_click=10;
                high.updateDisplay();
            }
            window.clear(sf::Color(204, 204, 204));
            start.checkHover(mousePos);
            start.draw(window);
            window.display();
            if(delay_click > 0)
                delay_click--;
            continue;
        }

        if (state == HIGHSCORE) {
            if (high.checkBackClick(mousePos) && delay_click == 0){
                delay_click = 10;
                state = MENU;
            }
            window.clear(sf::Color(204, 204, 204));
            high.checkHover(mousePos);
            high.draw(window);
            window.display();
            if(delay_click > 0)
                delay_click--;
            continue;
        }

        if (state == PAUSED) {
            window.clear(sf::Color(204, 204, 204));
            mainview.setCenter(myTank.body.position);
            window.setView(mainview);
            grid.draw(window);
            for (auto& b : my_bullets) b.draw(window);
            for (auto& o : obs) o.DrawObs(window);
            myTank.Drawtank(window);
            window.setView(window.getDefaultView());
            xpbar.draw(window);
            minimap.Drawmap(window, myTank.body.position);
            pause.checkHover(mousePos);
            if (pause.clickResume(mousePos) && delay_click==0){
                state = PLAYING;
                delay_click=10;
            }
            else if (pause.clickMenu(mousePos) && delay_click==0) {
                state = MENU;
                delay_click=10;
                continue;
            }
            if(delay_click > 0)
                delay_click--;
            pause.draw(window);
            window.display();
            continue;
        }

        if (!gameOver) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
                myTank.moveY(-1);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
                myTank.moveY(1);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
                myTank.moveX(-1);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
                myTank.moveX(1);

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
                myTank.shoot(my_bullets, angle);

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && delay_click == 0){
                if (evolutionUI.isVisible()) {
                    TankType selected = evolutionUI.getSelectedEvolution(mousePos);
                    if (selected != myTank.getTankType()){
                        if (myTank.evolveTank(selected)){
                            evolutionUI.setVisible(false);
                            delay_click = 10;
                        }
                    }
                }
                // Chọn nâng cấp
                if (statsbar.isVisible()) {
                    int statIndex = statsbar.checkButtonClick(mousePos);
                    if(statIndex != -1){
                        myTank.upgradeStat(statIndex);
                        delay_click = 10;
                    }
                }
            }
            int botangle=enemyTank.NextMove(myTank);
            myTank.update(angle);
            enemyTank.update(botangle);
            enemyTank.shoot(enemy_bullets,botangle);
            xpbar.update(myTank);
            statsbar.update(myTank);
            evolutionUI.update(myTank);

            // std::stringstream ss;
            // ss << "Level: " << myTank.level << "\nScore: " << myTank.score << "\nXp_base: " << myTank.xp_base << "/"
            //    << myTank.xp_to_lv_up << "\n"
            //    << "HP: " << (int)myTank.body.hp << "/" << (int)myTank.body.maxhp << "\n"
            //    << "HP_regen: " << myTank.body.hp_regen << "\nBody_dmg: " << myTank.body.body_dmg
            //    << "bSpeed: " << myTank.getBulletSpeed() << "\n"
            //    << "bLife: " << myTank.getBulletLife() << "\n"
            //    << "bDmg: " << myTank.getBulletDamage() << "\n";
            // debug.setString(ss.str());

            for (int i = (int)my_bullets.size() - 1; i >= 0; --i) {
                my_bullets[i].update();
                if (!my_bullets[i].alive) removeBullet(my_bullets, i);
            }
            for (int i = (int)enemy_bullets.size() -1 ; i>=0; --i){
                enemy_bullets[i].update();
                if(!enemy_bullets[i].alive)
                    removeBullet(enemy_bullets,i);
            }
            for (int i = (int)obs.size() - 1 ; i >= 0; --i){
                obs[i].update();
                if(!obs[i].alive && obs[i].timeToRespawn <= currentFrame )
                    obs[i].respawn(obs,myTank.body);
            }

            handleTankObstacleCollision(myTank, obs);
            handleBotObstacleCollision(enemyTank,obs);
            handleBulletObstacleCollision(my_bullets, obs, myTank);
            handleBulletTankCollision(enemy_bullets, myTank.body);
            handleBulletTankCollision(my_bullets,enemyTank.body);
            handleBullet_BulletCollision(enemy_bullets, my_bullets);
            handle2ObstacleCollision(obs);
            handleTankBotCollision(myTank,enemyTank);

            if (myTank.body.hp <= 0) {
                gameOver = true;
                high.addScore(playerName, myTank.score);
                std::ofstream cout("highscore.txt");
                for(int i=0;i<10;i++){
                    if(high.scores[i].name=="" )
                        break;
                    cout << "\"" << high.scores[i].name<<"\" "<< high.scores[i].score <<'\n';
                }
                cout.close();
                state = MENU;
            }
        }

        window.clear(sf::Color(204, 204, 204));
        mainview.setCenter(myTank.body.position);
        window.setView(mainview);
        grid.draw(window);
        for (auto& b : my_bullets) b.draw(window);
        for (auto& b : enemy_bullets) b.draw(window);
        for (auto& o : obs) o.DrawObs(window);
        myTank.Drawtank(window);
        enemyTank.Drawtank(window);

        // Draw UI
        window.setView(window.getDefaultView());
        statsbar.draw(window, uiFont);
        evolutionUI.draw(window);
        xpbar.draw(window);
        minimap.Drawmap(window,myTank.body.position,enemyTank.body.position);
        //window.draw(debug);
        window.display();

        if (state != PAUSED) 
            currentFrame++;
        if(delay_click > 0)
            delay_click--;
    }
    return 0;
}
