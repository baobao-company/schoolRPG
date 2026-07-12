#ifndef PLAYER_H
#define PLAYER_H

#include <string>
class Bag;          
class TaskManager;  
class ForgeManager; 
class Shop;
class Weapon;
class Armor;

class Player 
{
    private:
    // ----- 基础属性 -----
    std::string m_name;        // 玩家名称
    int m_level;               // 当前等级（1-100）
    int m_hp;                  // 当前生命值
    int m_maxHP;               // 最大生命值
    int m_baseAttack;          // 基础攻击力
    int m_baseDefense;         // 基础防御力
    int m_speed;               // 速度属性，影响战斗顺序
    int m_exp;                 // 当前经验值
    int m_gold;                // 金币数
    Bag* m_bag;                // 背包
    Shop* m_shop;              //商店
    TaskManager* m_taskManager; // 任务管理器
    ForgeManager* m_forgeManager; // 锻造工坊
    Weapon* m_equippedWeapon;   // 当前装备的武器（nullptr表示未装备）
    Armor* m_equippedArmor;     // 当前装备的防具（nullptr表示未装备）

    public:
    Player(const std::string& name);
    ~Player();
    // 装备操作
    bool equipWeapon(Weapon* weapon);   // 装备武器（从背包移除，放入装备槽）
    bool equipArmor(Armor* armor);      // 装备防具
    void unequipWeapon();               // 卸下武器（放回背包）
    void unequipArmor();                // 卸下防具
    //Getter方法
    std::string getName() const;          // 获取玩家名称
    int getLevel() const;                 // 获取等级
    int getHP() const;                    // 获取当前生命值
    int getMaxHP() const;                 // 获取最大生命值
    int getBaseAttack() const;            // 获取基础攻击力
    int getBaseDefense() const;           // 获取基础防御力
    int getSpeed() const;                 // 获取速度属性
    int getTotalAttack() const;           // 基础 + 武器
    int getTotalDefense() const;          // 基础 + 防具
    int getExp() const;                   // 获取当前经验值
    int getExpToNextLevel() const;        // 获取升级所需经验值
    int getGold() const;                  // 获取金币数
    Weapon* getEquippedWeapon() const { return m_equippedWeapon; }
    Armor* getEquippedArmor() const { return m_equippedArmor; }

    //Setter方法
    void setName(const std::string& name); // 设置玩家名称
    void setLevel(int level);              // 设置等级
    void setHP(int hp);                    // 设置当前生命值
    void setMaxHP(int maxHP);              // 设置最大生命值
    void setBaseAttack(int val);           // 设置基础攻击力
    void setBaseDefense(int val);          // 设置基础防御力
    void setSpeed(int val);                // 设置速度属性
    void addBaseAttack(int val);           // 增加基础攻击力
    void addBaseDefense(int val);          // 增加基础防御力
    void addSpeed(int val);                // 增加速度属性
    void setExp(int exp);                  // 设置当前经验值
    void setGold(int gold);                // 设置金币数

    //属性操作
    //增加经验值
    void addExp(int amount);
    //增加金币
    bool addGold(int amount);
    //恢复生命值
    int healHP(int amount);
    //受到伤害
    int takeDamage(int damage);
    //完全恢复
    void fullRestore();
    //升级
    void levelUp();
    //检查是否可以升级
    bool canLevelUp() const;
    //计算升级所需经验值
    static int getExpRequired(int level);
    //子系统访问
    
    //获取背包
    Bag* getBag() const;
    //获取任务管理器
    TaskManager* getTaskManager() const;
    //获取锻造工坊
    ForgeManager* getForgeManager() const;
    Shop* getShop() const { return m_shop; }
    //显示
    void displayInfo() const;
    // 序列化（存档/读档）
    std::string serialize() const;
    //反序列化玩家数据
    bool deserialize(const std::string& data);
};

#endif // PLAYER_H