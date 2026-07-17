#ifndef BATTLE_UI_H
#define BATTLE_UI_H
#include "UIBase.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <vector>
#include <sstream>
// 前置声明
class BattleSystem;
class Player;
class GameManager;
class Enemy;
class Bag;

enum BattleStage
{
    ST_IDLE,        // 待机：敌人选择界面
    ST_PLAYER_TURN, // 玩家回合：显示攻击/喝药
    ST_ENEMY_TURN,  // 敌人回合：自动执行
    ST_END          // 战斗结束（胜利/战败，停留看结算）
};

// 单条敌人条目数据结构
struct EnemyItem
{
    int idx;
    std::wstring desc;
    sf::FloatRect btnRect;
};

class BattleUI : public UIBase
{
private:
    BattleSystem* m_bs = nullptr;
    std::shared_ptr<Enemy> m_curEnemy;
    BattleStage m_battleStage = ST_IDLE;
    std::wstring m_battleMsg;
    std::vector<EnemyItem> m_enemyItemList;
    bool m_btnDirty;
    // 新增：日志最大显示行数，超出自动删最早内容
    const int MAX_LOG_LINE = 12;
    // 拆分添加日志函数，统一处理行数截断
    void appendLog(const std::wstring& line);
public:
    BattleUI();
    BattleUI(Player* p) = delete;
    ~BattleUI() override;
    void render(sf::RenderWindow& win, sf::Font& font, void* playerData) override;
    void handleEvent(sf::Event& e, sf::RenderWindow& win, GameManager* gm) override;
    void stepBattle(Player* p);
    void initEnemyItemList();
};
#endif