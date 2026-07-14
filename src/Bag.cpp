#include "Bag.h"
#include <sstream>    
#include <string>     

Bag::Bag(const std::string& ownerName, int capacity): m_ownerName(ownerName), m_capacity(capacity) {}
Bag::~Bag() 
{
    clear();
}
// 物品管理

bool Bag::addItem(Item* item) 
{
    // 1. 空指针检查
    if (item == nullptr) 
    {
        std::cout << " 添加失败：物品指针为空！" << std::endl;
        return false;
    }
    // 2. 容量检查
    if (getTotalItems() >= m_capacity) 
    {
        std::cout << "背包已满，无法添加 " << item->getName() << "！" << std::endl;
        delete item;  // 释放传入的指针，防止内存泄漏
        return false;
    }
    // 3. 查找是否已有同名同类型物品
    for (auto& existingItem : m_items) 
    {
        if (existingItem->getName() == item->getName() && existingItem->getType() == item->getType()) 
        {
            // 找到同类型物品，叠加数量
            existingItem->addQuantity(item->getQuantity());
            std::cout << " 增加 " << item->getName() << " 数量: " 
                      << item->getQuantity() << "，当前总数: " 
                      << existingItem->getQuantity() << std::endl;
            delete item;  // 释放传入的指针
            return true;
        }
    }

    // 4. 未找到同类物品，直接存入
    m_items.push_back(item);
    std::cout << " 添加 " << item->getName() << " 到背包" << std::endl;
    return true;
}

// 添加物品（智能指针版本）
bool Bag::addItem(std::shared_ptr<Item> item) 
{
    if (item == nullptr) 
    {
        std::cout << " 添加失败：物品指针为空！" << std::endl;
        return false;
    }
    // 深拷贝物品
    Item* newItem = cloneItem(item.get());
    if (newItem == nullptr) 
    {
        std::cout << " 克隆物品失败！" << std::endl;
        return false;
    }
    
    return addItem(newItem);
}

//移除物品
bool Bag::removeItem(const std::string& name, int quantity) 
{
    // 遍历查找物品
    for (auto it = m_items.begin(); it != m_items.end(); ++it) 
    {
        if ((*it)->getName() == name) 
        {
            int currentQty = (*it)->getQuantity();
            if (currentQty > quantity) 
            {
                // 数量充足，只减少数量
                (*it)->addQuantity(-quantity);
                std::cout << " 移除 " << name << " ×" << quantity << "，剩余: " << (*it)->getQuantity() << std::endl;
            } 
            else 
            {
                // 数量不足或刚好，删除整个物品
                std::cout << " 移除 " << name << " ×" << currentQty << std::endl;
                delete *it;      // 释放内存
                m_items.erase(it); // 从容器中移除
            }
            return true;
        }
    }
    std::cout << " 未找到物品: " << name << std::endl;
    return false;
}
bool Bag::removeItem(Item* item, bool deleteItem) 
{
    if (item == nullptr) return false;

    for (auto it = m_items.begin(); it != m_items.end(); ++it) 
    {
        if (*it == item) 
        {
            m_items.erase(it);
            if (deleteItem) 
            {
                delete item;  // 普通删除，释放内存
            }
            // 如果 deleteItem == false，不删除，交给调用方管理
            return true;
        }
    }
    return false;
}

//使用物品
bool Bag::useItem(const std::string& name, Player* player) 
{
    for (auto& item : m_items) 
    {
        if (item->getName() == name) 
        {
            item->use(player);  // 传入玩家指针
            if (item->getQuantity() <= 0) 
            {
                removeItem(item, true);
            }
            return true;
        }
    }
    std::cout << " 未找到物品: " << name << std::endl;
    return false;
}

//查找物品
Item* Bag::findItem(const std::string& name) 
{
    for (auto& item : m_items) 
    {
        if (item->getName() == name) 
        {
            return item;
        }
    }
    return nullptr;
}

//按类型获取所有物品
std::vector<Item*> Bag::getItemsByType(Item::ItemType type) const 
{
    std::vector<Item*> result;
    for (auto& item : m_items) 
    {
        if (item->getType() == type) 
        {
            result.push_back(item);
        }
    }
    return result;
}

