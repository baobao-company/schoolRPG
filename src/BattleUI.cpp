#include "BattleUI.h"
#include "../core/BattleSystem.h"
#include "../core/Player.h"
#include "../core/GameManager.h"
#include "../core/Bag.h"

BattleUI::BattleUI()
    : UIBase(1100, 680, 60, 20),
      m_bs(nullptr),
      m_battleStage(ST_IDLE),
      m_curEnemy(nullptr),
      m_battleMsg(L""),
      m_btnDirty(true)
{
    initEnemyItemList();
}

BattleUI::~BattleUI()
{
    if (m_bs != nullptr) delete m_bs;
}

// 新增：日志追加+自动截断，控制总行数不溢出屏幕
void BattleUI::appendLog(const std::wstring& line)
{
    // 拼接新行
    m_battleMsg += line + L"\n";
    std::wstringstream ss(m_battleMsg);
    std::vector<std::wstring> lines;
    std::wstring temp;
    // 分割所有行
    while (std::getline(ss, temp))
    {
        lines.push_back(temp);
    }
    // 超过最大行数，删除最旧的行
    while (lines.size() > MAX_LOG_LINE)
    {
        lines.erase(lines.begin());
    }
    // 重新拼接回m_battleMsg
    m_battleMsg.clear();
    for (auto& l : lines)
    {
        m_battleMsg += l + L"\n";
    }
}

void BattleUI::initEnemyItemList()
{
    m_enemyItemList.clear();
    std::vector<std::wstring> enemyTexts = {
        L"1.史莱姆 HP20 ATK3 EXP5 G2",
        L"2.野猫 HP15 ATK6 EXP8 G5",
        L"3.变异鱼 HP30 ATK8 EXP15 G3",
        L"4.大白鹅 HP45 ATK12 EXP25 G4",
        L"5.暗影校长 HP80 ATK18 EXP80 G6"
    };
    float startY = 90.f;
    float lineGap = 40.f;
    float btnX = 480.f;
    float btnW = 160.f;
    float btnH = 32.f;
    for(int i=0; i<5; i++)
    {
        EnemyItem item;
        item.idx = i;
        item.desc = enemyTexts[i];
        item.btnRect = sf::FloatRect(btnX, startY + i*lineGap, btnW, btnH);
        m_enemyItemList.push_back(item);
    }
}

void BattleUI::stepBattle(Player* p)
{
    if(m_battleStage == ST_IDLE || m_battleStage == ST_END || !p || !m_curEnemy || !m_curEnemy->isAlive())
        return;

    std::shared_ptr<Enemy> enemy = m_curEnemy;
    std::wstring log;

    if(m_battleStage == ST_ENEMY_TURN)
    {
        m_bs->enemyTurn(enemy.get(), log);
        appendLog(log); // 改用截断函数添加日志

        if(p->getHP() <= 0)
        {
            appendLog(L"=====================================");
            appendLog(L"【战斗失败】你被敌人击败！");
            appendLog(L"=====================================");
            m_battleStage = ST_END;
            m_curEnemy.reset();
            m_btnDirty = true;
            return;
        }
        m_battleStage = ST_PLAYER_TURN;
        m_btnDirty = true;
    }
}

