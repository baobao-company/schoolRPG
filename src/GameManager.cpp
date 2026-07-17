#include "GameManager.h"
#include "Player.h"
#include "Bag.h"
#include "BattleSystem.h"
#include "ForgeManager.h"
#include "Shop.h"
#include "Task.h"
#include "Item.h"
#include "../ui/MainMenuUI.h"
#include "../ui/BagUI.h"
#include "../ui/ForgeUI.h"
#include "../ui/ShopUI.h"
#include "../ui/TaskUI.h"
#include "../ui/BattleUI.h"
#include "../ui/PlayerInfoUI.h"
#include <fstream>
#include <sstream>
GameManager::GameManager() : m_player(nullptr), m_state(GameState::MAIN_MENU), m_runFlag(true)
{
    uiMain = std::make_unique<MainMenuUI>();
    uiPlayer = std::make_unique<PlayerInfoUI>();
    uiBag = std::make_unique<BagUI>();
    uiForge = std::make_unique<ForgeUI>();
    uiShop = std::make_unique<ShopUI>();
    uiTask = std::make_unique<TaskUI>();
    // 旧代码：uiBattle.reset(new BattleUI(m_player));
    // 替换为无参构造
    uiBattle.reset(new BattleUI());
    if (!loadFromFile("savegame.dat"))
        initGameNew();
}
GameManager::~GameManager()
{
    if (m_player != nullptr)
    {
        try
        {
            saveToFile("savegame.dat");
        }
        catch (...) {}
    }
    delete m_player;
    m_player = nullptr;
}
void GameManager::initGameNew()
{
    std::wstring name = L"玩家";
    m_player = new Player(name);
    Bag* b = m_player->getBag();
    b->addItem(new Food(L"面包",3,10,0,0));
    b->addItem(new Food(L"能量饼干",2,15,0,0));
    b->addItem(new Medicine(L"回复药水",3,30));
    b->addItem(new Material(L"普通材料",5,Material::COMMON));
}
void GameManager::exitGameLogic()
{
    m_runFlag = false;
}
void GameManager::setState(GameState s) { m_state = s; }
GameState GameManager::getState() const { return m_state; }
Player* GameManager::getPlayer() const { return m_player; }
void GameManager::handleSFEvent(sf::Event& ev, sf::RenderWindow& win)
{
    switch(m_state)
    {
        case GameState::MAIN_MENU:
            uiMain->handleEvent(ev, win, this); break;
        case GameState::VIEW_PLAYER:
            uiPlayer->handleEvent(ev, win, this); break;
        case GameState::VIEWING_BAG:
            uiBag->handleEvent(ev, win, this); break;
        case GameState::IN_FORGE:
            uiForge->handleEvent(ev, win, this); break;
        case GameState::IN_SHOP:
            uiShop->handleEvent(ev, win, this); break;
        case GameState::IN_TASK:
            uiTask->handleEvent(ev, win, this); break;
        case GameState::IN_BATTLE:
            uiBattle->handleEvent(ev, win, this); break;
        default: break;
    }
}
void GameManager::renderAll(sf::RenderWindow& win, sf::Font& font)
{
    switch(m_state)
    {
        case GameState::MAIN_MENU:
            uiMain->render(win, font, m_player); break;
        case GameState::VIEW_PLAYER:
            uiPlayer->render(win, font, m_player); break;
        case GameState::VIEWING_BAG:
            uiBag->render(win, font, m_player); break;
        case GameState::IN_FORGE:
            uiForge->render(win, font, m_player); break;
        case GameState::IN_SHOP:
            uiShop->render(win, font, m_player); break;
        case GameState::IN_TASK:
            uiTask->render(win, font, m_player); break;
        case GameState::IN_BATTLE:
            uiBattle->render(win, font, m_player); break;
        default: break;
    }
    // ==================== 新增：战斗外自动回血 ====================
    if(m_player == nullptr) return;
    // 计时冷却到达
    if(m_healTimer.getElapsedTime().asSeconds() >= HEAL_CD)
    {
        // 当前不在战斗界面才回血
        if(m_state != GameState::IN_BATTLE)
        {
            int nowHp = m_player->getHP();
            int maxHp = m_player->getMaxHP();
            if(nowHp < maxHp)
            {
                m_player->healHP(HEAL_NUM);
            }
        }
        m_healTimer.restart();
    }
    // ==============================================================
}
void GameManager::openPlayerInfo() { setState(GameState::VIEW_PLAYER); }
void GameManager::openBag() { setState(GameState::VIEWING_BAG); }
void GameManager::openForge() { setState(GameState::IN_FORGE); }
void GameManager::openShop() { setState(GameState::IN_SHOP); }
void GameManager::openTask() { setState(GameState::IN_TASK); }
void GameManager::openBattle() { setState(GameState::IN_BATTLE); }
void GameManager::doSave()
{
    saveToFile("savegame.dat");
}
void GameManager::doLoad()
{
    loadFromFile("savegame.dat");
}
bool GameManager::saveToFile(const std::string& path)
{
    try
    {
        if (m_player == nullptr)
            return false;

        std::ofstream f(path, std::ios::trunc);
        if (!f.is_open())
            return false;

        // 固定5行完整存档，一行不能少
        f << "1.0\n";
        f << m_player->serialize() << "\n";
        f << m_player->getBag()->serialize() << "\n";
        f << m_player->getTaskManager()->serialize() << "\n";
        f << m_player->getShop()->serialize() << "\n";

        f.flush();
        f.close();
        return true;
    }
    catch (...)
    {
        return false;
    }
}
bool GameManager::loadFromFile(const std::string& path)
{
    try
    {
        std::ifstream f(path);
        if (!f.is_open())
            return false;

        // 旧玩家彻底释放，杜绝内存泄漏
        if (m_player != nullptr)
        {
            delete m_player;
            m_player = nullptr;
        }
        m_player = new Player(L"玩家");

        std::string line;
        // 版本号
        if (!std::getline(f, line)) throw 1;
        // 玩家数据
        if (!std::getline(f, line) || !m_player->deserialize(line)) throw 2;
        // 背包
        if (!std::getline(f, line) || !m_player->getBag()->deserialize(line)) throw 3;
        // 任务
        if (!std::getline(f, line)) throw 4;
        m_player->getTaskManager()->deserialize(line);
        // 商店
        if (!std::getline(f, line)) throw 5;
        m_player->getShop()->deserialize(line);

        f.close();
        return true;
    }
    catch (...)
    {
        // 读取损坏，清空半成品玩家
        if (m_player != nullptr)
        {
            delete m_player;
            m_player = nullptr;
        }
        return false;
    }
}
