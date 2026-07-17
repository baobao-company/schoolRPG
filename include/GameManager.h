#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include <string>
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>


class Player;
class Bag;
class TaskManager;
class Shop;
class BattleSystem;
class ForgeManager;
class MainMenuUI;
class PlayerInfoUI;
class BagUI;
class ForgeUI;
class ShopUI;
class TaskUI;
class BattleUI;

enum class GameState
{
    MAIN_MENU, RUNNING, IN_BATTLE, IN_SHOP, IN_FORGE,
    IN_TASK, VIEWING_BAG, VIEW_PLAYER, SAVING, LOADING, EXITING
};

class GameManager
{
private:
    Player* m_player;
    GameState m_state;
    bool m_runFlag;

    std::unique_ptr<MainMenuUI> uiMain;
    std::unique_ptr<PlayerInfoUI> uiPlayer;
    std::unique_ptr<BagUI> uiBag;
    std::unique_ptr<ForgeUI> uiForge;
    std::unique_ptr<ShopUI> uiShop;
    std::unique_ptr<TaskUI> uiTask;
   // 旧代码：std::unique_ptr<BattleUI> uiBattle;
    // 替换为（无参构造，启动不传入空player）
    std::unique_ptr<BattleUI> uiBattle;

    // ========== 新增回血相关 ==========
    sf::Clock m_healTimer;
    const float HEAL_CD = 1.0f;   // 每3秒回一次血
    const int HEAL_NUM = 10;       // 每次恢复3HP

    void initGameNew();
    void exitGameLogic();
    std::string serializeAllData() const;
    bool deserializeAllData(const std::string& dat);
public:
    GameManager();
    ~GameManager();
    //SFML事件&渲染
    void handleSFEvent(sf::Event& ev, sf::RenderWindow& win);
    void renderAll(sf::RenderWindow& win, sf::Font& font);
    //界面切换入口
    void openPlayerInfo();
    void openBag();
    void openForge();
    void openShop();
    void openTask();
    void openBattle();
    void doSave();
    void doLoad();
    //存档IO
    bool saveToFile(const std::string& path);
    bool loadFromFile(const std::string& path);
    //状态
    void setState(GameState s);
    GameState getState() const;
    Player* getPlayer() const;
};
#endif
