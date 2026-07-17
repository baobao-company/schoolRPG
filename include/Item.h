#ifndef ITEM_H
#define ITEM_H
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
class Player;
class Item
{
public:
    enum ItemType
    {
        FOOD, WEAPON, ARMOR, MEDICINE, MATERIAL, NONE
    };
    Item(const std::wstring& name, ItemType type, int quantity = 1);
    virtual ~Item() = default;
    std::wstring getName() const;
    ItemType getType() const;
    int getQuantity() const;
    std::wstring getDescription() const;
    int getEnhanceLevel() const;
    void setQuantity(int qty);
    void addQuantity(int qty);
    void setDescription(const std::wstring& desc);
    void setEnhanceLevel(int level);
    void addEnhanceLevel(int delta);
    virtual void use(Player* player) = 0;
    virtual std::wstring getDisplayText() const;
    virtual bool isEnhanceable() const;
protected:
    std::wstring m_name;
    ItemType m_type;
    int m_quantity;
    std::wstring m_description;
    int m_enhanceLevel;
};
// 食物
class Food : public Item
{
public:
    Food(const std::wstring& name, int quantity = 1, int hpBonus = 5, int atk = 0, int def = 0);
    void use(Player* player) override;
    std::wstring getDisplayText() const override;
    int getHpBonus() const;
    int getAtkBonus() const;
    int getDefBonus() const;
private:
    int m_hpBonus, m_atkBonus, m_defBonus;
};
// 武器
class Weapon : public Item
{
public:
    Weapon(const std::wstring& name, int quantity = 1, int baseDmg = 5);
    void use(Player* player) override;
    std::wstring getDisplayText() const override;
    bool isEnhanceable() const override { return true; }
    int getBaseDamage() const;
    int getEnhanceDmg() const;
    int getDamage() const;
private:
    int m_baseDamage;
};
// 防具
class Armor : public Item
{
public:
    Armor(const std::wstring& name, int quantity = 1, int baseDef = 3);
    void use(Player* player) override;
    std::wstring getDisplayText() const override;
    bool isEnhanceable() const override { return true; }
    int getBaseDef() const;
    int getEnhanceDef() const;
    int getDef() const;
private:
    int m_baseDef;
};
// 药品
class Medicine : public Item
{
public:
    Medicine(const std::wstring& name, int quantity = 1, int heal = 20);
    void use(Player* player) override;
    std::wstring getDisplayText() const override;
    int getHealAmt() const;
private:
    int m_healAmount;
};
// 材料
class Material : public Item
{
public:
    enum Rarity { COMMON, UNCOMMON, RARE, EPIC, LEGENDARY };
    Material(const std::wstring& name, int quantity = 1, Rarity r = COMMON);
    void use(Player* player) override;
    std::wstring getDisplayText() const override;
    bool isEnhanceable() const override { return false; }
    Rarity getRarity() const;
    std::wstring getRarityStr() const;
    bool canMerge(const Material& other) const;
private:
    Rarity m_rarity;
};
Item* cloneItem(const Item* src);
#endif
