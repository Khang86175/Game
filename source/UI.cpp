#include "UI.hpp"


Minimap::Minimap(float x, float y, float size,int Mapsize):Mapsize(Mapsize){
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
void Minimap::Drawmap(sf::RenderWindow &window,sf::Vector2f playerpos,sf::Vector2f enemypos){
    player.setPosition({playerpos.x/Mapsize*map.getSize().x/2+map.getPosition().x, playerpos.y/Mapsize*map.getSize().y/2+map.getPosition().y});
    enemy.setPosition({enemypos.x/Mapsize*map.getSize().x/2+map.getPosition().x,enemypos.y/Mapsize*map.getSize().y/2+map.getPosition().y});
    window.draw(map);
    window.draw(player);
    if(enemypos.x != 2000 && enemypos.y != 2000)
        window.draw(enemy);
}

XpBar::XpBar(float x, float y, sf::Vector2f size, MyTank &mytank,sf::Font &font) : pos(x,y), xpText(font), scoreText(font), size(size){
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
void XpBar::update(MyTank &mytank){
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
void XpBar::draw(sf::RenderWindow &window){
    window.draw(xpBarBG);
    window.draw(xpBar);
    window.draw(xpText);
    window.draw(scoreText);
}

Bar::Bar(sf::Vector2f size, sf::Font &f, sf::Color color):name(f), color(color), idxText(f){
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
void Bar::updatePosition( sf::Vector2f position){
    barBG.setPosition(position);
    fill.setPosition(position);
    button.setPosition({position.x + barBG.getSize().x + 5, position.y});
    name.setPosition({position.x +30, position.y + barBG.getSize().y/2-4});
    idxText.setPosition({button.getPosition().x + button.getSize().x + 10, position.y + barBG.getSize().y/2 - 6});
}

StatsBar::StatsBar(sf::Font &f, int w, int h):pointsText(f), visible(false){
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
void StatsBar::update(MyTank &mytank){
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
int StatsBar::checkButtonClick(sf::Vector2i mousePos){
    for (int i = 0; i < 8; i++){
        if (bars[i].button.getGlobalBounds().contains(sf::Vector2f(mousePos))){
            return i;
        }
    }
    return -1;
}
void StatsBar::draw(sf::RenderWindow &window, sf::Font &font){
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
bool StatsBar::isVisible(){
    return  visible;
}

Option::Option(TankType t, sf::Texture &texture, sf::Vector2f pos)
    : type(t), sprite(texture){
    sprite.setScale({0.1f, 0.1f});
    // sf::FloatRect bounds = sprite.getLocalBounds();
    // button.setSize(bounds.size);
    // button.setFillColor(sf::Color::Transparent);
    sprite.setPosition(pos);
    // button.setPosition(pos);
}

void Option::draw(sf::RenderWindow &window) {
    window.draw(sprite);
}
bool Option::checkButtonClick(sf::Vector2i mousePos){
    return sprite.getGlobalBounds().contains(sf::Vector2f(mousePos));
}

TankEvolutionUI::TankEvolutionUI(sf::Vector2f wSize, float spacing)
    : visible(false), windowSize(wSize), currentTankType(TankType::BASIC) {
    position.x = 15;
    position.y = 15;
}
TankEvolutionUI::~TankEvolutionUI() {
    options.clear();
}
bool TankEvolutionUI::loadTextures() {
    textures.clear();
    textures.resize(texturePaths.size());
    for (int i = 0; i < texturePaths.size(); i++) {
        if (!textures[i].loadFromFile(texturePaths[i])) {
            return false;
        }
    }
    return true;
}
void TankEvolutionUI::updateAvailableEvolutions(TankType currentType, bool canEvolve) {
    currentTankType = currentType;
    availableEvolutions.clear();
    if (!canEvolve) {
        return;
    }
    if (currentType == TankType::BASIC) {
        availableEvolutions.push_back(TankType::TWIN);
        availableEvolutions.push_back(TankType::SNIPER);
    } 
    else if (currentType == TankType::TWIN) {
        availableEvolutions.push_back(TankType::TRIPLE);
        availableEvolutions.push_back(TankType::MACHINE_GUN);
    } 
    else if (currentType == TankType::SNIPER) {
        availableEvolutions.push_back(TankType::ASSASSIN);
    } 
    else if (currentType == TankType::MACHINE_GUN){
        availableEvolutions.push_back(TankType::DESTROYER);
    }
}
void TankEvolutionUI::updateOptions() {
    options.clear();
    int textureIndices[] = {
        -1, // BASIC
        0, // TWIN
        1, // SNIPER
        2, // MACHINE_GUN
        3, // TRIPLE
        4, // ASSASSIN
        5  // DESTROYER
    };
    for (size_t i = 0; i < availableEvolutions.size(); i++) {
        TankType type = availableEvolutions[i];
        int textureIdx = textureIndices[static_cast<int>(type)];
        if (textureIdx < 0 || textureIdx >= textures.size()) {
            continue; 
        }
        float xpos = position.x + i*(button_size + 20); 
        Option opt(type, textures[textureIdx], {xpos,position.y});
        options.push_back(opt);
    }
}
void TankEvolutionUI::update(MyTank &mytank){
    bool shouldBeVisible = mytank.canEvolve();
    // std::cout << "evolutionUI.update() - shouldBeVisible: " << shouldBeVisible << " | visible: " << visible << std::endl;
    if (shouldBeVisible && !visible) {
        // std::cout << "Affichage de l'UI d'évolution!" << std::endl;
        updateAvailableEvolutions(mytank.getTankType(), shouldBeVisible);
        // std::cout << "Nombre d'évolutions disponibles: " << availableEvolutions.size() << std::endl;
        updateOptions();
        // std::cout << "Nombre d'options créées: " << options.size() << std::endl;
    }
    visible = shouldBeVisible;
}
void TankEvolutionUI::draw(sf::RenderWindow &window) {
    if (!visible) return;
    for (int i = 0; i < options.size(); i++){
        options[i].draw(window);
    }
}
TankType TankEvolutionUI::getSelectedEvolution(sf::Vector2i mousePos) {
    if (!visible) return currentTankType;
    for (int i = 0; i < options.size(); i++) {
        if (options[i].checkButtonClick(mousePos)) {
            return options[i].getType();
        }
    }
    return currentTankType; // trả về currentTankType nếu ko chọn j
}