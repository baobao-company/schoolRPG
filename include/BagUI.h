#ifndef BAG_UI_H
#define BAG_UI_H
#include "UIBase.h"
class BagUI : public UIBase
{
private:
    std::wstring m_tipMsg;
    bool m_showAll; // true=全部物品，false=仅可使用物品+每行使用按钮
public:
    BagUI();
    void render(sf::RenderWindow& win, sf::Font& font, void* playerData) override;
    void handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm) override;
};
#endif