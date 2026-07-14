#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>             //算法头文件

class Player;
// 第一部分：物品基类 Item
class Item 
{
    public:
    // 物品类型枚举
    enum ItemType 
    {
        FOOD,       // 食物：增加属性
        WEAPON,     // 武器：增加攻击力
        ARMOR,      // 防具：增加防御力
        MEDICINE,   // 药品：治疗
        MATERIAL,   // 材料：用于合成或强化
        NONE        // 无类型
    };

    //构造函数，所有物品都需要有名称、类型和数量，默认数量为1，强化等级默认为0
    Item(const std::string& name, ItemType type, int quantity=1);
    virtual ~Item()=default;
    //Getter方法
    std::string getName() const;           // 获取物品名称
    ItemType getType() const;              // 获取物品类型
    int getQuantity() const;               // 获取物品数量
    std::string getDescription() const;    // 获取物品描述
    int getEnhanceLevel() const;           // 获取强化等级（0-10）
    //Setter方法
    void setQuantity(int qty);             // 设置物品数量
    void addQuantity(int qty);             // 增加物品数量（负数表示减少）
    void setDescription(const std::string& desc);  // 设置物品描述
    void setEnhanceLevel(int level);       // 设置强化等级（自动限制0-10）
    void addEnhanceLevel(int delta);       // 增加强化等级（正数提升，负数降低）
    //虚函数
    //使用效果
    virtual void use(Player* player);
    //显示物品信息
    virtual void display() const;

    //判断物品是否可强化,只有武器和防具可以强化
    virtual bool isEnhanceable() const;
    protected:
    std::string m_name;          // 物品名称
    ItemType m_type;             // 物品类型
    int m_quantity;              // 物品数量（多个同类物品叠加）
    std::string m_description;   // 物品描述信息
    int m_enhanceLevel;          // 强化等级（0表示未强化，最高10）

};
//食物类 Food,在战斗外使用，增加属性
class Food : public Item 
{
    public:
    //构造函数
    Food(const std::string& name, int quantity=1, int hpBonus=5, int attackBonus=0, int defenseBonus=0);
    void use(Player* player) override;          // 使用食物：恢复生命和体力
    void display() const override;              // 显示食物特有信息
    //Getter
    int getHpBonus() const;
    int getAttackBonus() const;
    int getDefenseBonus() const;
    private:
    int m_hpBonus;       // 最大HP永久增加
    int m_attackBonus;   // 攻击力永久增加
    int m_defenseBonus;  // 防御力永久增加
};
// 第三部分：武器类 Weapon
class Weapon : public Item 
{
    public:
    //构造函数
    Weapon(const std::string& name, int quantity=1, int damage=5);
    //重写虚函数
    void display() const override;                              // 显示武器特有信息
    bool isEnhanceable() const override { return true; }        // 武器可强化
    //Getter
    int getBaseDamage() const { return m_baseDamage; }
    int getDamage() const;                                      // 获取当前攻击力（基础 + 强化加成）
    //强化相关 
    int getEnhanceDamage() const;                               // 计算强化带来的攻击加成
    void Weapon::use(Player* player);
private:
    int m_baseDamage;                                           // 基础攻击力
};
// 第四部分：防具类 Armor
class Armor : public Item 
{
    public:
    //构造函数
    Armor(const std::string& name, int quantity=1, int defense=3);
    void display() const override;                                  // 显示防具特有信息
    bool isEnhanceable() const override { return true; }            // 防具可强化
    //Getter
    int getBaseDefense() const { return m_baseDefense; }
    int getDefense() const;                                         // 获取当前防御力（基础 + 强化加成）
    //强化相关
    int getEnhanceDefense() const;                                  // 计算强化带来的防御加成
    void Armor::use(Player* player);
    private:
    int m_baseDefense;       // 基础防御力
};
// 第五部分：药品类 Medicine
class Medicine : public Item
{
    public:
    //构造函数
    Medicine(const std::string& name, int quantity=1, int healAmount=20);
    //重写虚函数
    void use(Player* player) override;          // 使用药品
    void display() const override; // 显示药品特有信息
    //Getter
    int getHealAmount() const;     // 获取治疗量
    private:
    int m_healAmount;          // 治疗量
};
// 第六部分：材料类 Material
class Material : public Item 
{
    public:
    // 稀有度枚举
    // 从低到高5个等级，影响合成价值和使用场景
    enum MaterialRarity 
    {
        COMMON,      // 普通
        UNCOMMON,    // 稀有
        RARE,        // 罕见
        EPIC,        // 史诗
        LEGENDARY    // 传说
    };
    //构造函数
    Material(const std::string& name, int quantity = 1, MaterialRarity rarity = COMMON);
    //虚函数
    void display() const override;              // 显示材料特有信息
    bool isEnhanceable() const override { return false; }  // 材料不可强化
    //Getter
    MaterialRarity getRarity() const;               // 获取稀有度枚举
    std::string getRarityString() const;            // 获取稀有度的中文名称    
    //功能方法
    bool canCraftWith(const Material& other) const;  // 判断能否与其他材料合成

private:
    MaterialRarity m_rarity;   // 稀有度
};

#endif // ITEM_H
