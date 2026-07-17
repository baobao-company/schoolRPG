#include "ShopUI.h"
#include "../core/GameManager.h"
#include "../core/Player.h"
#include "../core/Shop.h"
#include "../core/Bag.h"

ShopUI::ShopUI() : UIBase(1060, 640, 70, 40), m_opMode(0), m_tipMsg(L"")
{}

void ShopUI::render(sf::RenderWindow& win, sf::Font& font, void* playerData)
{
    Player* p = static_cast<Player*>(playerData);
    Shop* shop = p->getShop();
    shop->refreshUnlock();

    win.draw(m_bgPanel);
    clearButtons();

    // 底部全局按钮（已修复参数，补全btnH）
    float btnW = 220.f;
    float btnH = 50.f;
    float gap = 30.f;
    float winW = 1280.f;
    float totalWidth = btnW * 3 + gap * 2;
    float startX = (winW - totalWidth) / 2.f;
    float bottomY = 600.f;
    addButton(startX, bottomY, btnW, btnH, L"返回", 1, font);
    addButton(startX + btnW + gap, bottomY, btnW, btnH, L"购买商品", 2, font);
    addButton(startX + (btnW + gap) * 2, bottomY, btnW, btnH, L"出售物品", 3, font);

    // 顶部金币文字
    sf::Text goldTxt;
    goldTxt.setFont(font);
    goldTxt.setCharacterSize(20);
    goldTxt.setFillColor(sf::Color::Yellow);
    goldTxt.setString(L"当前金币：" + std::to_wstring(p->getGold()));
    goldTxt.setPosition(110, 40);
    win.draw(goldTxt);

    // 购买模式：商品ID固定1~9
    // 购买模式：商品ID固定1~9
if (m_opMode == 1)
{
    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(19);
    title.setFillColor(sf::Color::White);
    title.setString(L"====商品列表（右侧点击购买）====");
    title.setPosition(110, 90);
    win.draw(title);

    float curY = 130.f;
    float btnX = 780.f;
    float btnW = 110.f;
    float btnH = 30.f;
    float lineGap = 42.f;
    std::vector<int> goodsIdList = {1,2,3,4,5,6,7,8,9};

    for (int id : goodsIdList)
    {
        auto [name, price, stock, unlocked] = shop->getShopGoodInfo(id);
        std::wstring lineText = L"ID:" + std::to_wstring(id) + L" " + name + L" | 价格:" + std::to_wstring(price);
        if (stock != -1)
            lineText += L" 库存:" + std::to_wstring(stock);
        if (!unlocked)
            lineText += L" [未解锁]";

        sf::Text lineTxt;
        lineTxt.setFont(font);
        lineTxt.setCharacterSize(18);
        lineTxt.setFillColor(sf::Color::White);
        lineTxt.setString(lineText);
        lineTxt.setPosition(110, curY);
        win.draw(lineTxt);
        addButton(btnX, curY, btnW, btnH, L"购买", 100 + id, font);
        curY += lineGap;
    }
}
    // 出售模式
    else if (m_opMode == 2)
    {
        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(19);
        title.setFillColor(sf::Color::White);
        title.setString(L"====背包可出售物品（右侧出售）====");
        title.setPosition(110, 90);
        win.draw(title);

        Bag* bag = p->getBag();
        std::vector<Item*> sellList;
        auto foods = bag->getByType(Item::FOOD);
        auto weps = bag->getWeapons();
        auto arms = bag->getArmors();
        auto meds = bag->getByType(Item::MEDICINE);
        auto mats = bag->getMaterials();
        for (auto i : foods) sellList.push_back(i);
        for (auto i : weps) sellList.push_back(i);
        for (auto i : arms) sellList.push_back(i);
        for (auto i : meds) sellList.push_back(i);
        for (auto i : mats) sellList.push_back(i);

        float curY = 130.f;
        float btnX = 780.f;
        float btnW = 110.f;
        float btnH = 30.f;
        float lineGap = 42.f;

        for (int i = 0; i < sellList.size(); i++)
        {
            Item* it = sellList[i];
            sf::Text lineTxt;
            lineTxt.setFont(font);
            lineTxt.setCharacterSize(18);
            lineTxt.setFillColor(sf::Color::White);
            lineTxt.setString(std::to_wstring(i) + L". " + it->getDisplayText());
            lineTxt.setPosition(110, curY);
            win.draw(lineTxt);
            addButton(btnX, curY, btnW, btnH, L"出售", 200 + i, font);
            curY += lineGap;
        }
    }

    // 底部提示文字
    sf::Text tipTxt;
    tipTxt.setFont(font);
    tipTxt.setCharacterSize(20);
    tipTxt.setFillColor(sf::Color::Yellow);
    tipTxt.setString(m_tipMsg);
    tipTxt.setPosition(110, 540);
    win.draw(tipTxt);

    // 绘制所有按钮
    for (auto& btn : m_buttons)
    {
        win.draw(btn.rect);
        win.draw(btn.text);
    }
}

void ShopUI::handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm)
{
    if (e.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        updateButtonHover(mouse);
    }
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        Player* p = gm->getPlayer();
        Shop* shop = p->getShop();
        Bag* bag = p->getBag();

        for (auto& btn : m_buttons)
        {
            if (!btn.isMouseOver(mouse)) continue;
            // 返回主菜单
            if (btn.btnId == 1)
            {
                gm->setState(GameState::MAIN_MENU);
                m_opMode = 0;
                m_tipMsg = L"";
                break;
            }
            // 切换购买界面
            else if (btn.btnId == 2)
            {
                m_opMode = 1;
                m_tipMsg = L"点击右侧购买按钮购入商品";
                break;
            }
            // 切换出售界面
            else if (btn.btnId == 3)
            {
                m_opMode = 2;
                m_tipMsg = L"点击出售卖出该物品全部数量";
                break;
            }
            // 购买按钮 100~109
            else if (btn.btnId >= 100 && btn.btnId <= 109)
            {
                int goodsId = btn.btnId - 100;
                m_tipMsg = shop->buy(goodsId);
                break;
            }
            // 出售按钮 200+
            else if (btn.btnId >= 200)
            {
                std::vector<Item*> sellList;
                auto foods = bag->getByType(Item::FOOD);
                auto weps = bag->getWeapons();
                auto arms = bag->getArmors();
                auto meds = bag->getByType(Item::MEDICINE);
                auto mats = bag->getMaterials();
                for (auto i : foods) sellList.push_back(i);
                for (auto i : weps) sellList.push_back(i);
                for (auto i : arms) sellList.push_back(i);
                for (auto i : meds) sellList.push_back(i);
                for (auto i : mats) sellList.push_back(i);

                int idx = btn.btnId - 200;
                if (idx < 0 || idx >= (int)sellList.size())
                {
                    m_tipMsg = L"物品不存在";
                    break;
                }
                Item* target = sellList[idx];
                m_tipMsg = shop->sellItem(target->getName(), target->getQuantity());
                break;
            }
        }
    }
}