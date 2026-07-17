#include "BagUI.h"
#include "../core/GameManager.h"
#include "../core/Player.h"
#include "../core/Bag.h"
#include "../core/Item.h"
#include <sstream>

BagUI::BagUI() 
    : UIBase(1000, 640, 100, 40), 
      m_tipMsg(L""),
      m_showAll(true)
{}

void BagUI::render(sf::RenderWindow& win, sf::Font& font, void* playerData)
{
    Player* p = static_cast<Player*>(playerData);
    Bag* bag = p->getBag();
    win.draw(m_bgPanel);
    clearButtons();

    // ========== 底部固定按钮 ==========
    float baseBtnW = 140.f;
    float baseBtnH = 45.f;
    float gap = 30.f;
    float baseX = 320.f;
    float bottomY = 560.f;
    addButton(baseX, bottomY, baseBtnW, baseBtnH, L"返回主菜单", 999, font);
    addButton(baseX + baseBtnW + gap, bottomY, baseBtnW, baseBtnH, L"使用物品", 998, font);

    // 文本基础坐标
    float textStartX = 130.f;
    float lineStartY = 70.f;
    float lineStep = 32.f; // 每行间距
    float useBtnX = 720.f; // 每行使用按钮X坐标
    float useBtnW = 120.f;
    float useBtnH = 28.f;

    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(19);
    infoText.setFillColor(sf::Color::White);

    if (m_showAll)
    {
        // 模式1：显示全部物品，无行内使用按钮
        infoText.setString(bag->getAllText());
        infoText.setPosition(textStartX, lineStartY);
        win.draw(infoText);
    }
    else
    {
        // 模式2：仅可使用物品，每行右侧生成使用按钮
    std::vector<Item*> usableList;
    // 只筛选数量大于0的物品，×0的直接跳过不显示
    for (auto i : bag->getByType(Item::FOOD))
    {
        if (i->getQuantity() > 0)
            usableList.push_back(i);
    }
    for (auto i : bag->getByType(Item::WEAPON))
    {
        if (i->getQuantity() > 0)
            usableList.push_back(i);
    }
    for (auto i : bag->getByType(Item::ARMOR))
    {
        if (i->getQuantity() > 0)
            usableList.push_back(i);
    }

        std::wstringstream ss;
        ss << L"====可使用物品（点击右侧按钮使用）====\n";
        infoText.setString(ss.str());
        infoText.setPosition(textStartX, lineStartY);
        win.draw(infoText);

        // 逐行绘制物品文字 + 动态添加使用按钮
        float curY = lineStartY + lineStep;
        for (int idx = 0; idx < usableList.size(); idx++)
        {
            Item* item = usableList[idx];
            // 绘制物品文字
            sf::Text lineTxt;
            lineTxt.setFont(font);
            lineTxt.setCharacterSize(19);
            lineTxt.setFillColor(sf::Color::White);
            lineTxt.setString(std::to_wstring(idx) + L". " + item->getDisplayText());
            lineTxt.setPosition(textStartX, curY);
            win.draw(lineTxt);

            // 为当前物品添加使用按钮，id用100+idx区分
            addButton(useBtnX, curY, useBtnW, useBtnH, L"使用", 100 + idx, font);
            curY += lineStep;
        }
    }

    // 底部提示文字
    sf::Text tipTxt;
    tipTxt.setFont(font);
    tipTxt.setFillColor(sf::Color::Yellow);
    tipTxt.setCharacterSize(20);
    tipTxt.setString(m_tipMsg);
    tipTxt.setPosition(130, 520);
    win.draw(tipTxt);

    // 绘制所有按钮（底部按钮+每行动态使用按钮）
    for (auto& btn : m_buttons)
    {
        win.draw(btn.rect);
        win.draw(btn.text);
    }
}

void BagUI::handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm)
{
    Player* p = gm->getPlayer();
    Bag* bag = p->getBag();

    // 鼠标悬浮更新按钮高亮
    if (e.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        updateButtonHover(mouse);
    }

    // 鼠标左键点击逻辑
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        std::vector<Item*> usableList;
    // 和渲染处保持一致，过滤数量为0物品
    for (auto i : bag->getByType(Item::FOOD))
    {
        if (i->getQuantity() > 0)
            usableList.push_back(i);
    }
    for (auto i : bag->getByType(Item::WEAPON))
    {
        if (i->getQuantity() > 0)
            usableList.push_back(i);
    }
    for (auto i : bag->getByType(Item::ARMOR))
    {
        if (i->getQuantity() > 0)
            usableList.push_back(i);
    }

        for (auto& btn : m_buttons)
        {
            if (!btn.isMouseOver(mouse)) continue;

            // 底部：返回主菜单
            if (btn.btnId == 999)
            {
                gm->setState(GameState::MAIN_MENU);
                m_tipMsg.clear();
                m_showAll = true; // 下次打开恢复全部视图
                break;
            }
            // 底部：切换至仅可用物品模式
            else if (btn.btnId == 998)
            {
                m_showAll = false;
                m_tipMsg = L"点击物品右侧【使用】按钮执行操作";
                break;
            }
            // 每行物品右侧的使用按钮：id=100+下标
            else if (btn.btnId >= 100)
            {
                int idx = btn.btnId - 100;
                if (idx < 0 || idx >= (int)usableList.size())
                {
                    m_tipMsg = L"物品无效";
                    break;
                }
                Item* target = usableList[idx];
               // 新增装备判断逻辑
                Weapon* wCheck = dynamic_cast<Weapon*>(target);
                Armor* aCheck = dynamic_cast<Armor*>(target);
                if (wCheck)
                {
                    if (p->getEquipWep() == wCheck)
                    {
                        p->unequipWeapon();
                        m_tipMsg = L"脱下武器，已放回背包";
                    }
                    else
                    {
                        target->use(p);
                        m_tipMsg = L"穿戴武器成功";
                    }
                }
                else if (aCheck)
                {
                    if (p->getEquipArm() == aCheck)
                    {
                        p->unequipArmor();
                        m_tipMsg = L"脱下防具，已放回背包";
                    }
                    else
                    {
                        target->use(p);
                        m_tipMsg = L"穿戴防具成功";
                    }
                }
                else
                {
                    target->use(p);
                    m_tipMsg = L"使用成功";
                }
                break;
            }
        }
    }
}