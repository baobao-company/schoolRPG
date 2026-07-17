#ifndef SHOP_UI_H
#define SHOP_UI_H
#include "UIBase.h"
#include <vector>
#include "../core/Shop.h"
class ShopUI : public UIBase
{
private:
    int m_opMode; // 0空闲 1购买 2出售
    std::wstring m_tipMsg;
    std::vector<ShopItem> m_shopCache;
public:
    ShopUI();
    void render(sf::RenderWindow& win, sf::Font& font, void* playerData) override;
    void handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm) override;
};
#endif