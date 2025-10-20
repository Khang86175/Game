#include "menu.hpp"
#include <algorithm>
#include <sstream>

using namespace sf;

// ---------------- HIGH SCORE SCREEN ---------------- //
HighScoreScreen::HighScoreScreen(Font& f, float w, float h, Texture& bg)
    : background(bg), backText(f), titleText(f), backHovered(false)
{
    // Background
    FloatRect b = background.getLocalBounds();
    background.setOrigin(b.size / 2.f);
    background.setPosition({ w / 2.f, h / 2.f });
    background.setScale({ w / b.size.x, h / b.size.y });

    // Title
    titleText.setString("HIGH SCORES");
    titleText.setCharacterSize(30);
    titleText.setFillColor(Color::White);
    titleText.setOutlineThickness(3);
    titleText.setOutlineColor(Color::Black);
    FloatRect tb = titleText.getLocalBounds();
    titleText.setOrigin(tb.size / 2.f);
    titleText.setPosition({ w / 2.f, 60 });

    // Back button
    backButton.setSize({ 200, 60 });
    backButton.setOrigin({ 100, 30 });
    backButton.setPosition({ w / 2.f, h - 50 });
    backButton.setFillColor(Color(0, 178, 225));
    backButton.setOutlineThickness(4);
    backButton.setOutlineColor(Color(14, 144, 178));

    // Back text
    backText.setString("BACK");
    backText.setCharacterSize(30);
    backText.setFillColor(Color::White);
    backText.setOutlineThickness(2);
    backText.setOutlineColor(Color::Black);
    FloatRect bb = backText.getLocalBounds();
    backText.setOrigin(bb.size / 2.f);
    backText.setPosition(backButton.getPosition());

    // Score texts
    scoreTexts.reserve(10);
    for (int i = 0; i < 10; ++i) {
        scores.push_back({ "", 0 });
        Text t(f);
        t.setCharacterSize(24);
        t.setFillColor(Color::White);
        t.setOutlineThickness(2);
        t.setOutlineColor(Color::Black);
        t.setPosition({ w / 2.f - 200, 150 + i * 35 });
        scoreTexts.push_back(t);
    }
}

void HighScoreScreen::addScore(const std::string& n, int s)
{
    scores.push_back({ n, s });
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) {
            return a.score > b.score;
        });

    if (scores.size() > 10)
        scores.resize(10);
}

void HighScoreScreen::updateDisplay()
{
    for (int i = 0; i < 10; ++i) {
        std::stringstream ss;
        ss << (i + 1) << ". ";
        if (i < (int)scores.size() && scores[i].score > 0)
            ss << scores[i].name << " - " << scores[i].score;
        else
            ss << "---";
        scoreTexts[i].setString(ss.str());
    }
}

void HighScoreScreen::checkHover(Vector2i m)
{
    bool h = backButton.getGlobalBounds().contains(Vector2f(m));
    backButton.setFillColor(h ? Color(0, 200, 255) : Color(0, 178, 225));
    backButton.setOutlineColor(h ? Color(20, 160, 200) : Color(14, 144, 178));
    Vector2f scale = h ? Vector2f(1.05f, 1.05f) : Vector2f(1.f, 1.f);
    backButton.setScale(scale);
    backText.setScale(scale);
    backHovered = h;
}

bool HighScoreScreen::checkBackClick(Vector2i m)
{
    return backButton.getGlobalBounds().contains(Vector2f(m)) &&
           sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
}

void HighScoreScreen::draw(RenderWindow& w)
{
    w.draw(background);
    w.draw(titleText);
    for (auto& t : scoreTexts)
        w.draw(t);
    w.draw(backButton);
    w.draw(backText);
}


