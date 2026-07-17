#include "MainMenuUI.h"
#include "../core/GameManager.h"
#include "../core/Player.h"
MainMenuUI::MainMenuUI() : UIBase(1080, 620, 100, 50)
{
}
void MainMenuUI::render(sf::RenderWindow& win, sf::Font& font, void* playerData)
{
    Player* player = static_cast<Player*>(playerData);
    win.draw(m_bgPanel);
    m_titleText.setFont(font);
    m_titleText.setString(L"校园RPG冒险");
    m_titleText.setCharacterSize(46);
    m_titleText.setFillColor(sf::Color(220, 230, 255));
    m_titleText.setPosition(480, 80);
    win.draw(m_titleText);

    clearButtons();
float btnW = 240.f;
float btnH = 50.f;
float colGap = 120.f;   // 缩小两列中间空隙，不再空旷
float rowGap = 55.f;
float totalWidth = btnW * 2 + colGap;
float winWidth = 1280.f;
// 整体两列网格水平居中起始X
float startX = (winWidth - totalWidth) / 2.f;
float startY = 160.f;

// 左列
addButton(startX, startY, btnW, btnH, L"1.角色信息", 1, font);
addButton(startX, startY + rowGap, btnW, btnH, L"3.锻造工坊", 3, font);
addButton(startX, startY + rowGap*2, btnW, btnH, L"5.任务", 5, font);
addButton(startX, startY + rowGap*3, btnW, btnH, L"7.存档", 7, font);

// 右列
float rightX = startX + btnW + colGap;
addButton(rightX, startY, btnW, btnH, L"2.背包", 2, font);
addButton(rightX, startY + rowGap, btnW, btnH, L"4.商店", 4, font);
addButton(rightX, startY + rowGap*2, btnW, btnH, L"6.战斗", 6, font);
addButton(rightX, startY + rowGap*3, btnW, btnH, L"8.读档", 8, font);

// 退出按钮：和两列总宽度对齐居中，和左右按钮同宽
float exitX = (winWidth - btnW) / 2.f;
addButton(exitX, startY + rowGap*4, btnW, btnH, L"9.退出游戏", 9, font);
    // 绘制所有按钮
    for (auto& btn : m_buttons)
    {
        win.draw(btn.rect);
        win.draw(btn.text);
    }

    // 底部玩家状态栏
    if (player)
{
    sf::Text statText;
    statText.setFont(font);
    statText.setCharacterSize(20);
    statText.setFillColor(sf::Color::Cyan);
    std::wstring info = L"玩家：" + player->getName() 
+ L" | Lv." + std::to_wstring(player->getLevel()) 
+ L" | HP:" + std::to_wstring(player->getHP()) + L"/" + std::to_wstring(player->getMaxHP())
+ L" | 金币：" + std::to_wstring(player->getGold());
    statText.setString(info);
    // 文字水平居中，不再靠左
    float textWidth = statText.getLocalBounds().width;
    statText.setPosition((winWidth - textWidth)/2.f, startY + rowGap*5 + 25);
    win.draw(statText);
}
}
void MainMenuUI::handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm)
{
    if (e.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        updateButtonHover(mouse);
    }
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        for (auto& btn : m_buttons)
        {
            if (btn.isMouseOver(mouse))
            {
                switch (btn.btnId)
                {
                    case 1: gm->openPlayerInfo(); break;
                    case 2: gm->openBag(); break;
                    case 3: gm->openForge(); break;
                    case 4: gm->openShop(); break;
                    case 5: gm->openTask(); break;
                    case 6: gm->openBattle(); break;
                    case 7:
{
    // 先调用保存函数
    gm->doSave();

    break;
}
                    case 8: gm->doLoad(); break;
                    case 9: win.close(); break;
                }
                break;
            }
        }
    }
}