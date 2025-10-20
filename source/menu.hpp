#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct HighScoreEntry {
    std::string name;
    int score;
    HighScoreEntry(std::string n = "", int s = 0) : name(n), score(s) {}
};

class HighScoreScreen {
private:
    sf::Sprite background;
    sf::RectangleShape backButton;
    sf::Text backText;
    sf::Text titleText;

    std::vector<sf::Text> scoreTexts;
    std::vector<HighScoreEntry> scores;
    bool backHovered;
public:
    HighScoreScreen(sf::Font& f, float w, float h, sf::Texture& bgTexture);

    void addScore(const std::string& name, int score);
    void updateDisplay();
    void checkHover(sf::Vector2i mousepos);
    bool checkBackClick(sf::Vector2i mousepos);
    void draw(sf::RenderWindow& window);
};

class StartingScreen {
private:
    sf::Sprite background;

    sf::RectangleShape startButton;
    sf::RectangleShape highscoreButton;
    sf::RectangleShape nameBox;

    sf::Text startText;
    sf::Text highscoreText;
    sf::Text nameLabel;
    sf::Text nameText;

    std::string playerName;
    bool startHovered;
    bool highscoreHovered;
    bool isTyping;
    
    
public:
    StartingScreen(sf::Font& f, float w, float h, sf::Texture& bgTexture);

    void handleTextInput(char32_t unicode);
    void handleNameBoxClick(sf::Vector2i mousepos);
    std::string getPlayerName() const;
    void checkHover(sf::Vector2i mousepos);
    bool checkStartClick(sf::Vector2i mousepos);
    bool checkHighscoreClick(sf::Vector2i mousepos);
    void draw(sf::RenderWindow& window);
};

class PauseScreen {
private:
    sf::RectangleShape dim;
    sf::Text title;

    sf::RectangleShape resumeButton;
    sf::Text resumeText;

    sf::RectangleShape menuButton;
    sf::Text menuText;
public:
    PauseScreen(sf::Font& f, float w, float h);

    void checkHover(sf::Vector2i mp);
    bool clickResume(sf::Vector2i mp);
    bool clickMenu(sf::Vector2i mp);
    void draw(sf::RenderWindow& window);
};
