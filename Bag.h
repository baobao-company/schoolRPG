#ifndef BAG_H
#define BAG_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// 物品基类
class Item {
public:
    enum ItemType {
        FOOD,       // 食物
        WEAPON,     // 武器
        MEDICINE,   // 药品
        MATERIAL,   // 材料 ← 新增类型
        NONE
    };

    Item(const std::string& name, ItemType type, int quantity = 1)
        : m_name(name), m_type(type), m_quantity(quantity), m_description("") {}

    virtual ~Item() = default;

    std::string getName() const { return m_name; }
    ItemType getType() const { return m_type; }
    int getQuantity() const { return m_quantity; }
    std::string getDescription() const { return m_description; }
    
    void setQuantity(int qty) { m_quantity = qty; }
    void addQuantity(int qty) { m_quantity += qty; }
    void setDescription(const std::string& desc) { m_description = desc; }

    virtual void use() = 0;
    virtual void display() const {
        std::cout << "物品: " << m_name 
                  << " | 数量: " << m_quantity 
                  << " | 描述: " << m_description << std::endl;
    }

protected:
    std::string m_name;
    ItemType m_type;
    int m_quantity;
    std::string m_description;
};

// 食物类
class Food : public Item {
public:
    Food(const std::string& name, int quantity = 1, int healthRestore = 10)
        : Item(name, FOOD, quantity), m_healthRestore(healthRestore), m_energyRestore(0) {}

    Food(const std::string& name, int quantity, int healthRestore, int energyRestore)
        : Item(name, FOOD, quantity), m_healthRestore(healthRestore), m_energyRestore(energyRestore) {}

    void use() override {
        if (m_quantity > 0) {
            std::cout << "食用 " << m_name 
                      << ", 恢复生命值: " << m_healthRestore 
                      << ", 恢复体力: " << m_energyRestore << std::endl;
            m_quantity--;
        } else {
            std::cout << m_name << " 数量不足!" << std::endl;
        }
    }

    void display() const override {
        std::cout << "[食物] " << m_name 
                  << " | 数量: " << m_quantity 
                  << " | 生命恢复: " << m_healthRestore 
                  << " | 体力恢复: " << m_energyRestore 
                  << " | " << m_description << std::endl;
    }

    int getHealthRestore() const { return m_healthRestore; }
    int getEnergyRestore() const { return m_energyRestore; }

private:
    int m_healthRestore;
    int m_energyRestore;
};

// 武器类
class Weapon : public Item {
public:
    Weapon(const std::string& name, int quantity = 1, int damage = 5, int durability = 100)
        : Item(name, WEAPON, quantity), m_damage(damage), m_durability(durability), 
          m_maxDurability(durability), m_range(1.0f) {}

    Weapon(const std::string& name, int quantity, int damage, int durability, float range)
        : Item(name, WEAPON, quantity), m_damage(damage), m_durability(durability), 
          m_maxDurability(durability), m_range(range) {}

    void use() override {
        if (m_quantity > 0 && m_durability > 0) {
            std::cout << "使用 " << m_name << " 攻击! 造成伤害: " << m_damage 
                      << " | 耐久度: " << m_durability << "/" << m_maxDurability << std::endl;
            m_durability--;
            if (m_durability <= 0) {
                std::cout << m_name << " 损坏了!" << std::endl;
                m_quantity--;
                m_durability = m_maxDurability;
            }
        } else if (m_quantity <= 0) {
            std::cout << m_name << " 数量不足!" << std::endl;
        } else {
            std::cout << m_name << " 已损坏,需要修理!" << std::endl;
        }
    }

    void display() const override {
        std::cout << "[武器] " << m_name 
                  << " | 数量: " << m_quantity 
                  << " | 伤害: " << m_damage 
                  << " | 耐久: " << m_durability << "/" << m_maxDurability 
                  << " | 范围: " << m_range 
                  << " | " << m_description << std::endl;
    }

    void repair(int amount) {
        m_durability = std::min(m_durability + amount, m_maxDurability);
        std::cout << m_name << " 修复完成, 当前耐久度: " << m_durability << std::endl;
    }