// ---------------- STARTING SCREEN ---------------- //
StartingScreen::StartingScreen(Font& f, float w, float h, Texture& bg)
    : startText(f), highscoreText(f),
      startHovered(false), highscoreHovered(false),
      background(bg), nameLabel(f), nameText(f),
      playerName(""), isTyping(false)
{
    // Background
    FloatRect b = background.getLocalBounds();
    background.setOrigin(b.size / 2.f);
    background.setPosition({ w / 2.f, h / 2.f });
    background.setScale({ w / b.size.x, h / b.size.y });

    // Buttons
    startButton.setSize({ 200, 60 });
    startButton.setOrigin({ 100, 30 });
    startButton.setPosition({ w / 2.f, h / 2.f + 100 });
    startButton.setFillColor(Color(0, 178, 225));
    startButton.setOutlineThickness(4);
    startButton.setOutlineColor(Color(14, 144, 178));

    startText.setString("START");
    startText.setCharacterSize(30);
    startText.setFillColor(Color::White);
    startText.setOutlineThickness(2);
    startText.setOutlineColor(Color::Black);
    FloatRect st = startText.getLocalBounds();
    startText.setOrigin(st.size / 2.f);
    startText.setPosition(startButton.getPosition());

    highscoreButton.setSize({ 200, 60 });
    highscoreButton.setOrigin({ 100, 30 });
    highscoreButton.setPosition({ w / 2.f, h / 2.f + 180 });
    highscoreButton.setFillColor(Color(255, 178, 0));
    highscoreButton.setOutlineThickness(4);
    highscoreButton.setOutlineColor(Color(200, 140, 0));

    highscoreText.setString("HIGH SCORES");
    highscoreText.setCharacterSize(26);
    highscoreText.setFillColor(Color::White);
    highscoreText.setOutlineThickness(2);
    highscoreText.setOutlineColor(Color::Black);
    FloatRect ht = highscoreText.getLocalBounds();
    highscoreText.setOrigin(ht.size / 2.f);
    highscoreText.setPosition(highscoreButton.getPosition());

    // Name input
    nameLabel.setString("NAME:");
    nameLabel.setCharacterSize(20);
    nameLabel.setFillColor(Color::White);
    nameLabel.setOutlineThickness(2);
    nameLabel.setOutlineColor(Color::Black);
    FloatRect nl = nameLabel.getLocalBounds();
    nameLabel.setOrigin(nl.size / 2.f);
    nameLabel.setPosition({ w / 2.f, h / 2.f - 50 });

    nameBox.setSize({ 300, 50 });
    nameBox.setOrigin({ 150, 25 });
    nameBox.setPosition({ w / 2.f, h / 2.f });
    nameBox.setFillColor(Color(255, 255, 255, 200));
    nameBox.setOutlineThickness(3);
    nameBox.setOutlineColor(Color(14, 144, 178));

    nameText.setCharacterSize(24);
    nameText.setFillColor(Color::Black);
    nameText.setPosition({ w / 2.f - 140, h / 2.f - 15 });
}

void StartingScreen::handleTextInput(char32_t u)
{
    if (u == 8) { // Backspace
        if (!playerName.empty()) {
            playerName.pop_back();
            nameText.setString(playerName);
        }
    }
    else if (playerName.length() < 10) {
        if (std::isalnum(u)) {
            playerName += static_cast<char>(u);
            nameText.setString(playerName);
        }
    }
}

void StartingScreen::handleNameBoxClick(Vector2i m)
{
    bool in = nameBox.getGlobalBounds().contains(Vector2f(m));
    isTyping = in;
    nameBox.setOutlineColor(in ? Color(0, 200, 255) : Color(14, 144, 178));
}

std::string StartingScreen::getPlayerName() const
{
    return playerName.empty() ? "Player" : playerName;
}

void StartingScreen::checkHover(Vector2i m)
{
    bool s = startButton.getGlobalBounds().contains(Vector2f(m));
    startButton.setFillColor(s ? Color(0, 200, 255) : Color(0, 178, 225));
    startButton.setOutlineColor(s ? Color(20, 160, 200) : Color(14, 144, 178));
    startButton.setScale(s ? Vector2f(1.05f, 1.05f) : Vector2f(1.f, 1.f));
    startText.setScale(s ? Vector2f(1.05f, 1.05f) : Vector2f(1.f, 1.f));

    bool h = highscoreButton.getGlobalBounds().contains(Vector2f(m));
    highscoreButton.setFillColor(h ? Color(255, 200, 50) : Color(255, 178, 0));
    highscoreButton.setOutlineColor(h ? Color(220, 160, 20) : Color(200, 140, 0));
    highscoreButton.setScale(h ? Vector2f(1.05f, 1.05f) : Vector2f(1.f, 1.f));
    highscoreText.setScale(h ? Vector2f(1.05f, 1.05f) : Vector2f(1.f, 1.f));
}

