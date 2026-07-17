#include "Bag.h"
#include "Player.h"
#include "Item.h"
#include <sstream>
#include <algorithm>
Bag::Bag(const std::wstring& owner, int cap)
    : m_owner(owner), m_cap(cap)
{}
Bag::~Bag()
{
    clear();
}
bool Bag::addItem(Item* item)
{
    if (!item) return false;
    if (totalItemCount() >= m_cap)
        return false;
    for (auto exist : m_items)
    {
        if (exist->getName() == item->getName() && exist->getType() == item->getType())
        {
            exist->addQuantity(item->getQuantity());
            delete item;
            return true;
        }
    }
    m_items.push_back(item);
    return true;
}
bool Bag::addItem(std::shared_ptr<Item> item)
{
    if (!item) return false;
    Item* copy = cloneItem(item.get());
    return addItem(copy);
}
bool Bag::removeItem(Item* item, bool del)
{
    for (auto it = m_items.begin(); it != m_items.end(); ++it)
    {
        if (*it == item)
        {
            m_items.erase(it);
            if (del) delete item;
            return true;
        }
    }
    return false;
}
bool Bag::removeItem(const std::wstring& name, int qty)
{
    for (auto it = m_items.begin(); it != m_items.end(); ++it)
    {
        if ((*it)->getName() == name)
        {
            int curr = (*it)->getQuantity();
            if (curr > qty)
            {
                (*it)->addQuantity(-qty);
            }
            else
            {
                delete *it;
                m_items.erase(it);
            }
            return true;
        }
    }
    return false;
}
bool Bag::useItem(const std::wstring& name, Player* p)
{
    for (auto item : m_items)
    {
        if (item->getName() == name)
        {
            item->use(p);
            if (item->getQuantity() <= 0)
                removeItem(item, true);
            return true;
        }
    }
    return false;
}
Item* Bag::findItem(const std::wstring& name)
{
    for (auto item : m_items)
        if (item->getName() == name)
            return item;
    return nullptr;
}
std::vector<Item*> Bag::getByType(Item::ItemType t) const
{
    std::vector<Item*> res;
    for (auto i : m_items)
        if (i->getType() == t) res.push_back(i);
    return res;
}
std::vector<Weapon*> Bag::getWeapons() const
{
    std::vector<Weapon*> res;
    for (auto i : m_items)
        if (i->getType() == Item::WEAPON)
            res.push_back(static_cast<Weapon*>(i));
    return res;
}
std::vector<Armor*> Bag::getArmors() const
{
    std::vector<Armor*> res;
    for (auto i : m_items)
        if (i->getType() == Item::ARMOR)
            res.push_back(static_cast<Armor*>(i));
    return res;
}
std::vector<Material*> Bag::getMaterials() const
{
    std::vector<Material*> res;
    for (auto i : m_items)
        if (i->getType() == Item::MATERIAL)
            res.push_back(static_cast<Material*>(i));
    return res;
}
std::wstring Bag::getAllText() const
{
    std::wstringstream ss;
    ss << m_owner << L" 背包 (" << std::to_wstring(totalItemCount()) << L"/" << std::to_wstring(m_cap) << L")\n";
    if (m_items.empty())
    {
        ss << L"背包为空";
        return ss.str();
    }
    auto print = [&](Item::ItemType type, std::wstring title) {
        ss << L"\n=====" << title << L"=====\n";
        bool has = false;
        for (auto i : m_items)
        {
            if (i->getType() == type)
            {
                ss << i->getDisplayText() << L"\n";
                has = true;
            }
        }
        if (!has) ss << L"无\n";
    };
    print(Item::WEAPON, L"武器");
    print(Item::ARMOR, L"防具");
    print(Item::MEDICINE, L"药品");
    print(Item::FOOD, L"食物");
    print(Item::MATERIAL, L"材料");
    return ss.str();
}
int Bag::totalItemCount() const
{
    int sum = 0;
    for (auto i : m_items) sum += i->getQuantity();
    return sum;
}
int Bag::typeCount(Item::ItemType t) const
{
    int sum = 0;
    for (auto i : m_items)
        if (i->getType() == t) sum += i->getQuantity();
    return sum;
}
int Bag::kindCount() const
{
    return (int)m_items.size();
}
int Bag::getCap() const
{
    return m_cap;
}
void Bag::clear()
{
    for (auto i : m_items) delete i;
    m_items.clear();
}
void Bag::sortType()
{
    std::sort(m_items.begin(), m_items.end(),
        [](Item* a, Item* b) { return a->getType() < b->getType(); });
}
void Bag::sortName()
{
    std::sort(m_items.begin(), m_items.end(),
        [](Item* a, Item* b) { return a->getName() < b->getName(); });
}
std::string Bag::serialize() const
{
    std::ostringstream oss;
    oss << m_items.size() << "\n";
    for (Item* item : m_items)
    {
        oss << (int)item->getType() << ","
            << std::string(item->getName().begin(),item->getName().end()) << ","
            << item->getQuantity() << ","
            << item->getEnhanceLevel();
        switch (item->getType())
        {
            case Item::WEAPON:
                oss << "," << static_cast<Weapon*>(item)->getBaseDamage();
                break;
            case Item::ARMOR:
                oss << "," << static_cast<Armor*>(item)->getBaseDef();
                break;
            case Item::FOOD:
            {
                Food* f = static_cast<Food*>(item);
                oss << "," << f->getHpBonus() << "," << f->getAtkBonus() << "," << f->getDefBonus();
                break;
            }
            case Item::MEDICINE:
                oss << "," << static_cast<Medicine*>(item)->getHealAmt();
                break;
            case Item::MATERIAL:
                oss << "," << (int)static_cast<Material*>(item)->getRarity();
                break;
            default: break;
        }
        oss << "\n";
    }
    return oss.str();
}
bool Bag::deserialize(const std::string& data)
{
    clear();
    std::stringstream ss(data);
    std::string line;
    std::getline(ss, line);
    int total = std::stoi(line);
    for (int i = 0; i < total; i++)
    {
        if (!std::getline(ss, line)) break;
        std::vector<std::string> parts;
        std::stringstream lineSs(line);
        std::string token;
        while (std::getline(lineSs, token, ',')) parts.push_back(token);
        if (parts.size() < 4) continue;
        int type = std::stoi(parts[0]);
        std::wstring name(parts[1].begin(), parts[1].end());
        int qty = std::stoi(parts[2]);
        int lvl = std::stoi(parts[3]);
        Item* newItem = nullptr;
        switch ((Item::ItemType)type)
        {
            case Item::WEAPON:
                newItem = new Weapon(name, qty, std::stoi(parts[4]));
                break;
            case Item::ARMOR:
                newItem = new Armor(name, qty, std::stoi(parts[4]));
                break;
            case Item::FOOD:
                newItem = new Food(name, qty, std::stoi(parts[4]), std::stoi(parts[5]), std::stoi(parts[6]));
                break;
            case Item::MEDICINE:
                newItem = new Medicine(name, qty, std::stoi(parts[4]));
                break;
            case Item::MATERIAL:
                newItem = new Material(name, qty, (Material::Rarity)std::stoi(parts[4]));
                break;
            default: break;
        }
        if (newItem)
        {
            newItem->setEnhanceLevel(lvl);
            addItem(newItem);
        }
    }
    return true;
}
