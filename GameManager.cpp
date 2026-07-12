#include "GameManager.h"
#include "Player.h"
#include "Bag.h"
#include "Task.h"
#include "Shop.h"
#include "BattleSystem.h"
#include "ForgeManager.h"
#include "Item.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>

GameManager::GameManager() : m_player(nullptr), m_state(GameState::MAIN_MENU), m_isRunning(true) 
{
    std::cout << "              校园RPG冒险游戏                     \n";
    // 尝试加载存档
    if (loadFromFile(getSaveFilePath())) 
    {
        std::cout << " 读取存档成功！欢迎回来，" << m_player->getName() << "！\n";
    } 
    else 
    {
        // 没有存档，创建新游戏
        std::cout << "没有找到存档，创建新游戏...\n";
        initGame();
    }
}

GameManager::~GameManager() 
{
    // 自动保存
    if (m_player) 
    {
        saveToFile(getSaveFilePath());
        std::cout << " 游戏已自动保存\n";
    }
    delete m_player;
    m_player = nullptr;
}

// 游戏初始化
void GameManager::initGame() 
{
    // 创建玩家
    std::string name;
    std::cout << "\n请输入你的角色名称：";
    std::cin >> name;                                                   ////////////////////////////
    m_player = new Player(name);
    // 给玩家一些初始物品
    Bag* bag = m_player->getBag();
    if (bag) 
    {
        bag->addItem(new Food("面包", 3, 10, 0, 0));      // HP+10
        bag->addItem(new Food("能量饼干", 2, 15, 0, 0)); // HP+15
        bag->addItem(new Medicine("回复药水", 3, 30));    // 恢复30HP
        bag->addItem(new Material("普通材料", 5, Material::COMMON));
    }
    std::cout << "\n 游戏初始化完成！\n";
    waitForEnter();
}

// 游戏主循环
void GameManager::run() 
{
    m_state = GameState::RUNNING;
    m_isRunning = true;
    while (m_isRunning) 
    {
        showMainMenu();

    }
    std::cout << "\n 感谢游玩！再见！\n";
}

// 主菜单显示
void GameManager::showMainMenu() 
{

    
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║                     主菜单                         ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";

    // 显示玩家状态
    if (m_player) 
    {
        std::cout << "   " << m_player->getName() 
                  << "  | Lv." << m_player->getLevel()
                  << "  |  " << m_player->getHP() << "/" << m_player->getMaxHP()
                  << "  |  " << m_player->getGold() << "金\n";
    }

    std::cout << "\n─────────────────────────────────────────────────────────\n";
    std::cout << "  1.  角色信息\n";
    std::cout << "  2.  背包\n";
    std::cout << "  3.  锻造工坊\n";
    std::cout << "  4.  商店\n";
    std::cout << "  5.  任务\n";
    std::cout << "  6.  战斗\n";
    std::cout << "  7.  存档\n";
    std::cout << "  8.  读档\n";
    std::cout << "  9.  退出游戏\n";
    std::cout << "─────────────────────────────────────────────────────────\n";
    std::cout << "请选择（输入数字）：";

    int choice;
    std::cin >> choice;                     ////////////////////////////////////////////////////

    // 处理输入错误
    if (std::cin.fail()) 
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << " 请输入有效数字！\n";
        waitForEnter();
        return;
    }

    // 处理选择
    switch (choice) 
    {
        case 1:
            viewPlayerInfo();
            break;
        case 2:
            viewBag();
            break;
        case 3:
            enterForge();
            break;
        case 4:
            enterShop();
            break;
        case 5:
            enterTask();
            break;
        case 6:
            enterBattle();
            break;
        case 7:
            saveGame();
            break;
        case 8:
            loadGame();
            break;
        case 9:
            exitGame();
            break;
        default:
            std::cout << " 无效选择，请重新输入！\n";
            waitForEnter();
            break;
    }
}

