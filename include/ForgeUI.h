#ifndef FORGE_UI_H
#define FORGE_UI_H
#include "UIBase.h"
#include <vector>
class ForgeManager;
class Item;
class ForgeUI : public UIBase
{
private:
    int m_curOp; // 0=无操作 1=强化模式 2=合成模式
    std::wstring m_tipMsg;
    std::vector<Item*> m_enhList; // 缓存可强化装备列表
public:
    ForgeUI();
    void render(sf::RenderWindow& win, sf::Font& font, void* playerData) override;
    void handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm) override;
};
#endif