#ifndef PLAYERINFO_UI_H
#define PLAYERINFO_UI_H
#include "UIBase.h"
class PlayerInfoUI : public UIBase
{
public:
    PlayerInfoUI();
    void render(sf::RenderWindow& win, sf::Font& font, void* playerData) override;
    void handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm) override;
};
#endif