//获取所有武器
std::vector<Weapon*> Bag::getWeapons() const 
{
    std::vector<Weapon*> result;
    for (auto& item : m_items) 
    {
        if (item->getType() == Item::WEAPON) 
        {
            result.push_back(static_cast<Weapon*>(item));
        }
    }
    return result;
}

//获取所有防具
std::vector<Armor*> Bag::getArmors() const 
{
    std::vector<Armor*> result;
    for (auto& item : m_items) 
    {
        if (item->getType() == Item::ARMOR) 
        {
            result.push_back(static_cast<Armor*>(item));
        }
    }
    return result;
}

//获取所有材料
std::vector<Material*> Bag::getMaterials() const 
{
    std::vector<Material*> result;
    for (auto& item : m_items) 
    {
        if (item->getType() == Item::MATERIAL) 
        {
            result.push_back(static_cast<Material*>(item));
        }
    }
    return result;
}

// 按类型显示物品
void Bag::displayByType(Item::ItemType type) const 
{
    // 获取类型名称
    std::string typeName;
    switch(type) 
    {
        case Item::FOOD:     typeName = "食物"; break;
        case Item::WEAPON:   typeName = "武器"; break;
        case Item::ARMOR:    typeName = "防具"; break;
        case Item::MEDICINE: typeName = "药品"; break;
        case Item::MATERIAL: typeName = "材料"; break;
        default:             typeName = "未知"; break;
    }
    
    std::cout << "\n========== " << typeName << "列表 ==========" << std::endl;
    bool found = false;
    
    for (const auto& item : m_items) 
    {
        if (item->getType() == type) 
        {
            item->display();  // 多态调用，显示具体类型的信息
            found = true;
        }
    }
    if (!found) 
    {
        std::cout << "背包中没有" << typeName << std::endl;
    }
}

//显示所有物品
void Bag::displayAll() const 
{
    std::cout << m_ownerName << " 的背包 ("
              << getTotalItems() << "/" << m_capacity << ")"<< std::endl;
    
    if (m_items.empty()) 
    {
        std::cout << "背包是空的" << std::endl;
        return;
    }

    // 按固定顺序显示各类物品
    displayByType(Item::WEAPON);
    displayByType(Item::ARMOR);
    displayByType(Item::MEDICINE);
    displayByType(Item::FOOD);
    displayByType(Item::MATERIAL);
}
// 统计信息

//获取物品总数量
int Bag::getTotalItems() const 
{
    int total = 0;
    for (const auto& item : m_items) 
    {
        total += item->getQuantity();
    }
    return total;
}

//获取指定类型的物品数量
int Bag::getTypeCount(Item::ItemType type) const 
{
    int count = 0;
    for (const auto& item : m_items) 
    {
        if (item->getType() == type) 
        {
            count += item->getQuantity();
        }
    }
    return count;
}

//获取物品种类数
int Bag::getItemCount() const 
{
    return static_cast<int>(m_items.size());
}
//背包容量
int Bag::getCapacity() const 
{
    return m_capacity;
}


//清空背包
void Bag::clear() 
{
    for (auto& item : m_items) 
    {
        delete item;  // 释放内存
    }
    m_items.clear(); // 清空容器
}

//按类型排序
void Bag::sortByType() 
{
    std::sort(m_items.begin(), m_items.end(), [](Item* a, Item* b) { return a->getType() < b->getType(); });
}

//按名称排序
void Bag::sortByName() 
{
    std::sort(m_items.begin(), m_items.end(), [](Item* a, Item* b) { return a->getName() < b->getName(); });
}