    int getDamage() const { return m_damage; }
    int getDurability() const { return m_durability; }
    int getMaxDurability() const { return m_maxDurability; }
    float getRange() const { return m_range; }

private:
    int m_damage;
    int m_durability;
    int m_maxDurability;
    float m_range;
};

// 药品类
class Medicine : public Item {
public:
    Medicine(const std::string& name, int quantity = 1, int healAmount = 20)
        : Item(name, MEDICINE, quantity), m_healAmount(healAmount), m_cureStatus("") {}

    Medicine(const std::string& name, int quantity, int healAmount, const std::string& cureStatus)
        : Item(name, MEDICINE, quantity), m_healAmount(healAmount), m_cureStatus(cureStatus) {}

    void use() override {
        if (m_quantity > 0) {
            std::cout << "使用 " << m_name 
                      << ", 恢复生命值: " << m_healAmount;
            if (!m_cureStatus.empty()) {
                std::cout << ", 治愈状态: " << m_cureStatus;
            }
            std::cout << std::endl;
            m_quantity--;
        } else {
            std::cout << m_name << " 数量不足!" << std::endl;
        }
    }

    void display() const override {
        std::cout << "[药品] " << m_name 
                  << " | 数量: " << m_quantity 
                  << " | 治疗量: " << m_healAmount;
        if (!m_cureStatus.empty()) {
            std::cout << " | 特殊效果: " << m_cureStatus;
        }
        std::cout << " | " << m_description << std::endl;
    }

    int getHealAmount() const { return m_healAmount; }
    std::string getCureStatus() const { return m_cureStatus; }

private:
    int m_healAmount;
    std::string m_cureStatus;
};

// ==================== 材料类（新增） ====================
class Material : public Item {
public:
    enum MaterialRarity {
        COMMON,      // 普通
        UNCOMMON,    // 稀有
        RARE,        // 罕见
        EPIC,        // 史诗
        LEGENDARY    // 传说
    };

    Material(const std::string& name, int quantity = 1, 
             MaterialRarity rarity = COMMON)
        : Item(name, MATERIAL, quantity), m_rarity(rarity), m_craftValue(1) {}

    Material(const std::string& name, int quantity, 
             MaterialRarity rarity, int craftValue)
        : Item(name, MATERIAL, quantity), m_rarity(rarity), m_craftValue(craftValue) {}

    void use() override {
        // 材料通常不能直接使用，而是用于合成
        std::cout << m_name << " 是材料,无法直接使用。" << std::endl;
        std::cout << "稀有度: " << getRarityString() 
                  << " | 合成价值: " << m_craftValue << std::endl;
    }

    void display() const override {
        std::cout << "[材料] " << m_name 
                  << " | 数量: " << m_quantity 
                  << " | 稀有度: " << getRarityString() 
                  << " | 合成价值: " << m_craftValue 
                  << " | " << m_description << std::endl;
    }

    MaterialRarity getRarity() const { return m_rarity; }
    int getCraftValue() const { return m_craftValue; }

    std::string getRarityString() const {
        switch(m_rarity) {
            case COMMON:    return "普通";
            case UNCOMMON:  return "稀有";
            case RARE:      return "罕见";
            case EPIC:      return "史诗";
            case LEGENDARY: return "传说";
            default:        return "未知";
        }
    }

    // 材料特有的合成检查
    bool canCraftWith(const Material& other) const {
        // 简单示例：相同稀有度的材料可以合成
        return m_rarity == other.m_rarity;
    }

private:
    MaterialRarity m_rarity;
    int m_craftValue;  // 合成价值
};

// 背包类
class Bag {
public:
    Bag(const std::string& ownerName = "Player", int capacity = 20)
        : m_ownerName(ownerName), m_capacity(capacity) {}

    ~Bag() {
        clear();
    }

