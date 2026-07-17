#include "ForgeUI.h"
#include "../core/GameManager.h"
#include "../core/Player.h"
#include "../core/ForgeManager.h"
#include "../core/Item.h"

ForgeUI::ForgeUI() : UIBase(1040, 660, 80, 30), m_curOp(0), m_tipMsg(L"")
{
    // 移除弹窗对象，不再引入UIPopupInput
}

void ForgeUI::render(sf::RenderWindow& win, sf::Font& font, void* playerData)
{
    win.clear(sf::Color(15,15,25));
    Player* p = static_cast<Player*>(playerData);
    ForgeManager* forge = p->getForge();
    Bag* bag = p->getBag();
    win.draw(m_bgPanel);
    clearButtons();

    // 左上：材料库存文字
    sf::Text matText;
    matText.setFont(font);
    matText.setCharacterSize(18);
    matText.setFillColor(sf::Color::White);
    matText.setString(forge->getMaterialText());
    matText.setPosition(120, 70);
    win.draw(matText);

    // 右上：强化规则
    sf::Text ruleText;
    ruleText.setFont(font);
    ruleText.setCharacterSize(16);
    ruleText.setFillColor(sf::Color(180,180,180));
    ruleText.setString(forge->getRuleText());
    ruleText.setPosition(420, 70);
    win.draw(ruleText);

    // 底部全局功能按钮
    float btnW = 180.f;
    float btnH = 45.f;
    float gap = 30.f;
    float bottomY = 600.f;
    addButton(300, bottomY, btnW, btnH, L"强化装备", 1, font);
    addButton(500, bottomY, btnW, btnH, L"材料合成", 2, font);
    addButton(700, bottomY, 140, btnH, L"返回", 0, font);

    // 强化模式：展示所有可强化装备+每行强化按钮
    if (m_curOp == 1)
    {
        m_enhList = forge->getEquippableAll();
        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(19);
        title.setFillColor(sf::Color::White);
        title.setString(L"可强化装备(右侧点击强化)");
        title.setPosition(120, 280);
        win.draw(title);

        float lineY = 310.f;
        float opBtnX = 650.f;
        float opBtnW = 120.f;
        float opBtnH = 30.f;
        float lineGap = 35.f;

        for (int i = 0; i < m_enhList.size(); i++)
        {
            Item* item = m_enhList[i];
            std::wstring displayName = item->getName();

            // 标记身上穿戴装备
            Weapon* w = dynamic_cast<Weapon*>(item);
            Armor* a = dynamic_cast<Armor*>(item);
            if (w && p->getEquipWep() == w)
                displayName += L" 【已穿戴】";
            if (a && p->getEquipArm() == a)
                displayName += L" 【已穿戴】";

            // 绘制装备文字
            sf::Text lineTxt;
            lineTxt.setFont(font);
            lineTxt.setCharacterSize(18);
            lineTxt.setFillColor(sf::Color::White);
            lineTxt.setString(std::to_wstring(i) + L". " + displayName);
            lineTxt.setPosition(120, lineY);
            win.draw(lineTxt);

            // 每行右侧生成强化按钮 ID=100+i
            addButton(opBtnX, lineY, opBtnW, opBtnH, L"强化", 100 + i, font);
            lineY += lineGap;
        }
    }
    // 合成模式：0~3级合成快捷按钮
    else if (m_curOp == 2)
    {
        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(19);
        title.setFillColor(sf::Color::White);
        title.setString(L"材料合成（点击对应等级");
        title.setPosition(120, 280);
        win.draw(title);

        float lineY = 310.f;
        float startX = 180.f;
        float btnW = 90.f;
        float gap = 20.f;
        // 0、1、2、3四级合成按钮 ID=200~203
        for (int lv = 0; lv <= 3; lv++)
        {
            std::wstring btnTxt = L"合成" + std::to_wstring(lv);
            addButton(startX + lv * (btnW + gap), lineY, btnW, 35, btnTxt, 200 + lv, font);
        }
    }

    // 底部黄色提示文字
   sf::Text tipTxt;
    tipTxt.setFont(font);
    tipTxt.setCharacterSize(20);
    tipTxt.setFillColor(sf::Color::Yellow);
    tipTxt.setString(m_tipMsg); // 把 tip → tipTxt
    tipTxt.setPosition(100, 440);
    win.draw(tipTxt);

    // 绘制全部按钮
    for (auto& b : m_buttons)
    {
        win.draw(b.rect);
        win.draw(b.text);
    }
}

void ForgeUI::handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm)
{
    // 鼠标悬浮高亮
    if (e.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        updateButtonHover(mouse);
    }

    // 鼠标点击处理
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        Player* p = gm->getPlayer();
        ForgeManager* forge = p->getForge();

        for (auto& btn : m_buttons)
        {
            if (!btn.isMouseOver(mouse))
                continue;

            // 返回主菜单
            if (btn.btnId == 0)
            {
                gm->setState(GameState::MAIN_MENU);
                m_curOp = 0;
                m_tipMsg = L"";
                break;
            }
            // 切换强化列表模式
            else if (btn.btnId == 1)
            {
                m_curOp = 1;
                m_tipMsg = L"选择右侧【强化】按钮提升装备";
                break;
            }
            // 切换材料合成模式
            else if (btn.btnId == 2)
            {
                m_curOp = 2;
                m_tipMsg = L"点击下方等级按钮合成高级材料";
                break;
            }
            // 装备强化按钮 100 ~ 199
            else if (btn.btnId >= 100 && btn.btnId < 200)
            {
                int idx = btn.btnId - 100;
                if (idx < 0 || idx >= (int)m_enhList.size())
                {
                    m_tipMsg = L"装备序号无效";
                    break;
                }
                m_tipMsg = forge->doEnhance(m_enhList[idx]);
                break;
            }
            // 材料合成按钮 200 ~ 203
            else if (btn.btnId >= 200 && btn.btnId <= 203)
            {
                int lv = btn.btnId - 200;
                m_tipMsg = forge->craft(lv);
                break;
            }
        }
    }
}