void BattleUI::render(sf::RenderWindow& win, sf::Font& font, void* playerData)
{
    win.clear(sf::Color(15,15,25));
    Player* p = static_cast<Player*>(playerData);
    win.draw(m_bgPanel);

    if(m_btnDirty)
    {
        clearButtons();
        float backW = 180.f;
        float backH = 50.f;
        float backX = (1280 - backW)/2.f;
        float bottomY = 600.f;
        addButton(backX, bottomY, backW, backH, L"返回", 99, font);

        if(m_battleStage == ST_PLAYER_TURN || m_battleStage == ST_ENEMY_TURN)
        {
            float btnW = 200.f;
            float btnH = 50.f;
            float gap = 40.f;
            float startX = (1280 - btnW*2 - gap)/2.f;
            addButton(startX, bottomY - 60, btnW, btnH, L"攻击", 1, font);
            addButton(startX + btnW + gap, bottomY - 60, btnW, btnH, L"喝药", 2, font);
        }
        m_btnDirty = false;
    }

    if(m_battleStage == ST_IDLE)
    {
        sf::Text title;
        title.setFont(font);
        title.setFillColor(sf::Color::White);
        title.setCharacterSize(22);
        title.setString(L"====敌人列表====");
        title.setPosition(60, 50);
        win.draw(title);

        for(auto& item : m_enemyItemList)
        {
            sf::Text txt;
            txt.setFont(font);
            txt.setFillColor(sf::Color::White);
            txt.setCharacterSize(18);
            txt.setString(item.desc);
            txt.setPosition(60, item.btnRect.top);
            win.draw(txt);

            sf::RectangleShape btnRect(sf::Vector2f(item.btnRect.width, item.btnRect.height));
            btnRect.setPosition(item.btnRect.left, item.btnRect.top);
            btnRect.setFillColor(sf::Color(40,70,130));
            btnRect.setOutlineColor(sf::Color::White);
            btnRect.setOutlineThickness(1.f);
            win.draw(btnRect);

            sf::Text btnTxt;
            btnTxt.setFont(font);
            btnTxt.setFillColor(sf::Color::White);
            btnTxt.setCharacterSize(16);
            btnTxt.setString(L"开始对战");
            btnTxt.setPosition(item.btnRect.left + 20, item.btnRect.top + 4);
            win.draw(btnTxt);
        }
    }
    else if(m_battleStage == ST_PLAYER_TURN || m_battleStage == ST_ENEMY_TURN || m_battleStage == ST_END)
    {
        sf::Text fightTitle;
        fightTitle.setFont(font);
        fightTitle.setFillColor(sf::Color::Cyan);
        fightTitle.setCharacterSize(22);
        if(m_battleStage == ST_END)
            fightTitle.setString(L"战斗结算（点击返回退出）");
        else
            fightTitle.setString(L"战斗中");
        fightTitle.setPosition(60, 50);
        win.draw(fightTitle);

        // 血量条固定独立区域，永远不会被日志挤掉
        if(m_curEnemy != nullptr)
        {
            std::wstring hpInfo = L"【玩家HP：" + std::to_wstring(p->getHP())
                + L"/" + std::to_wstring(p->getMaxHP())
                + L" | " + m_curEnemy->getName()
                + L" HP：" + std::to_wstring(m_curEnemy->getHP())
                + L"/" + std::to_wstring(m_curEnemy->getMaxHP()) + L"】";
            sf::Text hpTxt;
            hpTxt.setFont(font);
            hpTxt.setFillColor(sf::Color::Yellow);
            hpTxt.setCharacterSize(19);
            hpTxt.setString(hpInfo);
            hpTxt.setPosition(60, 80);
            win.draw(hpTxt);
        }

        // 日志固定在血量条下方，行数被限制不会出界
        sf::Text logTxt;
        logTxt.setFont(font);
        logTxt.setFillColor(sf::Color::White);
        logTxt.setCharacterSize(17);
        logTxt.setString(m_battleMsg);
        logTxt.setPosition(60, 115);
        win.draw(logTxt);

        // 开局只添加一次速度信息
        if(m_curEnemy != nullptr && m_battleMsg.find(L"玩家速度:") == std::wstring::npos)
        {
            int pSpd = p->getSpeed();
            int eSpd = m_curEnemy->getSpeed();
            appendLog(L"玩家速度:" + std::to_wstring(pSpd) + L" 敌人速度:" + std::to_wstring(eSpd));
            m_battleStage = (pSpd >= eSpd) ? ST_PLAYER_TURN : ST_ENEMY_TURN;
            m_btnDirty = true;
        }
    }

    for (auto& btn : m_buttons)
    {
        win.draw(btn.rect);
        win.draw(btn.text);
    }

    stepBattle(p);
}