    // 添加物品
    bool addItem(Item* item) {
        if (item == nullptr) return false;
        
        if (getTotalItems() >= m_capacity) {
            std::cout << "背包已满,无法添加 " << item->getName() << "!" << std::endl;
            return false;
        }

        // 查找是否已有同名同类型物品
        for (auto& existingItem : m_items) {
            if (existingItem->getName() == item->getName() && 
                existingItem->getType() == item->getType()) {
                existingItem->addQuantity(item->getQuantity());
                std::cout << "增加 " << item->getName() << " 数量: " 
                          << item->getQuantity() << ", 当前总数: " 
                          << existingItem->getQuantity() << std::endl;
                delete item;
                return true;
            }
        }

        m_items.push_back(item);
        std::cout << "添加 " << item->getName() << " 到背包" << std::endl;
        return true;
    }

    // 移除物品
    bool removeItem(const std::string& name, int quantity = 1) {
        for (auto it = m_items.begin(); it != m_items.end(); ++it) {
            if ((*it)->getName() == name) {
                if ((*it)->getQuantity() > quantity) {
                    (*it)->addQuantity(-quantity);
                    std::cout << "移除 " << name << " x" << quantity 
                              << ", 剩余: " << (*it)->getQuantity() << std::endl;
                } else {
                    std::cout << "移除 " << name << std::endl;
                    delete *it;
                    m_items.erase(it);
                }
                return true;
            }
        }
        std::cout << "未找到物品: " << name << std::endl;
        return false;
    }

    // 使用物品
    bool useItem(const std::string& name) {
        for (auto& item : m_items) {
            if (item->getName() == name) {
                item->use();
                if (item->getQuantity() <= 0) {
                    removeItem(name, 0);
                }
                return true;
            }
        }
        std::cout << "未找到物品: " << name << std::endl;
        return false;
    }

    // 查找物品
    Item* findItem(const std::string& name) {
        for (auto& item : m_items) {
            if (item->getName() == name) {
                return item;
            }
        }
        return nullptr;
    }

    // 按类型显示物品
    void displayByType(Item::ItemType type) const {
        std::string typeName;
        switch(type) {
            case Item::FOOD:     typeName = "食物"; break;
            case Item::WEAPON:   typeName = "武器"; break;
            case Item::MEDICINE: typeName = "药品"; break;
            case Item::MATERIAL: typeName = "材料"; break;  // ← 新增
            default:             typeName = "未知"; break;
        }
        
        std::cout << "\n========== " << typeName << "列表 ==========" << std::endl;
        bool found = false;
        for (const auto& item : m_items) {
            if (item->getType() == type) {
                item->display();
                found = true;
            }
        }
        if (!found) {
            std::cout << "背包中没有" << typeName << std::endl;
        }
    }

    // 显示所有物品
    void displayAll() const {
        std::cout << "\n========== " << m_ownerName << " 的背包 ("
                  << getTotalItems() << "/" << m_capacity << ") ==========" << std::endl;
        
        if (m_items.empty()) {
            std::cout << "背包是空的" << std::endl;
            return;
        }

        displayByType(Item::WEAPON);
        displayByType(Item::MEDICINE);
        displayByType(Item::FOOD);
        displayByType(Item::MATERIAL);  // ← 新增
    }

    // 获取物品总数
    int getTotalItems() const {
        int total = 0;
        for (const auto& item : m_items) {
            total += item->getQuantity();
        }
        return total;
    }

    // 获取不同类型的物品数量
    int getTypeCount(Item::ItemType type) const {
        int count = 0;
        for (const auto& item : m_items) {
            if (item->getType() == type) {
                count += item->getQuantity();
            }
        }
        return count;
    }

    // 清理背包
    void clear() {
        for (auto& item : m_items) {
            delete item;
        }
        m_items.clear();
    }

    // 排序背包
    void sortByType() {
        std::sort(m_items.begin(), m_items.end(), 
                  [](Item* a, Item* b) { return a->getType() < b->getType(); });
    }

    void sortByName() {
        std::sort(m_items.begin(), m_items.end(), 
                  [](Item* a, Item* b) { return a->getName() < b->getName(); });
    }

private:
    std::string m_ownerName;
    int m_capacity;
    std::vector<Item*> m_items;
};

#endif // BAG_H