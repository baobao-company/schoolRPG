#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <string>
#include <memory>
#include <vector>

class Player;
class Bag;
class TaskManager;
class Shop;
class BattleSystem;
class ForgeManager;

// 游戏状态枚举
enum class GameState 
{
    MAIN_MENU,      // 主菜单
    RUNNING,        // 游戏中
    IN_BATTLE,      // 战斗中
    IN_SHOP,        // 在商店
    IN_FORGE,       // 在锻造工坊
    IN_TASK,        // 在任务面板
    VIEWING_BAG,    // 查看背包
    VIEWING_PLAYER, // 查看角色
    SAVING,         // 存档中
    LOADING,        // 读档中
    EXITING         // 退出
};

// 游戏管理器类 GameManager
class GameManager
{
    private:
    Player* m_player;                // 玩家对象
    GameState m_state;               // 当前游戏状态
    bool m_isRunning;                // 游戏是否运行中
    //私有方法
    void initGame();                 // 初始化游戏
    void exitGame();                 // 退出游戏
    void waitForEnter() const;       // 等待用户按回车
    std::string getSaveFilePath() const;  // 获取存档文件路径
    bool confirmAction(const std::string& message) const;  // 确认操作
    //存档数据格式相关
    std::string serializeAll() const;     // 序列化所有数据
    bool deserializeAll(const std::string& data);  // 反序列化所有数据
    public:
    GameManager();
    ~GameManager();
    //游戏主循环
    void run();                      // 游戏主循环入口
    //菜单显示
    void showMainMenu();             // 显示主菜单
    //各功能入口
    void viewPlayerInfo();           // 查看角色信息
    void viewBag();                  // 查看背包
    void enterForge();               // 进入锻造工坊
    void enterShop();                // 进入商店
    void enterTask();                // 进入任务面板
    void enterBattle();              // 进入战斗系统
    void saveGame();                 // 存档
    void loadGame();                 // 读档
    //存档/读档
    bool saveToFile(const std::string& filename);   // 保存到文件
    bool loadFromFile(const std::string& filename); // 从文件读取
    //游戏状态
    GameState getState() const;
    void setState(GameState state);
    //玩家访问
    Player* getPlayer() const;
};

#endif // GAME_MANAGER_H