void BattleUI::handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm)
{
    if (e.type == sf::Event::MouseMoved)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        updateButtonHover(mouse);
    }

    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f mouse = win.mapPixelToCoords(sf::Mouse::getPosition(win));
        Player* player = gm->getPlayer();
        if(!player)
        {
            m_battleMsg = L"玩家数据为空，无法战斗";
            return;
        }

        if(m_battleStage == ST_END)
        {
            for (auto& btn : m_buttons)
            {
                if (btn.isMouseOver(mouse) && btn.btnId == 99)
                {
                    gm->setState(GameState::MAIN_MENU);
                    m_curEnemy.reset();
                    m_battleMsg.clear();
                    m_battleStage = ST_IDLE;
                    m_btnDirty = true;
                    if(m_bs)
                    {
                        delete m_bs;
                        m_bs = nullptr;
                    }
                    return;
                }
            }
            return;
        }

        if(m_battleStage == ST_IDLE)
        {
            for(auto& item : m_enemyItemList)
            {
                if(item.btnRect.contains(mouse))
                {
                    int idx = item.idx;
                    if(idx <0 || idx >=5)
                    {
                        m_battleMsg = L"敌人编号非法";
                        return;
                    }
                    if(m_bs == nullptr)
                    {
                        m_bs = new BattleSystem(player);
                    }
                    std::shared_ptr<Enemy> tempEnemy = m_bs->createEnemy((EnemyType)idx);
                    if(!tempEnemy)
                    {
                        m_battleMsg = L"创建敌人失败";
                        return;
                    }
                    m_curEnemy = tempEnemy;
                    m_battleMsg.clear();
                    appendLog(L"遭遇：" + m_curEnemy->getName()); // 改用截断函数
                    m_battleStage = ST_PLAYER_TURN;
                    m_btnDirty = true;
                    return;
                }
            }

            for (auto& btn : m_buttons)
            {
                if (btn.isMouseOver(mouse) && btn.btnId == 99)
                {
                    gm->setState(GameState::MAIN_MENU);
                    m_curEnemy.reset();
                    m_battleMsg.clear();
                    m_battleStage = ST_IDLE;
                    m_btnDirty = true;
                    if(m_bs)
                    {
                        delete m_bs;
                        m_bs = nullptr;
                    }
                    return;
                }
            }
        }
        else if(m_battleStage == ST_PLAYER_TURN)
        {
            if(!m_curEnemy)
            {
                m_battleMsg = L"当前无战斗目标";
                return;
            }
            Bag* bag = player->getBag();
            if(!bag)
            {
                m_battleMsg = L"背包数据异常";
                return;
            }

            std::shared_ptr<Enemy> enemy = m_curEnemy;
            for (auto& btn : m_buttons)
            {
                if (!btn.isMouseOver(mouse)) continue;
                std::wstring log;

                if(btn.btnId == 1) // 攻击
                {
                    m_bs->playerTurn(enemy.get(), log);
                    appendLog(log); // 统一截断添加

                    if(!enemy->isAlive())
                    {
                        std::wstring rewardLog;
                        m_bs->grantRewards(enemy.get(), rewardLog);

                        appendLog(L"=====================================");
                        appendLog(L"【战斗胜利】");
                        appendLog(rewardLog);
                        appendLog(L"=====================================");

                        m_battleStage = ST_END;
                        m_curEnemy.reset();
                        m_btnDirty = true;
                        return;
                    }
                    m_battleStage = ST_ENEMY_TURN;
                    m_btnDirty = true;
                }
                else if(btn.btnId == 2) // 喝药
                {
                    bool useOk = bag->useItem(L"回复药水", player);
                    if(useOk)
                    {
                        appendLog(L"使用回复药水，恢复生命值！");
                        m_battleStage = ST_ENEMY_TURN;
                        m_btnDirty = true;
                    }
                    else
                    {
                        appendLog(L"背包无回复药水，使用失败！");
                    }
                }
                break;
            }
        }
    }
}