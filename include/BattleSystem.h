#ifndef BATTLE_H
#define BATTLE_H

#include <string>
#include <vector>
#include <memory>
#include <random>
#include <ctime>

class Player;
class Item;
class Weapon;
class Armor;
class Material;

enum class EnemyType 
{
    SLIME,      // 史莱姆 - 最基础，容易击败
    WILD_CAT,   // 野猫 - 速度型，攻击高但血少
    GIANT_FISH, // 变异鱼 - 水中怪物，血量中等
    ANGRY_GOOSE,// 大白鹅 - 攻击高，血量较高
    SHADOW_BOSS // 暗影校长 - Boss级敌人
};

enum class EnemyState 
{
    ALIVE,      // 存活
    DEAD,       // 死亡
    FLEEING     // 逃跑（暂未使用）
};

class Enemy 
{
public:
    static std::mt19937 s_rng;

    Enemy(const std::string& name, EnemyType type, int hp, int attack, int defense, int speed,int expReward, int goldReward);
    virtual ~Enemy() = default;
    //Getter 方法
    std::string getName() const;
    EnemyType getType() const;
    int getHP() const;
    int getMaxHP() const;
    int getAttack() const;
    int getDefense() const;
    int getSpeed() const;
    int getExpReward() const;
    int getGoldReward() const;
    EnemyState getState() const;
    bool isAlive() const;

    //Setter 方法 
    void setHP(int hp);
    void setState(EnemyState state);
    void setExpReward(int exp);
    void setGoldReward(int gold);
    //核心战斗方法
    //伤害
    virtual int attack(Player* player);
    //受到伤害
    int takeDamage(int damage);
    //显示敌人信息
    virtual void display() const;
    protected:
    std::string m_name;        // 敌人名称
    EnemyType m_type;          // 敌人类型
    int m_hp;                  // 当前生命值
    int m_maxHP;               // 最大生命值
    int m_attack;              // 攻击力
    int m_defense;             // 防御力
    int m_speed;               // 速度属性，影响战斗顺序
    int m_expReward;           // 经验奖励
    int m_goldReward;          // 金币奖励
    EnemyState m_state;        // 当前状态
};

//史莱姆类
class Slime : public Enemy 
{
    public:
    Slime();
    void display() const override;
};

//野猫类
class WildCat : public Enemy 
{
public:
    WildCat();
    void display() const override;
};

//变异鱼类
class GiantFish : public Enemy 
{
public:
    GiantFish();
    void display() const override;
};

//大白鹅类
class AngryGoose : public Enemy 
{
public:
    AngryGoose();
    // 重写攻击方法：大白鹅攻击力更高，可能造成暴击
    int attack(Player* player) override;
    void display() const override;
};

//暗影校长类
class ShadowBoss : public Enemy 
{
public:
    ShadowBoss();
    // 重写攻击方法：Boss特殊攻击，伤害更高
    int attack(Player* player) override;
    void display() const override;
};

// 战斗系统类 BattleSystem
class BattleSystem 
{
public:
    //构造函数
    BattleSystem(Player* player);
    ~BattleSystem() = default;
    //主菜单
    void showMenu();
    //战斗流程
    void startBattle(int enemyTypeIndex);     // 开始战斗
    void playerTurn(Enemy* enemy);            // 玩家回合
    void enemyTurn(Enemy* enemy);             // 敌人回合
    //敌人管理
    void showEnemyList() const;               // 显示敌人列表
    std::shared_ptr<Enemy> createEnemy(EnemyType type);  // 创建敌人
    //奖励发放
    void grantRewards(Enemy* enemy);          // 发放经验、金币、物品
    private:
    Player* m_player;                         // 玩家指针
    std::vector<std::shared_ptr<Enemy>> m_enemyPool;  // 敌人池
    bool m_battleActive;                      // 战斗是否进行中
    //私有方法
    void initEnemyPool();                     // 初始化敌人池
    void displayBattleStatus(Enemy* enemy);   // 显示战斗状态
    void dropItems(Enemy* enemy);             // 掉落物品
};

#endif // BATTLE_H
