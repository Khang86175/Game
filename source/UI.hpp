#pragma once
#include <SFML/Graphics.hpp>
#include <sstream>
#include "Tank.hpp"

class Minimap{
public:
    sf::RectangleShape map;
    sf::CircleShape player;
    sf::CircleShape enemy;
    int Mapsize;
    Minimap(float x, float y, float size,int Mapsize);
    void Drawmap(sf::RenderWindow &window,sf::Vector2f playerpos={120,120},sf::Vector2f enemypos={2000,2000});
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
    XpBar(float x, float y, sf::Vector2f size, MyTank &mytank,sf::Font &font);
    void update(MyTank &mytank);
    void draw(sf::RenderWindow &window);
};
class Bar{
public:
    sf::RectangleShape barBG;
    sf::RectangleShape fill;
    sf::RectangleShape button;
    sf::Text name, idxText;
    int  index;
    sf::Color color;
   
    Bar(sf::Vector2f size, sf::Font &f, sf::Color color);
    void updatePosition( sf::Vector2f position);
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
    StatsBar(sf::Font &f, int w, int h);
    void update(MyTank &mytank);
    int checkButtonClick(sf::Vector2i mousePos);
    void draw(sf::RenderWindow &window, sf::Font &font);
    bool isVisible();
};

class Option {
private:
    TankType type;
    sf::Sprite sprite;
    // sf::FloatRect bounds;
    // sf::Vector2f position;
    // bool isClickable;
public:
    // sf::RectangleShape button;
    Option(TankType t, sf::Texture &texture, sf::Vector2f pos);
    // void setPosition(sf::Vector2f pos);
    void draw(sf::RenderWindow &window);
    TankType getType() { return type; }
    bool checkButtonClick(sf::Vector2i mousePos);
    // void setClickable(bool clickable) { isClickable = clickable; }
    // bool getClickable() const { return isClickable; }
    sf::Sprite& getSprite() { return sprite; }
};
class TankEvolutionUI {
private:
    std::vector<Option> options;
    bool visible;
    sf::Vector2f position;
    sf::Vector2f windowSize;
    std::vector<sf::Texture> textures;
    std::vector<std::string> texturePaths = {
        "assets/tank/Twin.png", 
        "assets/tank/Sniper.png", 
        "assets/tank/MachineGun.png", 
        "assets/tank/Triple.png", 
        "assets/tank/Assassin.png", 
        "assets/tank/Destroyer.png"
    };
    TankType currentTankType;
    std::vector<TankType> availableEvolutions;
    
public:
    float button_size = 100;
    TankEvolutionUI(sf::Vector2f windowSize, float spacing = 80.f);
    ~TankEvolutionUI();
    bool loadTextures();
    void updateAvailableEvolutions(TankType currentType, bool canEvolve);
    void updateOptions();
    void update(MyTank &mytank);
    void draw(sf::RenderWindow &window);
    TankType getSelectedEvolution(sf::Vector2i mousePos);
    void setVisible(bool v) { visible = v; }
    bool isVisible() const { return visible; }
    void setPosition(sf::Vector2f pos) { position = pos; }
};