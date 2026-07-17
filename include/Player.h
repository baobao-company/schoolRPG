#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include "Item.h"
class Bag;
class TaskManager;
class ForgeManager;
class Shop;
class Weapon;
class Armor;
class Player
{
private:
    std::wstring m_name;
    int m_level;
    int m_hp, m_maxHP;
    int m_baseAtk, m_baseDef, m_speed;
    int m_exp, m_gold;
    Bag* m_bag;
    Shop* m_shop;
    TaskManager* m_taskMgr;
    ForgeManager* m_forge;
    Weapon* m_equipWep;
    Armor* m_equipArm;
public:
    Player(const std::wstring& name);
    ~Player();
    // 装备
    bool equipWeapon(Weapon* w);
    bool equipArmor(Armor* a);
    void unequipWeapon();
    void unequipArmor();
    // Getter
    std::wstring getName() const;
    int getLevel() const;
    int getHP() const;
    int getMaxHP() const;
    int getBaseAttack() const;
    int getBaseDefense() const;
    int getSpeed() const;
    int getTotalAtk() const;
    int getTotalDef() const;
    int getExp() const;
    int getExpNext() const;
    int getGold() const;
    Weapon* getEquipWep() const;
    Armor* getEquipArm() const;
    // Setter
    void setName(const std::wstring&);
    void setLevel(int);
    void setHP(int);
    void setMaxHP(int);
    void setBaseAttack(int);
    void setBaseDefense(int);
    void setSpeed(int);
    void setExp(int);
    void setGold(int);
    // 数值操作
    void addExp(int amt);
    bool addGold(int amt);
    void addBaseAttack(int v);
    void addBaseDefense(int v);
    int healHP(int amt);
    int takeDmg(int dmg);
    void fullRestore();
    bool canLevelUp() const;
    void levelUp();
    static int expNeed(int lvl);
    // 子系统
    Bag* getBag() const;
    Shop* getShop() const;
    TaskManager* getTaskManager() const;
    ForgeManager* getForge() const;
    // UI用文本输出
    std::wstring getInfoText() const;
    // 序列化存档
    std::string serialize() const;
    bool deserialize(const std::string& data);
};
#endif
