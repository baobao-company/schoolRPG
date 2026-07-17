#ifndef BAG_H
#define BAG_H
#include <string>
#include <vector>
#include <memory>
#include "Item.h"
class Player;
class Item;
class Weapon;
class Armor;
class Material;

class Bag
{
private:
    std::wstring m_owner;
    int m_cap;
    std::vector<Item*> m_items;
public:
    Bag(const std::wstring& owner = L"玩家", int cap = 20);
    ~Bag();
    Bag(const Bag&) = delete;
    Bag& operator=(const Bag&) = delete;
    bool addItem(Item* it);
    bool addItem(std::shared_ptr<Item> it);
    bool removeItem(Item* it, bool del = true);
    bool removeItem(const std::wstring& name, int qty = 1);
    bool useItem(const std::wstring& name, Player* p);
    Item* findItem(const std::wstring& name);
    std::vector<Item*> getByType(Item::ItemType t) const;
    std::vector<Weapon*> getWeapons() const;
    std::vector<Armor*> getArmors() const;
    std::vector<Material*> getMaterials() const;
    std::wstring getAllText() const;
    int totalItemCount() const;
    int typeCount(Item::ItemType t) const;
    int kindCount() const;
    int getCap() const;
    void clear();
    void sortType();
    void sortName();
    std::string serialize() const;
    bool deserialize(const std::string& data);
};
#endif