// 背包序列化
std::string Bag::serialize() const 
{
    std::ostringstream oss;
    // 1. 物品种类数
    oss << m_items.size() << "\n";
    // 2. 逐个序列化物品
    for (Item* item : m_items) 
    {
        // 类型
        oss << static_cast<int>(item->getType()) << ",";
        // 名称
        oss << item->getName() << ",";
        // 数量
        oss << item->getQuantity() << ",";
        // 强化等级
        oss << item->getEnhanceLevel();
        // 根据类型保存特有属性
        switch (item->getType()) 
        {
            case Item::WEAPON: 
            {
                Weapon* w = static_cast<Weapon*>(item);
                oss << "," << w->getBaseDamage();
                break;
            }
            case Item::ARMOR: 
            {
                Armor* a = static_cast<Armor*>(item);
                oss << "," << a->getBaseDefense();
                break;
            }
            case Item::FOOD: 
            {
                Food* f = static_cast<Food*>(item);
                oss << "," << f->getHpBonus();
                oss << "," << f->getAttackBonus();
                oss << "," << f->getDefenseBonus();
                break;
            }
            case Item::MEDICINE: 
            {
                Medicine* m = static_cast<Medicine*>(item);
                oss << "," << m->getHealAmount();
                break;
            }
            case Item::MATERIAL: 
            {
                Material* m = static_cast<Material*>(item);
                oss << "," << static_cast<int>(m->getRarity());
                break;
            }
            default:
                break;
        }
        oss << "\n";
    }
    return oss.str();
}
// 背包反序列化
bool Bag::deserialize(const std::string& data) 
{
    if (data.empty()) return false;
    // 先清空背包
    clear();
    std::stringstream ss(data);
    std::string line;

    // 读取物品种类数
    if (!std::getline(ss, line)) return false;
    int count = std::stoi(line);

    for (int i = 0; i < count; ++i) 
    {
        if (!std::getline(ss, line)) break;
        if (line.empty()) continue;

        std::stringstream itemSS(line);
        std::string token;
        std::vector<std::string> parts;

        while (std::getline(itemSS, token, ',')) 
        {
            parts.push_back(token);
        }

        if (parts.size() < 4) continue;

        int type = std::stoi(parts[0]);
        std::string name = parts[1];
        int quantity = std::stoi(parts[2]);
        int enchantLevel = std::stoi(parts[3]);

        Item* item = nullptr;

        switch (static_cast<Item::ItemType>(type)) 
        {
            case Item::WEAPON: 
            {
                int damage = std::stoi(parts[4]);
                item = new Weapon(name, quantity, damage);
                break;
            }
            case Item::ARMOR: 
            {
                int defense = std::stoi(parts[4]);
                item = new Armor(name, quantity, defense);
                break;
            }
            case Item::FOOD: 
            {
                int hpBonus = std::stoi(parts[4]);
                int attackBonus = std::stoi(parts[5]);
                int defenseBonus = std::stoi(parts[6]);
                item = new Food(name, quantity, hpBonus, attackBonus, defenseBonus);
                break;
            }
            case Item::MEDICINE: 
            {
                int healAmount = std::stoi(parts[4]);
                item = new Medicine(name, quantity, healAmount);
                break;
            }
            case Item::MATERIAL: 
            {
                int rarity = std::stoi(parts[4]);
                item = new Material(name, quantity, static_cast<Material::MaterialRarity>(rarity));
                break;
            }
            default:
                continue;
        }

        if (item) {
            item->setEnhanceLevel(enchantLevel);
            addItem(item);
        }
    }
    return true;
}
Item* cloneItem(const Item* item) 
{
    if (!item) return nullptr;
    switch (item->getType()) 
    {
        case Item::FOOD: 
        {
            const Food* food = dynamic_cast<const Food*>(item);
            if (food) 
            {
                return new Food(food->getName(), food->getQuantity());
            }
            break;
        }
        case Item::WEAPON:
        {
            const Weapon* weapon = dynamic_cast<const Weapon*>(item);
            if (weapon)
            {
                return new Weapon(weapon->getName(), weapon->getQuantity());
            }
            break;
        }
        case Item::ARMOR:
        {
            const Armor* armor = dynamic_cast<const Armor*>(item);
            if (armor) 
            {
                return new Armor(armor->getName(), armor->getQuantity());
            }
            break;
        }
        case Item::MEDICINE: 
        {
            const Medicine* medicine = dynamic_cast<const Medicine*>(item);
            if (medicine) 
            {
                return new Medicine(medicine->getName(), medicine->getQuantity(),medicine->getHealAmount());
            }
            break;
        }
        case Item::MATERIAL: 
        {
            const Material* material = dynamic_cast<const Material*>(item);
            if (material) 
            {
                return new Material(material->getName(), material->getQuantity(),material->getRarity());
            }
            break;
        }
        default:
            break;
    }
    return nullptr;
}
