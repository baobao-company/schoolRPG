#include "Item.h"
#include <iostream>
#include <algorithm>

//Item 基类实现
Item::Item(const std::string& name, ItemType type, int quantity)
    : m_name(name), m_type(type), m_quantity(quantity), m_description(""), m_enhanceLevel(0) {}

//Getter实现
std::string Item::getName() const 
{
    return m_name;
}
Item::ItemType Item::getType() const 
{
    return m_type;
}
int Item::getQuantity() const 
{
    return m_quantity;
}
std::string Item::getDescription() const 
{
    return m_description;
}
int Item::getEnhanceLevel() const 
{
    return m_enhanceLevel;
}

//Setter实现
void Item::setQuantity(int qty) 
{
    m_quantity=qty;
}

void Item::addQuantity(int qty) 
{
    m_quantity+=qty;
    if (m_quantity<0) m_quantity=0;
}

void Item::setDescription(const std::string& desc) 
{
    m_description=desc;
}

//设置强化等级
void Item::setEnhanceLevel(int level) 
{
    if (level<0) 
    {
        m_enhanceLevel=0;
    }
    else if (level>10) 
    {
        m_enhanceLevel=10;
    } 
    else 
    {
        m_enhanceLevel=level;
    }
}

//增加强化等级
void Item::addEnhanceLevel(int delta) 
{
    setEnhanceLevel(m_enhanceLevel+delta);
}

//判断物品是否可强化
bool Item::isEnhanceable() const 
{
    return (m_type==WEAPON||m_type==ARMOR);
}

//显示物品信息
void Item::display() const 
{
    std::cout<<"物品: "<<m_name<<" | 数量: "<< m_quantity<<" | 描述: "<<m_description;
    if (isEnhanceable()&& m_enhanceLevel>0) 
    {
        std::cout<<" | 强化等级: +"<< m_enhanceLevel;
    }
    std::cout<<std::endl;
}
//Food 食物类实现
Food::Food(const std::string& name, int quantity, int hpBonus, int attackBonus, int defenseBonus)
    : Item(name, FOOD, quantity),m_hpBonus(hpBonus),m_attackBonus(attackBonus),m_defenseBonus(defenseBonus) {}

void Food::use(Player* player) 
{
    if (!player) return;
    if (m_quantity<=0) 
    {
        std::cout << m_name << " 数量不足！" << std::endl;
        return;
    }

    // 永久增加属性
    if (m_hpBonus>0) 
    {
        player->setMaxHP(player->getMaxHP()+m_hpBonus);
        player->healHP(m_hpBonus);
        std::cout << " 最大生命值永久 +" << m_hpBonus << "！" << std::endl;
    }
    if (m_attackBonus > 0) 
    {
        std::cout << " 攻击力永久 +" << m_attackBonus << "！" << std::endl;
    }
    if (m_defenseBonus > 0) 
    {
        std::cout << " 防御力永久 +" << m_defenseBonus << "！" << std::endl;
    }

    m_quantity--;
}

void Food::display() const 
{
    std::cout << "[食物] " << m_name<< " | 数量: " << m_quantity<< " | 最大HP+" << m_hpBonus<< " | 攻击+" << m_attackBonus<< " | 防御+" << m_defenseBonus<< " | " << m_description << std::endl;
}

int Food::getHpBonus() const { return m_hpBonus; }
int Food::getAttackBonus() const { return m_attackBonus; }
int Food::getDefenseBonus() const { return m_defenseBonus; }

//Weapon 武器类实现
Weapon::Weapon(const std::string& name, int quantity, int damage): Item(name, WEAPON, quantity), m_baseDamage(damage){}

//显示武器信息
void Weapon::display() const 
{
    std::cout << "[武器] " << m_name;
    if (m_enhanceLevel > 0)
    {
        std::cout << " +" << m_enhanceLevel;
    }
    std::cout << " | 攻击: " << getDamage() 
              << " (基础" << m_baseDamage << " + 强化" << getEnhanceDamage() << ")"
              << " | " << m_description << std::endl;
}

int Weapon::getDamage() const 
{
    return m_baseDamage + getEnhanceDamage();
}

//计算强化带来的攻击加成
int Weapon::getEnhanceDamage() const 
{
    if (m_enhanceLevel == 0) return 0;
    if (m_enhanceLevel <= 2) return m_enhanceLevel * 2;
    if (m_enhanceLevel <= 5) return 4 + (m_enhanceLevel - 2) * 3;
    if (m_enhanceLevel <= 8) return 13 + (m_enhanceLevel - 5) * 4;
    return 25 + (m_enhanceLevel - 8) * 5;
}

//Armor 防具类实现
Armor::Armor(const std::string& name, int quantity, int defense)
    : Item(name, ARMOR, quantity), m_baseDefense(defense){}

//显示防具信息
void Armor::display() const 
{
    std::cout << "[防具] " << m_name;
    if (m_enhanceLevel > 0) 
    {
        std::cout << " +" << m_enhanceLevel;
    }
    std::cout << " | 防御: " << getDefense() 
              << " (基础" << m_baseDefense << " + 强化" << getEnhanceDefense() << ")"
              << " | " << m_description << std::endl;
}
//获取当前防御力
int Armor::getDefense() const 
{
    return m_baseDefense + getEnhanceDefense();
}

int Armor::getEnhanceDefense() const 
{
    return m_enhanceLevel * 2;
}

//Medicine 药品类实现
Medicine::Medicine(const std::string& name, int quantity, int healAmount)
    : Item(name, MEDICINE, quantity), m_healAmount(healAmount){}
//使用药品,恢复生命值
void Medicine::use(Player* player) 
{
    if (!player) return;
    if (m_quantity <= 0) 
    {
        std::cout << m_name << " 数量不足！" << std::endl;
        return;
    }

    int actualHeal = player->healHP(m_healAmount);
    if (actualHeal > 0) {
        std::cout << "使用 " << m_name << "，恢复 " << actualHeal << " HP！" << std::endl;
        m_quantity--;
    } else 
    {
        std::cout << " HP已满，无需使用 " << m_name << std::endl;
    }
}

//显示药品信息
void Medicine::display() const 
{
    std::cout << "[药品] " << m_name 
              << " | 数量: " << m_quantity 
              << " | 治疗量: " << m_healAmount;
    std::cout << " | " << m_description << std::endl;
}

int Medicine::getHealAmount() const {
    return m_healAmount;
}
//Material 材料类实现
Material::Material(const std::string& name, int quantity, MaterialRarity rarity)
    : Item(name, MATERIAL, quantity), m_rarity(rarity){}
//显示材料信息
void Material::display() const 
{
    std::cout << "[材料] " << m_name 
              << " | 数量: " << m_quantity 
              << " | 稀有度: " << getRarityString() 
              << " | " << m_description << std::endl;
}

Material::MaterialRarity Material::getRarity() const 
{
    return m_rarity;
}

//获取稀有度的中文名称,用于界面显示
std::string Material::getRarityString() const 
{
    switch(m_rarity) 
    {
        case COMMON:    return "普通";
        case UNCOMMON:  return "稀有";
        case RARE:      return "罕见";
        case EPIC:      return "史诗";
        case LEGENDARY: return "传说";
        default:        return "未知";
    }
}
bool Material::canCraftWith(const Material& other) const 
{
    return m_rarity == other.m_rarity;
}