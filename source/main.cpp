#include <SFML/Graphics.hpp>

#include "Bullet.hpp"
#include "Collision.hpp"
#include "Globals.hpp"
#include "Objects.hpp"
#include "Obstacle.hpp"
#include "Tank.hpp"
#include "UI.hpp"
#include "World.hpp"
#include "menu.hpp"

enum GameState { MENU, PLAYING, HIGHSCORE, PAUSED ,GAMEOVER};

int main() {
    srand((unsigned)time(nullptr));
    int delay_click = 0;
    float bodysize = 36;
    int finalScore = 0;
    int gameOverEndFrame = -1;
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Diep", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(fps);
    sf::View mainview;
    mainview.setSize({WIDTH * 1.5, HEIGHT * 1.5});

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
    GameOverScreen gameOverScreen(SupercellMagic, WIDTH, HEIGHT, fps);

    MyTank myTank(0, 0, bodysize, MapSize);
    EnemyTank enemyTank(150, 0, bodysize, MapSize, 0);
    Minimap mmap((float)WIDTH, (float)HEIGHT, 120.f, MapSize);
    XpBar xpbar((float)WIDTH, (float)HEIGHT, {400.f, 10.f}, myTank, uiFont);
    StatsBar sbar(uiFont, (int)WIDTH, (int)HEIGHT);
    Line grid(MapSize);

    std::vector<Obstacle> obs;
    std::vector<Bullet> my_bullets, enemy_bullets;

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
                if (state == MENU && m->button == sf::Mouse::Button::Left)
                    start.handleNameBoxClick(sf::Mouse::getPosition(window));
            }
        }

        sf::Vector2i mp = sf::Mouse::getPosition(window);
        int dx = mp.x - (int)window.getSize().x / 2, dy = mp.y - (int)window.getSize().y / 2;
        angle = (int)(std::atan2((float)dy, (float)dx) * 180.f / 3.14f);

        if (state == MENU) {
            if (start.checkStartClick(mp)) {
                state = PLAYING;
                playerName = start.getPlayerName();
                gameOver = false;
                resetWorld();
            }
            if (start.checkHighscoreClick(mp)) {
                state = HIGHSCORE;
                high.updateDisplay();
            }
            window.clear(sf::Color(204, 204, 204));
            start.checkHover(mp);
            start.draw(window);
            window.display();
            continue;
        }

        if (state == HIGHSCORE) {
            if (high.checkBackClick(mp)) state = MENU;
            window.clear(sf::Color(204, 204, 204));
            high.checkHover(mp);
            high.draw(window);
            window.display();
            continue;
        }
        if (state == GAMEOVER) {
            gameOverScreen.draw(window, mainView, defaultView,
                                line, bullets, obs, myTank, xpbar,
                                minimap, highScoreScreen, playerName, state);
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
            mmap.Drawmap(window, myTank.body.position);
            pause.checkHover(mp);
            if (pause.clickResume(mp))
                state = PLAYING;
            else if (pause.clickMenu(mp)) {
                state = MENU;
                continue;
            }
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

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && sbar.isVisible() && delay_click == 0) {
                int idx = sbar.checkButtonClick(mp);
                if (idx != -1) myTank.upgradeStat(idx);
                delay_click = 10;
            }
            delay_click = std::max(0, delay_click - 1);

            myTank.update(angle);
            xpbar.update(myTank);
            sbar.update(myTank);

            std::stringstream ss;
            ss << "Level: " << myTank.level << "\nScore: " << myTank.score << "\nXp_base: " << myTank.xp_base << "/"
               << myTank.xp_to_lv_up << "\n"
               << "HP: " << (int)myTank.body.hp << "/" << (int)myTank.body.maxhp << "\n"
               << "HP_regen: " << myTank.body.hp_regen << "\nBody_dmg: " << myTank.body.body_dmg
               << "bSpeed: " << myTank.getBulletSpeed() << "\n"
               << "bLife: " << myTank.getBulletLife() << "\n"
               << "bDmg: " << myTank.getBulletDamage() << "\n";
            debug.setString(ss.str());

            for (int i = (int)my_bullets.size() - 1; i >= 0; --i) {
                my_bullets[i].update();
                if (!my_bullets[i].alive) removeBullet(my_bullets, i);
            }
            for (auto& o : obs) o.update();
            handleTankObstacleCollision(myTank, obs);
            handleBulletObstacleCollision(my_bullets, obs, myTank);
            handleBulletTankCollision(enemy_bullets, myTank);
            handleBullet_BulletCollision(enemy_bullets, my_bullets);

            if (myTank.body.hp <= 0) {
                gameOver = true;
                gameOverScreen.setFinalScore(myTank.score);
                state = GAMEOVER;
                gameOverScreen.reset();
            }
        }
        


        window.clear(sf::Color(204, 204, 204));
        mainview.setCenter(myTank.body.position);
        window.setView(mainview);
        grid.draw(window);
        for (auto& b : my_bullets) b.draw(window);
        for (auto& o : obs) o.DrawObs(window);
        myTank.Drawtank(window);
        window.setView(window.getDefaultView());
        sbar.draw(window, uiFont);
        xpbar.draw(window);
        mmap.Drawmap(window, myTank.body.position);
        window.draw(debug);
        window.display();

        if (state != PAUSED) 
            currentFrame++;
    }
    return 0;
}