// 各功能入口实现
//查看角色信息
void GameManager::viewPlayerInfo() 
{
    if (m_player) 
    {
        m_player->displayInfo();
    }
    waitForEnter();
}
//查看背包
void GameManager::viewBag() 
{
    if (!m_player) return;
    Bag* bag = m_player->getBag();
    if (!bag) return;
    bag->displayAll();
    // 提供使用物品的快捷功能
    if (bag->getItemCount() > 0) 
    {
        std::cout << "\n─────────────────────────────────────────────────────────\n";
        std::cout << "是否要使用物品？(y/n)：";
        char choice;
        std::cin >> choice;                             /////////////////////////////////////////////
        if (choice == 'y' || choice == 'Y') 
        {
            std::cout << "请输入要使用的物品名称：";
            std::string itemName;
            std::cin >> itemName;
            bag->useItem(itemName, m_player); 
            std::cout << "\n更新后的背包：\n";
            bag->displayAll();
        }
    }
    waitForEnter();
}
//进入锻造工坊
void GameManager::enterForge() 
{
    if (!m_player) return;
    ForgeManager* forge = m_player->getForgeManager();
    if (forge) 
    {
        forge->showMenu();
    }
}
//进入商店
void GameManager::enterShop() 
{
    if (!m_player) return;
    Shop* shop = m_player->getShop();
    if (shop) 
    {
        shop->showMenu();
    }
}
//进入任务面板
void GameManager::enterTask() 
{
    if (!m_player) return;
    TaskManager* taskMgr = m_player->getTaskManager();
    if (!taskMgr) 
    {
        std::cout << "任务系统未初始化！\n";
        waitForEnter();
        return;
    }
    taskMgr->showTaskList();
    std::cout << "\n─────────────────────────────────────────────────────────\n";
    std::cout << "请选择操作：\n";
    std::cout << "  1. 查看任务详情\n";
    std::cout << "  2. 接取任务\n";
    std::cout << "  3. 领取奖励\n";
    std::cout << "  0. 返回\n";
    std::cout << "请选择：";

    int choice;
    std::cin >> choice;                                 //////////////////////////////////
    switch (choice) 
    {
        case 1: 
        {
            std::cout << "请输入任务ID：";
            int taskId;
            std::cin >> taskId;                     //////////////////////////
            taskMgr->showTaskDetail(taskId);
            break;
        }
        case 2: 
        {
            std::cout << "请输入任务ID：";
            int taskId;
            std::cin >> taskId;                 ///////////////////////////
            taskMgr->acceptTask(taskId);
            break;
        }
        case 3:     
        {
            std::cout << "请输入任务ID：";
            int taskId;
            std::cin >> taskId;             /////////////////////
            taskMgr->claimReward(taskId);
            break;
        }
        case 0:
        default:
            break;
    }
    waitForEnter();
}
//进入战斗系统
void GameManager::enterBattle() 
{
    if (!m_player) return;
    // 检查玩家是否存活
    if (m_player->getHP() <= 0) 
    {
        std::cout << " 你的生命值为0！\n";
        std::cout << "   请使用药品恢复，或重新读档。\n";
        waitForEnter();
        return;
    }
    BattleSystem* battle = new BattleSystem(m_player);
    battle->showMenu();
    delete battle;
}
// 存档/读档
//保存游戏
void GameManager::saveGame() 
{

    if (!m_player) 
    {
        std::cout << " 没有玩家数据可保存！\n";
        waitForEnter();
        return;
    }
    std::cout << "\n 正在保存游戏...\n";
    if (saveToFile(getSaveFilePath())) 
    {
        std::cout << " 游戏保存成功！\n";
        std::cout << "   存档位置: " << getSaveFilePath() << "\n";
    } 
    else 
    {
        std::cout << " 游戏保存失败！\n";
    }
    waitForEnter();
}
//读取存档
void GameManager::loadGame() 
{
    std::cout << "\n 正在读取存档...\n";
    if (loadFromFile(getSaveFilePath())) 
    {
        std::cout << " 读取存档成功！\n";
        m_state = GameState::RUNNING;
    } 
    else
    {
        std::cout << " 读取存档失败！\n";
    }
    waitForEnter();
}

