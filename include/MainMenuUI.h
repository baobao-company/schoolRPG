#ifndef MAIN_MENU_UI_H
#define MAIN_MENU_UI_H
#include "UIBase.h"
class MainMenuUI : public UIBase
{
private:
    sf::Text m_titleText;
public:
    MainMenuUI();
    void render(sf::RenderWindow& win, sf::Font& font, void* playerData) override;
    void handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm) override;
};
#endif