bool StartingScreen::checkStartClick(Vector2i m)
{
    return startButton.getGlobalBounds().contains(Vector2f(m)) &&
           Mouse::isButtonPressed(Mouse::Button::Left);
}

bool StartingScreen::checkHighscoreClick(Vector2i m)
{
    return highscoreButton.getGlobalBounds().contains(Vector2f(m)) &&
           Mouse::isButtonPressed(Mouse::Button::Left);
}

void StartingScreen::draw(RenderWindow& w)
{
    w.draw(background);
    w.draw(nameLabel);
    w.draw(nameBox);
    w.draw(nameText);
    w.draw(startButton);
    w.draw(startText);
    w.draw(highscoreButton);
    w.draw(highscoreText);
}


// ---------------- PAUSE SCREEN ---------------- //
PauseScreen::PauseScreen(Font& f, float w, float h)
    : title(f), resumeText(f), menuText(f)
{
    dim.setSize({ w, h });
    dim.setFillColor(Color(0, 0, 0, 140));

    title.setString("PAUSED");
    title.setCharacterSize(50);
    title.setFillColor(Color::White);
    title.setOutlineThickness(3);
    title.setOutlineColor(Color::Black);
    FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.size / 2.f);
    title.setPosition({ w / 2.f, h / 2.f - 100 });

    resumeButton.setSize({ 220, 60 });
    resumeButton.setOrigin({ 110, 30 });
    resumeButton.setPosition({ w / 2.f, h / 2.f -30 });
    resumeButton.setFillColor(Color(0, 178, 225));
    resumeButton.setOutlineThickness(4);
    resumeButton.setOutlineColor(Color(14, 144, 178));

    resumeText.setString("RESUME");
    resumeText.setCharacterSize(28);
    resumeText.setFillColor(Color::White);
    resumeText.setOutlineThickness(2);
    resumeText.setOutlineColor(Color::Black);
    FloatRect rb = resumeText.getLocalBounds();
    resumeText.setOrigin(rb.size / 2.f);
    resumeText.setPosition(resumeButton.getPosition());

    menuButton.setSize({ 220, 60 });
    menuButton.setOrigin({ 110, 30 });
    menuButton.setPosition({ w / 2.f, h / 2.f + 50 });
    menuButton.setFillColor(Color(255, 178, 0));
    menuButton.setOutlineThickness(4);
    menuButton.setOutlineColor(Color(200, 140, 0));

    menuText.setString("MAIN MENU");
    menuText.setCharacterSize(28);
    menuText.setFillColor(Color::White);
    menuText.setOutlineThickness(2);
    menuText.setOutlineColor(Color::Black);
    FloatRect mb = menuText.getLocalBounds();
    menuText.setOrigin(mb.size / 2.f);
    menuText.setPosition(menuButton.getPosition());
}

void PauseScreen::checkHover(Vector2i m)
{
    bool r = resumeButton.getGlobalBounds().contains(Vector2f(m));
    resumeButton.setFillColor(r ? Color(0, 200, 255) : Color(0, 178, 225));
    resumeButton.setOutlineColor(r ? Color(20, 160, 200) : Color(14, 144, 178));

    bool n = menuButton.getGlobalBounds().contains(Vector2f(m));
    menuButton.setFillColor(n ? Color(255, 200, 50) : Color(255, 178, 0));
    menuButton.setOutlineColor(n ? Color(220, 160, 20) : Color(200, 140, 0));
}

bool PauseScreen::clickResume(Vector2i m)
{
    return resumeButton.getGlobalBounds().contains(Vector2f(m)) &&
           Mouse::isButtonPressed(Mouse::Button::Left);
}

bool PauseScreen::clickMenu(Vector2i m)
{
    return menuButton.getGlobalBounds().contains(Vector2f(m)) &&
           Mouse::isButtonPressed(Mouse::Button::Left);
}

void PauseScreen::draw(RenderWindow& w)
{
    w.draw(dim);
    w.draw(title);
    w.draw(resumeButton);
    w.draw(resumeText);
    w.draw(menuButton);
    w.draw(menuText);
}