//退出游戏
void GameManager::exitGame() 
{
    if (confirmAction("确定要退出游戏吗？")) 
    {
        m_isRunning = false;
        std::cout << " 再见！\n";
    }
}
// 文件操作
//获取存档文件路径
std::string GameManager::getSaveFilePath() const 
{
    return "savegame.dat";
}
//保存游戏到文件
bool GameManager::saveToFile(const std::string& filename) 
{
    if (!m_player) return false;
    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) 
    {
        std::cout << " 无法打开文件 " << filename << " 进行写入！\n";
        return false;
    }
    try {

        // 1. 版本号
        file << "1.0\n";
        // 2. 玩家数据
        file << m_player->serialize() << "\n";
        // 3. 背包数据
        Bag* bag = m_player->getBag();
        if (bag) 
        {
            file << bag->serialize() << "\n";
        }

        // 4. 任务进度
        TaskManager* taskMgr = m_player->getTaskManager();
        if (taskMgr) 
        {
            file << taskMgr->serializeProgress() << "\n";
        }
        else 
        {
            file << "\n";
        }
        // 5. 商店数据
        Shop* shop = m_player->getShop();
        if (shop) 
        {
            file << shop->serialize() << "\n";
        } 
        else 
        {
            file << "0\n";
        }   
        file.close();
        return true;
    } 
    catch (const std::exception& e) 
    {
        std::cout << " 保存时发生错误：" << e.what() << "\n";
        file.close();
        return false;
    }
}
//从文件读取存档
bool GameManager::loadFromFile(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file.is_open()) 
    {
        return false;
    }
    try 
    {
        std::string line;
        
        // 1. 读取版本号
        std::getline(file, line);
        if (line.empty()) 
        {
            file.close();
            return false;
        }

        // 2. 读取玩家数据
        std::getline(file, line);
        if (line.empty()) 
        {
            file.close();
            return false;
        }

        // 如果没有玩家对象，创建
        if (!m_player) 
        {
            std::stringstream ss(line);
            std::string name;
            std::getline(ss, name, ',');
            m_player = new Player(name);
        }

        // 恢复玩家数据
        if (!m_player->deserialize(line)) 
        {
            file.close();
            return false;
        }

        // 3. 读取背包数据 
        std::getline(file, line);
        Bag* bag = m_player->getBag();
        if (bag && !line.empty() && line != "0") 
        {
            bag->deserialize(line);
        }

        // 4. 读取任务进度 
        std::getline(file, line);
        TaskManager* taskMgr = m_player->getTaskManager();
        if (taskMgr && !line.empty()) 
        {
            taskMgr->deserializeProgress(line);
        }

        // 5. 读取商店数据
        std::getline(file, line);
        Shop* shop = m_player->getShop();
        if (shop && !line.empty() && line != "0") 
        {
            shop->deserialize(line);
        }

        file.close();
        return true;
    } 
    catch (const std::exception& e) 
    {
        std::cout << " 读取存档时发生错误：" << e.what() << "\n";
        file.close();
        return false;
    }
}

// 辅助方法
//等待用户按回车
void GameManager::waitForEnter() const 
{
    std::cout << "\n按回车键继续...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

//确认操作
bool GameManager::confirmAction(const std::string& message) const 
{
    std::cout << message << " (y/n)：";
    char choice;
    std::cin >> choice;
    return (choice == 'y' || choice == 'Y');
}

// Getter / Setter
GameState GameManager::getState() const 
{
    return m_state;
}

void GameManager::setState(GameState state) 
{
    m_state = state;
}

Player* GameManager::getPlayer() const 
{
    return m_player;
}